#include <klibc.h>
#include <ktty.h>

void putdec(unsigned int byte);
void puthexi(unsigned int dword);
void puthex(unsigned char byte);
void puthexd(unsigned char byte);
void vprintf(const char *fmt, va_list args);

void putdec(unsigned int byte)
{
  unsigned char b1;
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

void puthexi(unsigned int dword)
{
  puthex( (dword & 0xFF000000) >>24);
  puthex( (dword & 0x00FF0000) >>16);
  puthex( (dword & 0x0000FF00) >>8);
  puthex( (dword & 0x000000FF));
}

void puthexl(unsigned long qword)
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

void puthex(unsigned char byte)
{
 unsigned  char lb, rb;

  lb=byte >> 4;
  
  rb=byte & 0x0F;
  
  
  puthexd(lb);
  puthexd(rb);
}

void puthexd(unsigned char digit)
{
  char table[]="0123456789ABCDEF";
   ktty_putc(table[digit]);
}

void putbin(unsigned char byte)
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
	ktty_putc(va_arg(args, unsigned int));
	break;
	
      case 'd':
      case 'i':
	putdec(va_arg(args, unsigned int));
	break;

      case 'x':
	puthex(va_arg(args, unsigned int));
	break;
	
      case 'X':
	puthexi(va_arg(args, unsigned int));
	break;

      case 'l':
	puthexl(va_arg(args, unsigned long));
	break;

      case 'b':
	putbin(va_arg(args, unsigned int));
	break;

/*      case 'z':
	textcolor(va_arg(args,unsigned int));
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

