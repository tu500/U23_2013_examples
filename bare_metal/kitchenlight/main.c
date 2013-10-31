#include <stdio.h>
#include <stdlib.h>

#include <stm32f4xx.h>

#include "shiftbrite.h"
#include "dma_spi.h"
#include "kitchenlight.h"


#define wait_led 10000000


// Send buffer
// Make sure to always use this attribute. It moves the variable into the sram,
// which is the only section the DMA can access.
static uint32_t data_buffer[4] __attribute__ ((section (".sram.bss")));



int main()
{
  initialize_kitchenlight();

  while (1)
    kitchenlight_loop();

  // Set end notification LED
  GPIO_SetBits(GPIOD, GPIO_Pin_14);

  while(1);
}


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
	ERROR_MSG("Assertion failed in file %s:%d\r\n", file, line);
	while(1);	
}
#endif
