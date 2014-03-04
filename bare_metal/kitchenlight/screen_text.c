#include "shiftbrite.h"
#include "pixelfont.h"
#include "screen_text.h"

#define state_data ((struct screen_text_data*) current_screen_state.user_data)


static char const *default_string = "Hello World!";
static int counter = 0;
static const int multiplier = 256;


static void e_init(void);
static void e_draw(void);

ScreenConfig screenconfig_text =
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
  if (!user_data) // TODO
    user_data = malloc(sizeof(struct screen_text_data));

  counter = 30 * multiplier;
  if (!user_data->string)
    user_data->string = default_string;
}

static void e_draw(void)
{
  counter--;
  if (counter < -get_rendered_string_length(state_data->string) * multiplier)
    counter = 30 * multiplier;

  uint32_t* buffer = get_working_buffer();
  render_string(state_data->string, buffer, counter / multiplier);
  set_next_buffer(buffer);
}
