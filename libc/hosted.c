/*
 * hosted.c
 *
 * Недостающие функции ядра для hosted-режима
 *
 */

#include <stdint.h>

#define MAVAIL __POOL_SIZE__

static char internal[MAVAIL+0x1000];
static const int mavail = MAVAIL;
static int mpos = 0;

void *kmalloc(size_t size)
{
  if (mpos >= mavail)
    return 0;

  long internal_addr = ((long)internal & (~0xFFF)) + 0x1000;

  size = (size&(~0xFFF)) + 0x1000;

  long ret = internal_addr + mpos;
  mpos += size;

  return ret;
}

void kfree(void *p)
{
  // Ничего не делаем
}

void ktty_puts(const char *s)
{
  size_t len = strlen(s);
  char *nl = "\n";

  write(1, s, len);
  write(1, nl, 1);
}

void ktty_putc(char c)
{
	char data[1];
	data[0] = c;
	write(1, data, 1);
}

void remap_pages(void *oldaddr, void *newaddr, uint64_t size)
{
  mremap(oldaddr, size, size, 3, newaddr);
}

