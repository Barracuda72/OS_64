#include <ioport.h>

void outb(unsigned short port, unsigned char byte)
{
	asm("outb %b0, %w1"::"a"(byte),"d"(port));
}

unsigned char inb(unsigned short port)
{
	unsigned char result;
	asm("inb %w1, %b0":"=a"(result):"d"(port));
	return result;
}


