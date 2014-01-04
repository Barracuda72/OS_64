/*
 * Это - площадка для проверки драйвера файловой системы
 * в hosted-режиме
 */

#include <vfs.h>
#include <initrd.h>

#include <stdio.h>
#include <stdlib.h>

/*
 * Заглушки для функций ядра
 */

void *kmalloc(uint64_t size)
{
  return malloc(size);
}

void kfree(void *p)
{
  free(p);
}

int main(int argc, char *argv[])
{
  FILE *f = fopen("initrd.img", "rb");
  char *buf = malloc(0x1000);
  int l = fread(buf, 1, 0x1000, f);
  fclose(f);

  vfs_node_t *ird = initrd_init(buf, l);

  char *b = malloc(100);

  int ret = vfs_read(ird, 1, 100, b);

  printf("%d : %s\n", ret, b);

  free(b);
  free(ird);
  free(buf);
  return 0;
}
