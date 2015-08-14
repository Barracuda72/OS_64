#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int fs_test_main(void *p, int len);

#define B_SIZE 0x1000*0x400

int main(int argc, char *argv[])
{
  FILE *f = fopen("initrd.img", "rb");
  int bsize = 0;
  int l;
  char *buf = NULL;

  do {
    buf = realloc(buf, bsize + B_SIZE);
    l = fread(buf+bsize, 1, B_SIZE, f);
    //printf("Readed %d bytes\n", l);
    bsize += l;
  } while (l == B_SIZE);

  //printf("Total %d bytes\n", bsize);

  fclose(f);

  fs_test_main(buf, bsize);

  free(buf);
  return 0;
}

