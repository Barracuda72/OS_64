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

int main(int argc, char *argv)
{
  char *u = "Test string: В чащах юга жил бы цитрус? Да, но фальшивый экземпляр! Ёж, ну чё, пойдём?\n";
  char *t = malloc(strlen(u)+1);
  utf2win(u, t);
  puts(t);
  return 0;
}
