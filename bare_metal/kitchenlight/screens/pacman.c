#include "shiftbrite.h"
#include "screens/pacman.h"

#define K 0
#define R 0xfc00000f
#define Y 0xffaa000f
#define B 0x00003ff0
static uint32_t pacman[4][KITCHENLIGHT_BUFFER_SIZE] = {
  {
    K,K,K,K,R,R,R,K,K,K,K,K,Y,Y,Y,K,K,K,K,K,K,K,K,B,B,B,K,K,K,K,
    K,K,K,R,K,K,K,R,K,K,K,Y,Y,K,Y,Y,K,K,K,K,K,K,B,K,K,K,B,K,K,K,
    K,K,R,R,K,K,K,K,K,K,Y,Y,Y,Y,Y,Y,K,K,K,K,K,B,B,K,K,K,K,K,K,K,
    K,K,R,R,K,K,K,K,K,K,Y,Y,Y,Y,Y,Y,K,K,K,K,K,B,B,K,K,K,K,K,K,K,
    K,K,K,R,K,K,K,R,K,K,K,Y,Y,Y,Y,Y,K,K,K,K,K,K,B,K,K,K,B,K,K,K,
    K,K,K,K,R,R,R,K,K,K,K,K,Y,Y,Y,K,K,K,K,K,K,K,K,B,B,B,K,K,K,K
  },
  {
    K,K,K,K,R,R,R,K,K,K,K,K,Y,Y,Y,K,K,K,K,K,K,K,K,B,B,B,K,K,K,K,
    K,K,K,R,K,K,K,R,K,K,K,Y,Y,K,Y,Y,K,K,K,K,K,K,B,K,K,K,B,K,K,K,
    K,K,R,R,K,K,K,K,K,K,Y,Y,Y,Y,Y,Y,K,K,K,K,K,B,B,K,K,K,K,K,K,K,
    K,K,R,R,K,K,K,K,K,K,Y,Y,Y,Y,K,K,K,K,K,K,K,B,B,K,K,K,K,K,K,K,
    K,K,K,R,K,K,K,R,K,K,K,Y,Y,Y,Y,Y,K,K,K,K,K,K,B,K,K,K,B,K,K,K,
    K,K,K,K,R,R,R,K,K,K,K,K,Y,Y,Y,K,K,K,K,K,K,K,K,B,B,B,K,K,K,K
  },
  {
    K,K,K,K,R,R,R,K,K,K,K,K,Y,Y,Y,K,K,K,K,K,K,K,K,B,B,B,K,K,K,K,
    K,K,K,R,K,K,K,R,K,K,K,Y,Y,K,Y,Y,K,K,K,K,K,K,B,K,K,K,B,K,K,K,
    K,K,R,R,K,K,K,K,K,K,Y,Y,Y,Y,Y,K,K,K,K,K,K,B,B,K,K,K,K,K,K,K,
    K,K,R,R,K,K,K,K,K,K,Y,Y,Y,Y,K,K,K,K,K,K,K,B,B,K,K,K,K,K,K,K,
    K,K,K,R,K,K,K,R,K,K,K,Y,Y,Y,Y,K,K,K,K,K,K,K,B,K,K,K,B,K,K,K,
    K,K,K,K,R,R,R,K,K,K,K,K,Y,Y,Y,Y,K,K,K,K,K,K,K,B,B,B,K,K,K,K
  },
  {
    K,K,K,K,R,R,R,K,K,K,K,K,Y,Y,Y,K,K,K,K,K,K,K,K,B,B,B,K,K,K,K,
    K,K,K,R,K,K,K,R,K,K,K,Y,Y,K,Y,Y,K,K,K,K,K,K,B,K,K,K,B,K,K,K,
    K,K,R,R,K,K,K,K,K,K,Y,Y,Y,Y,Y,Y,K,K,K,K,K,B,B,K,K,K,K,K,K,K,
    K,K,R,R,K,K,K,K,K,K,Y,Y,Y,Y,K,K,K,K,K,K,K,B,B,K,K,K,K,K,K,K,
    K,K,K,R,K,K,K,R,K,K,K,Y,Y,Y,Y,Y,K,K,K,K,K,K,B,K,K,K,B,K,K,K,
    K,K,K,K,R,R,R,K,K,K,K,K,Y,Y,Y,K,K,K,K,K,K,K,K,B,B,B,K,K,K,K
  },
};
#undef K
#undef R
#undef Y
#undef B

static void e_draw(void)
{
  static unsigned char i = 0;
  i = (i+1) % 120;
  unsigned char j = i / 30;
  set_next_buffer_ro(pacman[j]);
}

ScreenConfig screenconfig_pacman =
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

