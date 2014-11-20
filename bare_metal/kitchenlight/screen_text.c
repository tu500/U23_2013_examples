#include <stdlib.h>

#include "shiftbrite.h"
#include "pixelfont.h"
#include "screen_text.h"

#define state_data ((struct screen_text_data*) current_screen_state->user_data)


static char const *default_string = "Hello World!";
static int counter = 0;
static const int multiplier = 8;
//static const int multiplier = 256;
static uint32_t temp_buffer[KITCHENLIGHT_BUFFER_SIZE] __attribute__ ((section (".sram.bss")));


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


uint8_t data[sizeof(struct screen_text_data)];


static void e_init(void)
{
  if (!state_data) // TODO
    //current_screen_state->user_data = malloc(sizeof(struct screen_text_data));
    current_screen_state->user_data = &data;

  counter = 30 * multiplier;
  if (!state_data->string)
    state_data->string = default_string;
}

static void e_draw(void)
{
  counter--;
  if (counter < -get_rendered_string_length(state_data->string) * multiplier)
    counter = 30 * multiplier;

  // clear buffer
  for (int i = 0; i < KITCHENLIGHT_BUFFER_SIZE; ++i)
    temp_buffer[i] = 0;

  //render_string(state_data->string, temp_buffer, counter / multiplier);
  render_string(default_string, temp_buffer, counter / multiplier);

  set_next_buffer_ro(temp_buffer);
}
