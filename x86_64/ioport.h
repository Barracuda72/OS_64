#ifndef __IOPORT_H__
#define __IOPORT_H__

#include <stdint.h>

void outb(uint16_t port, uint8_t byte);

uint8_t inb(uint16_t port);

void serial_init();

void serial_write(char a);

#endif //__IOPORT_H__
