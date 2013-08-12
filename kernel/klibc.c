#include <klibc.h>
#include <stdint.h>
#include <ktty.h>

void putdec(uint32_t byte);
void puthexi(uint32_t dword);
void puthex(uint8_t byte);
void puthexd(uint8_t byte);
void vprintf(const char *fmt, va_list args);

void putdec(uint32_t byte)
{
  uint8_t b1;
  int b[30];
  signed int nb;
  int i=0;
  
  while(1){
    b1=byte%10;
    b[i]=b1;
    nb=byte/10;
    if(nb<=0){
      break;
    }
    i++;
    byte=nb;
  }
  
  for(nb=i+1;nb>0;nb--){
    puthexd(b[nb-1]);
  }
}

void puthexi(uint32_t dword)
{
  puthex( (dword & 0xFF000000) >>24);
  puthex( (dword & 0x00FF0000) >>16);
  puthex( (dword & 0x0000FF00) >>8);
  puthex( (dword & 0x000000FF));
}

void puthexl(uint64_t qword)
{
  puthex( (qword & 0xFF00000000000000) >>56);
  puthex( (qword & 0x00FF000000000000) >>48);
  puthex( (qword & 0x0000FF0000000000) >>40);
  puthex( (qword & 0x000000FF00000000) >>32);
  puthex( (qword & 0x00000000FF000000) >>24);
  puthex( (qword & 0x0000000000FF0000) >>16);
  puthex( (qword & 0x000000000000FF00) >>8);
  puthex( (qword & 0x00000000000000FF));
}

void puthex(uint8_t byte)
{
 unsigned  char lb, rb;

  lb=byte >> 4;
  
  rb=byte & 0x0F;
  
  
  puthexd(lb);
  puthexd(rb);
}

void puthexd(uint8_t digit)
{
  char table[]="0123456789ABCDEF";
   ktty_putc(table[digit]);
}

void putbin(uint8_t byte)
{
        int i = 0;
        for (i = 0; i <8; i++)  puthexd(((byte<<i)>>7)&0x01);
}

void printf(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  //textcolor(7);

  vprintf(fmt, args);

  va_end(args);
}

void vprintf(const char *fmt, va_list args)
{
  while (*fmt) {
    
    switch (*fmt) {
    case '%':
      fmt++;

      switch (*fmt) {
      case 's':
        ktty_puts(va_arg(args, char *));
        break;
  
      case 'c':
        ktty_putc(va_arg(args, uint32_t));
        break;
  
      case 'd':
      case 'i':
        putdec(va_arg(args, uint32_t));
        break;

      case 'x':
        puthex(va_arg(args, uint32_t));
        break;
  
      case 'X':
        puthexi(va_arg(args, uint32_t));
        break;

      case 'l':
        puthexl(va_arg(args, uint64_t));
        break;

      case 'b':
        putbin(va_arg(args, uint32_t));
        break;
/*
      case 'z':
        textcolor(va_arg(args,uint32_t));
        break;
*/
      }
      

      break;
      
    default:
      ktty_putc(*fmt);
      break;
    }

    fmt++;
  }
}

char *strncpy(char *dest, char *src, int len)
{
  int i;
  for(i = 0; (i < len)&&(src[i] != 0); i++)
    dest[i] = src[i];
  for(i; i < len; i++)
    dest[i] = 0;
  return dest;
}

void *memset(void *s, int c, uint32_t n)
{ 
  uint8_t *s1 = s;
  uint32_t i;
  for (i = 0; i < n; i++)
    s1[i] = (char)c;
  return s;
}

void *zeromem(void *s, uint32_t n)
{
  return memset(s, 0, n);
}

void *memcpy(void *dest, const void *src, uint32_t n)
{
  char *d = (char *)dest;
  char *s = (char *)src;
  uint32_t i;
  for(i = 0; i < n; i++)
    d[i] = s[i];

  return dest;
}
