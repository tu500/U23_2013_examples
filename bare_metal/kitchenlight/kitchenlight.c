#include <stdbool.h>

#include "kitchenlight.h"
#include "shiftbrite.h"
#include "dma_spi.h"

#include "screen_empty.h"
#include "screen_checker.h"

ScreenState* current_screen_state = 0;

//static ScreenConfig* next_screen_config;
static bool exit_screen_state = false;

static uint32_t* volatile next_buffer = 0;
static uint32_t* volatile current_buffer = 0;

static uint32_t data_buffer0[KITCHENLIGHT_BUFFER_SIZE] __attribute__ ((section (".sram.bss")));
static uint32_t data_buffer1[KITCHENLIGHT_BUFFER_SIZE] __attribute__ ((section (".sram.bss")));
static uint32_t reset_buffer[2] __attribute__ ((section (".sram.bss")));

static void assert(bool b);

static uint32_t frame_counter = 0;
static uint32_t reset_count = 0;
static bool resetting = false;
static void init_reset_buffer(void);
static void start_reset(void);
static void start_reset_package(void);



// Interface


// Get the one of the two internal write buffers, that is currently not being
// written.
//  Make sure to call this only if there is no buffer set for the next frame.
//  (ie. only in the 'Draw' method)
uint32_t* get_working_buffer(void)
{
  assert(next_buffer == 0);
  return (current_buffer == data_buffer0) ? data_buffer1 : data_buffer0;
}

// Set the next buffer to render
//  bare version, you need to change the order of the pixels
void set_next_buffer(uint32_t* buffer)
{
  next_buffer = buffer;
}

// Set the next buffer to render
//  automatically copies the buffer to the internal write buffer and change the
//  pixel's order
void set_next_buffer_ro(uint32_t* buffer)
{
  uint32_t* b = get_working_buffer();
  copy_buffer_b2f(buffer, b);
  next_buffer = b;
}

//int ChangeState(ScreenConfig *screen)
//{
//  next_screen_config = ...;
//}

void ExitState(void)
{
  exit_screen_state = true;
}



// Internal affairs


static void start_next_dma_package(void);
ScreenState scr_st; //TODO


void initialize_kitchenlight(void)
{
  init_leds();
  init_spi();
  init_reset_buffer();

  // Set first screen state
  scr_st = (ScreenState) {
    .config = &screenconfig_checker,
    //.config = &screenconfig_empty,
  };
  current_screen_state = &scr_st;

  // Init first screen state
  if (current_screen_state->config->Init)
    current_screen_state->config->Init();
}


void kitchenlight_loop(void)
{
  // Wait until there is no buffer in the dma queue (i.e. VSync)
  do
  {
    if(current_screen_state->config->Update)
      current_screen_state->config->Update();
  } while(next_buffer);

  if(current_screen_state->config->Draw)
    current_screen_state->config->Draw();
  start_reset();

  // screen state management

  start_next_dma_package();
}


// This callback is called after the latch done by the DMA interrupt
void spi_dma_package_done(void)
{
  if (resetting)
  {
    start_reset_package();
  }
  else
  {
    current_buffer = 0;
    frame_counter++;
    start_next_dma_package();
  }
}


// Start a new DMA transfer, if the DMA is not running and there is a pending
//  buffer.
// Should be interrupt-safe.
static void start_next_dma_package(void)
{
  if (next_buffer && !current_buffer)
  {
    current_buffer = next_buffer;
    next_buffer = 0;

    // Start DMA
    configure_dma((uint16_t*) current_buffer, KITCHENLIGHT_BUFFER_SIZE*2);
    start_dma_spi(KITCHENLIGHT_BUFFER_SIZE*2);
  }
}


// Initialize buffer for reset frames
static void init_reset_buffer(void)
{
  uint32_t t = 0x40000000 | 1<<6 | 1<<16 | 1<<26;
  t = (t<<16) | (t>>16); // byte order
  for (int i = 0; i < 2; ++i)
    reset_buffer[i] = t;
}
// Initiate a reset sequence
static void start_reset(void)
{
  reset_count = 0;
  resetting = true;

  // don't do it here, it will automatically be queued after the current package
  //start_reset_package();
}
// The start_next_dma_package equivalent for reset sequences
static void start_reset_package(void)
{
  reset_count++;

  configure_dma((uint16_t*) reset_buffer, 2*2);
  start_dma_spi(2*2);

  if (reset_count++ >= KITCHENLIGHT_BUFFER_SIZE)
  //if (reset_count >= 5)
  {
    resetting = false;
  }
}



// Helpers


// Copy a work buffer to a write buffer for the DMA, changing the order of the
// pixels from a y*x array to the output array as the DMA/SPI expects it.
void copy_buffer_b2f(uint32_t* src, uint32_t* dst)
{
  for (int i=150; i<180; i++)
    *(dst++) = src[i];
  for (int i=149; i>=120; i--)
    *(dst++) = src[i];
  for (int i=90; i<120; i++)
    *(dst++) = src[i];
  for (int i=89; i>=60; i--)
    *(dst++) = src[i];
  for (int i=30; i<60; i++)
    *(dst++) = src[i];
  for (int i=29; i>=0; i--)
    *(dst++) = src[i];
}

static void assert(bool b)
{
  if (!b)
    while(1);
}
