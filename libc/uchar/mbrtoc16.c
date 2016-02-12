/*
 * mbrtoc16.c
 *
 * Часть стандарта C2011
 *
 * Функция преобразует некоторый символ строки, возможно, задаваемый 
 * параметром ps, в 16-битный символ Unicode
 *
 */

#include <decl/NULL.h>

#include <uchar.h>

#include <errno.h>

enum
{
  S_INIT,
  S_ONEBYTE
};

size_t mbrtoc16(char16_t * restrict pc16,
                const char * restrict s, size_t n,
                mbstate_t * restrict ps)
{
  char16_t res;

  if (s == NULL)
  {
    ps->st = S_INIT;
    ps->pos = 0;
    return 0;
  } else {
    switch (n)
    {
      case 0:
        errno = EILSEQ;
        return (size_t)-1;

      case 1:
      default:
        if (ps->st == S_ONEBYTE)
        {
          res = ps->ch | s[ps->pos++] << 8;
          ps->ch = 0;
          ps->st = S_INIT;
          *pc16 = res;
          return (res == 0);
        } else {
          if (n == 1)
          {
            ps->ch = s[ps->pos++];
            ps->st = S_ONEBYTE;
            return (size_t)(-2);
          } else {
            *pc16 = *((char16_t *) s + ps->pos);
            ps->pos += 2;
            return 2;
          }
        }
    }
  }
}

