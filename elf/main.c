#include <stdio.h>
#include <stdlib.h>

//#define __USE_GNU
//#include <sys/mman.h>
#include <errno.h>

/*
 *  * Гранулярность буфера
 *   */
#define BUF_GRAN 16384

#define O_RDONLY 00


int start_process(void *buffer);

int main(int argc, char *argv[])
{
  // Буфер под файл
  char *buffer = NULL;

  // Считаем файл в память
  int f = open("test.elf", O_RDONLY);
  int len = lseek(f, 0, SEEK_END);
  lseek(f, 0, SEEK_SET);

  buffer = kmalloc(len);
  read(f, buffer, len);
  close(f);

  start_process(buffer);

  //printf("main : %x\n", main);

  return 0;
}

