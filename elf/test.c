#include <unistd.h>
#include "../kernel/debug.h"
#include "../libc/os64sys.h"

char hello[] = "Hello, world!\n";

int main(int argc, char *argv[])
{
  BREAK();
#ifdef __HOSTED__
  write(1, hello, sizeof(hello));
#else
  syscall_test_out(hello, sizeof(hello));
#endif
  return 0;
}

