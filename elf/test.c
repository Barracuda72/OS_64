#include <unistd.h>
#include "../kernel/debug.h"
#include "../kernel/syscall.h"

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

#ifndef __HOSTED__
DEFN_SYSCALL2(test_out, 0, const char *, unsigned long);
#endif
