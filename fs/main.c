#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int fs_test_main(void *p, int len);
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

  fs_test_main(buf, l);

  free(buf);
  return 0;
}

