//#include <unistd.h>
#include "../kernel/debug.h"
#include "../libc/os64sys.h"

char hello[] = "Hello, world!\n";

int main(int argc, char *argv[])
{
  BREAK();
  int pid = getpid();
  kprintf("My PID is %d!\n", pid);
  // write(1, hello, sizeof(hello));
  return 0;
}

