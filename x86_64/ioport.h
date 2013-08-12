#ifndef __IOPORT_H__
#define __IOPORT_H__

#include <stdint.h>

void outb(uint16_t port, uint8_t byte);

uint8_t inb(uint16_t port);

#endif //__IOPORT_H__
