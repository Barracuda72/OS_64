#include <ioport.h>
#include <stdint.h>

void outb(uint16_t port, uint8_t byte)
{
  asm("outb %b0, %w1"::"a"(byte),"d"(port));
}

uint8_t inb(uint16_t port)
{
  uint8_t result;
  asm("inb %w1, %b0":"=a"(result):"d"(port));
  return result;
}


