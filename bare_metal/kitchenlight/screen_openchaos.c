#include <stdlib.h>

#include "shiftbrite.h"
#include "pixelfont.h"
#include "screen_openchaos.h"


static char const *strings[] = {
  "Open",
  "Chaos",
  "<<<<<",
};
static int counter = 0;
static const int counter_max = 90;
static uint32_t temp_buffer[KITCHENLIGHT_BUFFER_SIZE] __attribute__ ((section (".sram.bss")));


static void e_init(void);
static void e_draw(void);

ScreenConfig screenconfig_openchaos =
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


static void e_init(void)
{
  counter = 0;
}

static void e_draw(void)
{
  counter++;
  counter %= 90;

  // clear buffer
  for (int i = 0; i < KITCHENLIGHT_BUFFER_SIZE; ++i)
    temp_buffer[i] = 0;

  render_string(strings[counter / (counter_max/3)], temp_buffer, 3);

  set_next_buffer_ro(temp_buffer);
}
