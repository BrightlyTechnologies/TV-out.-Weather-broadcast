#pragma once
#include "Font.h"

class CompositeGraphics
{ 
  public:
    int xres;
    int yres;
    char **frame;
    int cursorX, cursorY, cursorBaseX;
    int frontColor, backColor;
    Font<CompositeGraphics> *font;

  CompositeGraphics(int w, int h, int initialTrinagleBufferSize = 0) : xres(w), yres(h) {
    font = 0;
    cursorX = cursorY = cursorBaseX = 0;
    frontColor = 50;
    backColor = -1;
  }

  void init()
  {
    frame = (char**)malloc(yres * sizeof(char*));
    for(int y = 0; y < yres; y++)
    {
      frame[y] = (char*)malloc(xres);
    }
    fillScreen();
  }

  void setTextColor(int front, int back = -1)
  {
    frontColor = front;
    backColor = back;
  }

  void setFont(Font<CompositeGraphics> &font)
  {
    this->font = &font;
  }
  
  void setCursor(int x, int y)
  {
    cursorX = cursorBaseX = x;  
    cursorY = y;  
  }

  void fillScreen(int color = 0) {
    if(color < 0) color = 0;
    else if (color > 54) color = 54;
    for(int y = 0; y < yres; y++)
      for(int x = 0; x < xres; x++)
        frame[y][x] = color;
  }

  void dot(int x, int y, char color)
  {
    if (color < 0) color = 0;
    else if (color > 54) color = 54;
    frame[y][x] = color;
  }

  void dotFast(int x, int y, char color)
  {
    frame[y][x] = color;
  }

  void print(char *str)
  {
    if (!font) return;
    while (*str)
    {
      if ((*str >= 32) && (*str < 128))
        font->drawChar(*this, cursorX, cursorY, *str, frontColor, backColor);
      cursorX += font->xres; 
      if (((cursorX + font->xres) > xres) || (*str == '\n'))
      {
        cursorX = cursorBaseX;
        cursorY += font->yres;        
      }
      str++;
    }
  }

  void print(int number, int base = 10, int minCharacters = 1)
  {
    bool sign = number < 0;
    if(sign) number = -number;
    const char baseChars[] = "0123456789ABCDEF";
    char temp[33];
    temp[32] = 0;
    int i = 31;
    do
    {
      temp[i--] = baseChars[number % base];
      number /= base;
    } while(number > 0);
    if (sign)
      temp[i--] = '-';
    for (;i > 31 - minCharacters; i--)
      temp[i] = ' ';
    print(&temp[i + 1]);
  }

  void fillRect(int x, int y, int w, int h, int color)
  {
    if (x < 0)
    {
      w += x;
      x = 0;
    }
    if (y < 0)
    {
      h += y;
      y = 0;
    }
    if (x + w > xres)
      w = xres - x;
    if (y + h > yres)
      h = yres - y;
    for (int j = y; j < y + h; j++)
      for (int i = x; i < x + w; i++)
        dotFast(i, j, color);
  }

  void drawRect(int x, int y, int w, int h, int color)
  {
    fillRect(x, y, w, 1, color);
    fillRect(x, y, 1, h, color);
    fillRect(x, y + h - 1, w, 1, color);
    fillRect(x + w - 1, y, 1, h, color);
  }
};

