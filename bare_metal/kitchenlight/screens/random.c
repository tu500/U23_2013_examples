#include "shiftbrite.h"
#include "screens/strobo.h"
#include "rng.h"


static uint32_t counter_max = 300;
static uint16_t r,g,b;
static uint16_t r2,g2,b2;


static void e_init(void);
static void e_draw(void);

ScreenConfig screenconfig_random =
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
  init_rng();
}

static uint32_t counter = 0;
static uint32_t color;

static void e_draw(void)
{
  counter++;
  counter %= counter_max;

  uint32_t* buffer = get_working_buffer();
  uint32_t rand = get_random_int();
  if (counter == 0)
  {
    r2 = r;
    g2 = g;
    b2 = b;
    r = rand & 0x3ff;
    g = rand>>10 & 0x3ff;
    b = rand>>20 & 0x3ff;
  }
  color = convert_color_rb(
      (counter * r + (counter_max - counter) * r2) / counter_max,
      (counter * g + (counter_max - counter) * g2) / counter_max,
      (counter * b + (counter_max - counter) * b2) / counter_max
      );
  for (int i = 0; i < KITCHENLIGHT_BUFFER_SIZE; ++i)
  {
    buffer[i] = color;
  }
  set_next_buffer(buffer);
}
