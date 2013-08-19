#include "font.h"
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char uint8_t;
typedef unsigned int  uint32_t;

uint8_t *utf2win(const uint8_t *utf, uint8_t *win)
{
  // Формат русских символов в UTF-8:
  // d0 90 - d0 bf : А-Я, а-п
  // d1 80 - d1 8f : р-я
  // d0 81 , d1 91 : Ё,ё
  // Формат русских символов в CP1251:
  // c0 - df : А-Я
  // e0 - ff : а-я
  // a8 , b8 : Ё,ё
  uint8_t *out = win;
  uint8_t bit;

  while(*utf)
  {
    if(*utf < 0x80) 
      *win++ = *utf++;
    else if (((*utf)&0xFE) == 0xd0)
    {
      bit = (*utf++)&1;
      if (((*utf)&0x91) == ((0x81)|(bit<<4))) // Ёё
        *win++ = (*utf++) + 0x27;
      else
        *win++ = ((*utf++)|(bit<<6)) + 0x30;
    } else {
      // wrong symbol
      utf += 2;
      *win++ = '?';
    }
  }
  *win = 0;
  return out;
}

uint8_t font[256][8] = {0};

#define FB_WIDTH (80)
#define FB_HEIGHT (60)

uint8_t framebuf[FB_WIDTH][FB_HEIGHT] = {' '};
uint32_t x, y;

void put_char(uint8_t c)
{
  uint8_t i, j;
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
      if ((1<<(7-j))&font[c][i])
        framebuf[x+j][y+i] = 'x';
      else
        framebuf[x+j][y+i] = ' ';
  }
  x += 8;
  if ((x+8) >= FB_WIDTH) // Следующий символ не поместится
  {
    x = 0;
    y += 8;
  }
}

void put_s(uint8_t *s)
{
  while(*s)
    put_char(*s++);
}

void load_font()
{
  uint32_t i,j;
  uint8_t bit;

  for (i = 0; i < 128; i++)
    for (j = 0; j < 128; j++)
    {
      bit = header_data[i*128 + j];
      if (bit)
      {
        font[(i>>3)*16 + (j>>3)][i%8] |= (1<<(7 - (j%8)));
      }
    }
}

int main(int argc, char *argv)
{
  x = 0;
  y = 0;
  load_font();
  put_s("THE \"QUICK\" BROWN FOX, 45 JUMPS OVER THE LAZY DOG!");
#if 1
  for (y = 0; y < FB_HEIGHT; y++)
  {
    for (x = 0; x < FB_WIDTH; x++)
      putchar(framebuf[x][y]);
    putchar('\n');
  }
#else
  for (y = 0; y < 16; y++)
  {
    for(x = 0; x < 16; x++)
    {
      putchar(y*16 + x);
      putchar(' ');
    }
    putchar('\n');
  }
#endif
  return 0;
}
