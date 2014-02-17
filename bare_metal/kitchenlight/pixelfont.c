#include "shiftbrite.h"

#include "pixelfont.h"

// options: fore/background color, write background pixels?, write kerning?

void render_char(char c, uint32_t* buffer, int xpos)
{
  uint8_t width = std_font.chars[c].width;

  if (xpos <= -width || xpos >= 30)
    return;

  for (int j = 0; j < 6; ++j)
    for (int x = -xpos; x < width; ++x)
    {
      uint16_t v = std_font.chars[c].rows[y] & (1<<x) ? 1023 : 0;
      buffer[y * 30 + (xpos + x)] = convert_color(v, v, v);
    }
}

void render_string(char c, uint32_t* buffer, int xpos)
{
  int p = xpos;
  while (p < 30)
  {
    render_char(c, buffer, p);
    p += std_font.chars[c].width + 1;
  }
}
