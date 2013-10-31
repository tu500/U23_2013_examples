#include "shiftbrite.h"
#include "screen_empty.h"


static void e_draw(void);

ScreenConfig screenconfig_empty = 
{
  //.Init
  //.Deinit
  //.Update
  .Draw = e_draw,
  //.OnDelegate
  //.OnResume
  //.OnPause
  //.OnUnpause
};

static int counter = 0;

static void e_draw(void)
{
  counter++;
  counter %= 300;

  uint32_t* buffer = get_working_buffer();
  uint32_t v = convert_color_rb(counter < 150 ? 1023 : 0, 0, 0);
  for (int i = 0; i < KITCHENLIGHT_BUFFER_SIZE; ++i)
  {
    buffer[i] = v;
  }
  set_next_buffer(buffer);
}
