#ifndef SHIFTBRITE_H_AE8JZHFM
#define SHIFTBRITE_H_AE8JZHFM


#include <stdint.h>

// To view what happens while bitbanging exchange these two lines
#define wait_latch 10
//#define wait_latch 1000000

uint32_t convert_color(uint32_t r, uint32_t g, uint32_t b);
uint32_t convert_color_rb(uint32_t r, uint32_t g, uint32_t b);
void rewrite_byteorder(uint32_t* src, uint16_t* dst, int count);
void wait_own(int v);
void init_leds(void);
void latch(void);


#endif /* end of include guard: SHIFTBRITE_H_AE8JZHFM */
