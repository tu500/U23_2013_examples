#include <stm32f4xx.h>

#include "rng.h"


void init_rng(void)
{
  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
  RNG_Cmd(ENABLE);
  RNG_ITConfig(DISABLE);
}

uint32_t get_random_int(void)
{
  while (RNG_GetFlagStatus(RNG_FLAG_DRDY) == RESET)
    ;
  return RNG_GetRandomNumber();
}
