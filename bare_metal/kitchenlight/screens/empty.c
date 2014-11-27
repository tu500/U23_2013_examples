#include "shiftbrite.h"
#include "screens/empty.h"


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

static void e_draw(void)
{
  uint32_t* buffer = get_working_buffer();
  uint32_t v = convert_color_rb(0, 0, 0);
  for (int i = 0; i < KITCHENLIGHT_BUFFER_SIZE; ++i)
  {
    buffer[i] = v;
  }
  set_next_buffer(buffer);
}
