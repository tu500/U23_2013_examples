#ifndef PIXELFONT_H_5UH0YGHD
#define PIXELFONT_H_5UH0YGHD

#include <stdint.h>

struct Pixelfont_Char
{
  uint8_t width;
  uint8_t rows[6]; // bitfield: lsb is left-most pixel
};

struct Pixelfont
{
  struct Pixelfont_Char chars[256];
};

extern struct Pixelfont std_font;


void render_char(char c, uint32_t* buffer, int xpos);
void render_string(char const *c, uint32_t* buffer, int xpos);
int get_rendered_string_length(char const *c);

#endif /* end of include guard: PIXELFONT_H_5UH0YGHD */

