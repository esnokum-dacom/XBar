#include "drw.h"

void draw_wrapped_text(Display *dpy, XftDraw *xdraw, XftFont *font,
                       XftColor *color, int x, int y, int max_width,
                       const char *text) {

  char line[1280] = {0};
  char word[256];
  char test[1280];

  int line_h = font->ascent + font->descent + 2;
  int cy = y;
  const char *p = text;

  while (*p) {

    int i = 0;

    while (*p && *p != ' ' && *p != '\n')
      word[i++] = *p++;

    word[i] = '\0';

    if (strlen(line) == 0)
      snprintf(test, sizeof(test), "%s", word);
    else
      snprintf(test, sizeof(test), "%s %s", line, word);

    XGlyphInfo ext;
    XftTextExtentsUtf8(dpy, font, (FcChar8 *)test, strlen(test), &ext);

    if (ext.xOff > max_width) {

      XftDrawStringUtf8(xdraw, color, font, x, cy, (FcChar8 *)line,
                        strlen(line));
      cy += line_h;
      snprintf(line, sizeof(line), "%s", word);
    } else
      snprintf(line, sizeof(line), "%s", test);

    if (*p == ' ')
      p++;

    if (*p == '\n') {
      XftDrawStringUtf8(xdraw, color, font, x, cy, (FcChar8 *)line,
                        strlen(line));
      cy += line_h;
      line[0] = '\0';
      p++;
    }
  }

  if (strlen(line) > 0) {
    XftDrawStringUtf8(xdraw, color, font, x, cy, (FcChar8 *)line, strlen(line));
  }
}

int count_wrapped_lines(Display *dpy, XftFont *font, int max_width,
                        const char *text) {
  char line[1280] = {0};
  char word[256];
  char test[1280];

  int lines = 1;
  const char *p = text;

  while (*p) {
    int i = 0;

    while (*p && *p != ' ' && *p != '\n')
      word[i++] = *p++;

    word[i] = '\0';
    if (strlen(line) == 0)
      snprintf(test, sizeof(test), "%s", word);
    else
      snprintf(test, sizeof(test), "%s %s", line, word);

    XGlyphInfo ext;
    XftTextExtentsUtf8(dpy, font, (FcChar8 *)test, strlen(test), &ext);

    if (ext.xOff > max_width) {
      lines++;
      snprintf(line, sizeof(line), "%s", word);
    } else
      snprintf(line, sizeof(line), "%s", test);

    if (*p == ' ')
      p++;

    if (*p == '\n') {
      lines++;
      line[0] = '\0';
      p++;
    }
  }

  return lines;
}

void draw_text(Display *dpy, XftDraw *xdraw, XftFont *font,
               XftColor *title_color, int x, int y, const char *title,
               int16_t type) {

  XGlyphInfo extents;
  XftTextExtentsUtf8(dpy, font, (FcChar8 *)title, strlen(title), &extents);

  int w_s = extents.xOff;

  if (type == 1)
    XftDrawStringUtf8(xdraw, title_color, font, x - w_s, y, (FcChar8 *)title,
                      strlen(title));
  else if (type == 2)
    XftDrawStringUtf8(xdraw, title_color, font, x + w_s, y, (FcChar8 *)title,
                      strlen(title));
  else if (type == 3)
    XftDrawStringUtf8(xdraw, title_color, font, x - (w_s / 2), y,
                      (FcChar8 *)title, strlen(title));
  else if (type == 0)
    XftDrawStringUtf8(xdraw, title_color, font, x, y, (FcChar8 *)title,
                      strlen(title));
}
