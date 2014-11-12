#include <stdbool.h>
#include <stm32f4xx.h>

#include "shiftbrite.h"
#include "dma_spi.h"

static uint32_t convenient_buffer __attribute__ ((section (".sram.bss")));
static bool spi_running[3] = {false, false, false};


// Initialize SPI
void init_spi(void)
{
  // Enable clock for SPI hardware, GPIO port B, and DMA
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

  // SPI1 (SCK PB3, MOSI PB5)
  GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
      .GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_5,
      .GPIO_Mode = GPIO_Mode_AF,
      .GPIO_Speed = GPIO_Speed_50MHz,
    });

  // SPI2 (SCK PB10, MOSI PB15)
  GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
      .GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_15,
      .GPIO_Mode = GPIO_Mode_AF,
      .GPIO_Speed = GPIO_Speed_50MHz,
    });

  // SPI3 (SCK PC10, MOSI PC12)
  GPIO_Init(GPIOC, &(GPIO_InitTypeDef){
      .GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_12,
      .GPIO_Mode = GPIO_Mode_AF,
      .GPIO_Speed = GPIO_Speed_50MHz,
    });

  // Configure pins to be used by the SPI hardware (alternate function)
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SPI3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SPI3);

  // Latch pin
  GPIO_Init(GPIOB, &(GPIO_InitTypeDef){
      .GPIO_Pin = GPIO_Pin_12,
      .GPIO_Mode = GPIO_Mode_OUT,
      .GPIO_Speed = GPIO_Speed_50MHz,
    });

  // reset latch
  GPIO_WriteBit(GPIOB, GPIO_Pin_12, RESET);

  // Init SPI
  //
  // SPI can only work with 8 and 16 bit data packets, we use 16 to minimize
  // interrupts. This means we have to split up uint32_t's (-> sentUpperWord).
  SPI_InitTypeDef spi_init_struct = {
      //.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16, // Configure Data speed
      .SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256, // Configure Data speed
      .SPI_CPHA = SPI_CPHA_1Edge, // Sample data on rising edge
      .SPI_CPOL = SPI_CPOL_Low, // Clock is default low
      .SPI_CRCPolynomial = 1, // Don't use CRC
      .SPI_DataSize = SPI_DataSize_16b, // Send 16 bit words at a time
      .SPI_Direction = SPI_Direction_1Line_Tx, // Only enable sending (transmission/TX)
      .SPI_FirstBit = SPI_FirstBit_MSB, // Most Significant Bit first
      .SPI_Mode = SPI_Mode_Master, // STM32 is the master
      .SPI_NSS = SPI_NSS_Soft, // Don't use automatic chip select
    };
  SPI_Init(SPI1, &spi_init_struct);
  SPI_Init(SPI2, &spi_init_struct);
  SPI_Init(SPI3, &spi_init_struct);


  // Tell the SPI hardware to request data from the DMA
  SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
  SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
  SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);

  // Enable SPI hardware
  SPI_Cmd(SPI1, ENABLE);
  SPI_Cmd(SPI2, ENABLE);
  SPI_Cmd(SPI3, ENABLE);


  // Configure interrupts for the DMA (transfer complete)...
  NVIC_Init(&(NVIC_InitTypeDef){
      .NVIC_IRQChannel = DMA2_Stream5_IRQn,
      .NVIC_IRQChannelPreemptionPriority = 0,
      .NVIC_IRQChannelSubPriority = 1,
      .NVIC_IRQChannelCmd = ENABLE,
    });
  NVIC_Init(&(NVIC_InitTypeDef){
      .NVIC_IRQChannel = DMA1_Stream4_IRQn,
      .NVIC_IRQChannelPreemptionPriority = 0,
      .NVIC_IRQChannelSubPriority = 1,
      .NVIC_IRQChannelCmd = ENABLE,
    });
  NVIC_Init(&(NVIC_InitTypeDef){
      .NVIC_IRQChannel = DMA1_Stream5_IRQn,
      .NVIC_IRQChannelPreemptionPriority = 0,
      .NVIC_IRQChannelSubPriority = 1,
      .NVIC_IRQChannelCmd = ENABLE,
    });

  // ... and SPI (TXE, see ISR for explanation)
  NVIC_Init(&(NVIC_InitTypeDef){
      .NVIC_IRQChannel = SPI1_IRQn,
      .NVIC_IRQChannelPreemptionPriority = 0,
      .NVIC_IRQChannelSubPriority = 0,
      .NVIC_IRQChannelCmd = ENABLE,
    });
  NVIC_Init(&(NVIC_InitTypeDef){
      .NVIC_IRQChannel = SPI2_IRQn,
      .NVIC_IRQChannelPreemptionPriority = 0,
      .NVIC_IRQChannelSubPriority = 0,
      .NVIC_IRQChannelCmd = ENABLE,
    });
  NVIC_Init(&(NVIC_InitTypeDef){
      .NVIC_IRQChannel = SPI3_IRQn,
      .NVIC_IRQChannelPreemptionPriority = 0,
      .NVIC_IRQChannelSubPriority = 0,
      .NVIC_IRQChannelCmd = ENABLE,
    });

  // Configure the DMA temporarilly with 0-length buffer
  configure_dma(0, 0);

  // Enable the DMA TC (transmission complete) interrupt
  DMA_ITConfig(DMA2_Stream5, DMA_IT_TC, ENABLE);
  DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);
  DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);
}

