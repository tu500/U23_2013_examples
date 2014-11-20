#include "shiftbrite.h"

#include "pixelfont.h"

// options: fore/background color, write background pixels?, write kerning?

void render_char(char c, uint32_t* buffer, int xpos)
{
  uint8_t width = std_font.chars[(uint8_t)c].width;

  // character offscreen?
  if (xpos <= -width || xpos >= 30)
    return;

  for (int y = 0; y < 6; ++y)
    // start from first screen column or first character column,
    // goto last character or screen column
    for (int x = xpos < 0 ? -xpos : 0; x < width && xpos + x < 30; ++x)
    {
      uint16_t v = std_font.chars[(uint8_t)c].rows[y] & (1<<x) ? 1023 : 0;
      buffer[y * 30 + (xpos + x)] = convert_color_rb(0, v, 0);
    }
}

void render_string(char const *c, uint32_t* buffer, int xpos)
{
  int p = xpos;
  while (p < 30 && *c)
  {
    render_char(*c, buffer, p);
    p += std_font.chars[(uint8_t)(*c)].width;

    // clear kerning
    // TODO optimize this check? don't render for negative coordinates?
    if (p >= 0 && p < 30)
      for (int y = 0; y < 6; ++y)
        buffer[y * 30 + p] = convert_color(0, 0, 0);

    p++; // add kerning
    c++;
  }
}

int get_rendered_string_length(char const *c)
{
  int r = 0;
  while (*c)
  {
    r += std_font.chars[(uint8_t)(*c)].width + 1;
    c++;
  }
  r--; // remove one kerning space
  return r;
}
