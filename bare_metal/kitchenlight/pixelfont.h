#ifndef PIXELFONT_H_5UH0YGHD
#define PIXELFONT_H_5UH0YGHD

struct Pixelfont_Char
{
  uint8_t width;
  uint8_t rows[6];
};

struct Pixelfont
{
  struct Pixelfont_Char chars[256];
};

extern Pixelfont std_font;


void render_char(char c, uint32_t* buffer, int xpos);
void render_string(char c, uint32_t* buffer, int xpos);

#endif /* end of include guard: PIXELFONT_H_5UH0YGHD */

