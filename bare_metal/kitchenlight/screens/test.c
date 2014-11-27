#include "shiftbrite.h"
#include "screens/strobo.h"


static int counter_max = 300;


static void e_draw(void);

ScreenConfig screenconfig_strobo =
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
  counter %= counter_max;

  uint32_t* buffer = get_working_buffer();
  uint16_t v = counter < counter_max/2 ? 1023 : 0;
  uint32_t color = convert_color_rb(v, v, v);
  for (int i = 0; i < KITCHENLIGHT_BUFFER_SIZE; ++i)
  {
    buffer[i] = color;
  }
  set_next_buffer(buffer);
}
