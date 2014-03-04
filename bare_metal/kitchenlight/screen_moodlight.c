#include "shiftbrite.h"
#include "screen_strobo.h"


static int counter_max = 300;


static void e_draw(void);

ScreenConfig screenconfig_strobo =
{
  .Init
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
  if (!user_data) // TODO
    user_data = malloc(sizeof(struct screen_moodlight_data));
}

static void e_draw(void)
{
  uint32_t* buffer = get_working_buffer();
  uint32_t color = convert_color_rb(user_data->ch_red, user_data->ch_green, user_data->ch_blue);
  for (int i = 0; i < KITCHENLIGHT_BUFFER_SIZE; ++i)
  {
    buffer[i] = color;
  }
  set_next_buffer(buffer);
}
