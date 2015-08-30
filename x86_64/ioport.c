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

#define PORT 0x3F8 // COM1

void serial_init()
{
  outb(PORT + 1, 0x00); // Отключить прерывания
  outb(PORT + 3, 0x80); // Сообщаем о том, что намерены установить делитель
  outb(PORT + 0, 0x03); // Младший байт
  outb(PORT + 1, 0x00); // Старший байт
  outb(PORT + 3, 0x03); // 8 бит, без четности, один стоповый бит
  outb(PORT + 2, 0xC7); // Включить FIFO

}

void serial_write(char a)
{
  while (!(inb(PORT + 5) & 0x20));
  outb(PORT, a);
}