// Start the SPI/DMA, transmit 'data_count' uint16_t's
//
//  data_count in uint16_t units (2 byte)
void start_dma_spi(int data_count)
{
  // First reset DMA
  DMA_Cmd(DMA2_Stream5, DISABLE);
  DMA_Cmd(DMA1_Stream4, DISABLE);
  DMA_Cmd(DMA1_Stream5, DISABLE);

  // Set the current buffer length
  DMA_SetCurrDataCounter(DMA2_Stream5, data_count);
  DMA_SetCurrDataCounter(DMA1_Stream4, data_count);
  DMA_SetCurrDataCounter(DMA1_Stream5, data_count);

  // Reset Latch
  GPIO_WriteBit(GPIOB, GPIO_Pin_12, RESET);

  spi_running[0] = spi_running[1] = spi_running[2] = true;

  // Start DMA
  DMA_Cmd(DMA2_Stream5, ENABLE);
  DMA_Cmd(DMA1_Stream4, ENABLE);
  DMA_Cmd(DMA1_Stream5, ENABLE);
}

// Set which buffer the DMA will use, and its size.
//
//  buffer_size in uint16_t units (2 byte)
//  and only a third of what is actually there
//  (ie the size going out one SPI bus)
void configure_dma(uint16_t* buffer, int buffer_size)
{
  // Configure the DMA channel to move the right data in the right direction
  // SPI1
  DMA_Init(DMA2_Stream5, &(DMA_InitTypeDef){
      .DMA_Channel = DMA_Channel_3,
      .DMA_BufferSize = buffer_size,
      .DMA_DIR = DMA_DIR_MemoryToPeripheral,
      .DMA_Memory0BaseAddr = (uint32_t)buffer,
      .DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord,
      .DMA_MemoryInc = DMA_MemoryInc_Enable,
      .DMA_Mode = DMA_Mode_Normal,
      .DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR,
      .DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord,
      .DMA_PeripheralInc = DMA_PeripheralInc_Disable,
      .DMA_Priority = DMA_Priority_High,
    });
  // SPI2
  DMA_Init(DMA1_Stream4, &(DMA_InitTypeDef){
      .DMA_Channel = DMA_Channel_0,
      .DMA_BufferSize = buffer_size,
      .DMA_DIR = DMA_DIR_MemoryToPeripheral,
      .DMA_Memory0BaseAddr = (uint32_t)(buffer + buffer_size),
      .DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord,
      .DMA_MemoryInc = DMA_MemoryInc_Enable,
      .DMA_Mode = DMA_Mode_Normal,
      .DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR,
      .DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord,
      .DMA_PeripheralInc = DMA_PeripheralInc_Disable,
      .DMA_Priority = DMA_Priority_High,
    });
  // SPI3
  DMA_Init(DMA1_Stream5, &(DMA_InitTypeDef){
      .DMA_Channel = DMA_Channel_0,
      .DMA_BufferSize = buffer_size,
      .DMA_DIR = DMA_DIR_MemoryToPeripheral,
      .DMA_Memory0BaseAddr = (uint32_t)(buffer + 2*buffer_size),
      .DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord,
      .DMA_MemoryInc = DMA_MemoryInc_Enable,
      .DMA_Mode = DMA_Mode_Normal,
      .DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR,
      .DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord,
      .DMA_PeripheralInc = DMA_PeripheralInc_Disable,
      .DMA_Priority = DMA_Priority_High,
    });
}

// Convenience function to write a uint32_t out, using an internal buffer.
void spi_write_int32(uint32_t data)
{
  //TODO really needed anymore?
return;
  // Write to internal buffer
  convenient_buffer = data;
  // Change byteorder
  rewrite_byteorder(&convenient_buffer, (uint16_t*) &convenient_buffer, 1);
  // Set to use the internal buffer
  configure_dma((uint16_t*) &convenient_buffer, 2);
  // Start the DMA/SPI
  start_dma_spi(2);
}



// DMA INTERRUPT


void DMA2_Stream5_IRQHandler(void){
  // Check for interrupt cause (transmission complete)
  if(DMA_GetITStatus(DMA2_Stream5, DMA_IT_TCIF5) == SET)
  {
    // The DMA said 'transmission complete'. This means it just threw the last word
    // into the SPI TX buffer. But the SPI is not done sending yet. So enable the
    // TXE (transmit buffer empty) interrupt.
    SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE);

    //Clear the DMA Transfer Complete flag
    DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF5);
  }
}

void DMA1_Stream4_IRQHandler(void){
  if(DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) == SET)
  {
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, ENABLE);
    DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
  }
}

void DMA1_Stream5_IRQHandler(void){
  if(DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5) == SET)
  {
    SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, ENABLE);
    DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TCIF5);
  }
}



// SPI INTERRUPT


static void spi_handleSPI_TxeInterrupt(void);

void SPI1_IRQHandler(void){
  // Check for interrupt cause (TX buffer empty)
  if(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == SET){
    spi_running[0] = false;
    spi_handleSPI_TxeInterrupt();
  }
}

void SPI2_IRQHandler(void){
  if(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == SET){
    spi_running[1] = false;
    spi_handleSPI_TxeInterrupt();
  }
}

void SPI3_IRQHandler(void){
  if(SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == SET){
    spi_running[2] = false;
    spi_handleSPI_TxeInterrupt();
  }
}


// This callback is defined in kitchenlight.c
void spi_dma_package_done(void);

// The SPI said 'TX buffer empty'. However it is not done sending yet. It still
// is shifting data bits out of its internal shift register. So wait until it is
// done finally.
static void spi_handleSPI_TxeInterrupt(void){
  // Wait until the SPI is _really_ done
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET
      || SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET
      || SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_BSY) == SET)
    ;

  // Disable the TXE interrupt
  SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, DISABLE);
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_TXE, DISABLE);
  SPI_I2S_ITConfig(SPI3, SPI_I2S_IT_TXE, DISABLE);

  // Do a latch
  latch();

  // kitchenlight callback
  spi_dma_package_done();
}
