//#include <stdio.h>
//#include <stdlib.h>

//#define __USE_GNU
//#include <sys/mman.h>
//#include <errno.h>

#include <stdint.h>

/*
 *  * Гранулярность буфера
 *   */
#define BUF_GRAN 16384

#define O_RDONLY 00

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

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

