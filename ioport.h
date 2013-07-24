#ifndef __IOPORT_H__
#define __IOPORT_H__

void outb(unsigned short port, unsigned char byte);

unsigned char inb(unsigned short port);

#endif //__IOPORT_H__