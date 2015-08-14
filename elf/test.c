#include <unistd.h>

char hello[] = "Hello, world!\n";

int main(int argc, char *argv[])
{
  write(1, hello, sizeof(hello));
  return 0;
}
