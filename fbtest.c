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

uint8_t ch[1][8] = 
{
  {
    0x18, // 00011000
    0x24, // 00100100
    0x42, // 01000010
    0x42, // 01000010
    0x7E, // 01111110
    0x42, // 01000010
    0x42, // 01000010
    0x42  // 01000010
  }
};

uint8_t buf[80][25] = {' '};
uint8_t x, y;

void put_char(uint8_t c)
{
  uint8_t i, j;
  for (i = 0; i < 8; i++)
  {
    for (j = 0; j < 8; j++)
      if ((1<<j)&ch[c][i])
        buf[x+j][y+i] = 'x';
      else
        buf[x+j][y+i] = ' ';
  }
  x += 8;
  if (x >= 80)
  {
    x = 0;
    y += 8;
  }
}

int main(int argc, char *argv)
{
  x = 0;
  y = 0;
  put_char(0);

  for (y = 0; y < 25; y++)
  {
    for (x = 0; x < 80; x++)
      putchar(buf[x][y]);
    putchar('\n');
  }
  return 0;
}
