#include "shiftbrite.h"
#include "screen_matrix.h"
#include "rng.h"


static uint32_t temp_buffer[KITCHENLIGHT_BUFFER_SIZE] __attribute__ ((section (".sram.bss")));


typedef struct
{
  uint8_t pos_x;
  uint32_t pos_y; // in 256-th pixel
  uint8_t speed; // in 256-th pixel per frame
  uint8_t length;
} falling_pixel;

static const int PIXEL_RESOLUTION = 256;
static const int PIXEL_COUNT = 32;
static falling_pixel pixels[32];


static void e_init(void);
static void e_draw(void);

ScreenConfig screenconfig_matrix =
{
  .Init = e_init,
  //.Deinit
  //.Update
  .Draw = e_draw,
  //.OnDelegate
  //.OnResume
  //.OnPause
  //.OnUnpause
};

static void make_new_pixel(falling_pixel *p)
{
  uint32_t r = get_random_int();
  p->pos_x = (r & 0xff) % 30; // (0 to 255) % 30
  p->pos_y = 0;
  p->speed = (r>>8 & 0x3f) + 10; // (0 to 63) + 10
  p->length = (r>>16 & 0x1f) + 4; // (0 to 31) + 4
}

static void e_init(void)
{
  init_rng();

  for (int i = 0; i < PIXEL_COUNT; ++i)
    make_new_pixel(&pixels[i]);
}

static void e_draw(void)
{
  // clear buffer
  for (int i = 0; i < KITCHENLIGHT_BUFFER_SIZE; ++i)
    temp_buffer[i] = 0;

  for (int i = 0; i < PIXEL_COUNT; ++i)
  {
    pixels[i].pos_y += pixels[i].speed;

    if (pixels[i].pos_y / PIXEL_RESOLUTION >= pixels[i].length + 6)
      make_new_pixel(&pixels[i]);

    for (uint32_t y = 0; y < 6; ++y)
    {
      // the physical pixel's position in the falling stream
      uint32_t pos = pixels[i].pos_y - (y * PIXEL_RESOLUTION);

      // this means the matrix pixel already has fallen past the physical pixel
      // or if pos is "negative" (which also fulfills the inequality) it hasn't
      // yet reached the physical pixel
      if (pos >= (uint32_t)pixels[i].length * PIXEL_RESOLUTION)
	continue;


      uint32_t v = 1024 - pos * 992 / ((uint32_t)pixels[i].length * PIXEL_RESOLUTION);
      uint32_t o = v * ((pixels[i].speed - 10) + 1) / (4*32*2);
      //uint32_t o = v/16;
      temp_buffer[y*30 + pixels[i].pos_x] = convert_color_rb(o, v, o);
    }
  }

  set_next_buffer_ro(temp_buffer);
}
