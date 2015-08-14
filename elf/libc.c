#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <fcntl.h>

int memcmp(const void *s1, const void *s2, size_t n)
{
  size_t i;
  const unsigned char *a1 = (const unsigned char *)s1;
  const unsigned char *a2 = (const unsigned char *)s2;

  for (i = 0; i < n; i++)
    if (a1[i]-a2[i])
      return a1[i] - a2[i];

  return 0;
}

void *memcpy(void *dest, const void *src, size_t n)
{
  char *d = (char *)dest;
  char *s = (char *)src;
  int i;

  for (i = 0; i < n; i++)
    d[i] = s[i];

  return dest;
}

void *memset(void *s, int c, size_t n)
{
  char *d = (char *)s;

  for (; n > 0; n--)
    d[n-1] = c;

  return s;
}

size_t strlen(const char *s)
{
  size_t i = 0;
  while(s[i]) 
    i++;

  return i;
}

ssize_t write(int fd, const void *buf, size_t count)
{
  ssize_t nfd = fd;
  ssize_t written = 0;

  asm volatile("\
    mov $4, %%rax\n\
    mov %1, %%rbx\n\
    mov %2, %%rcx\n\
    mov %3, %%rdx\n\
    int $0x80\n\
    mov %%rax, %0\n\
    ":"=r"(written)
     :"r"(nfd),
      "r"(buf),
      "r"(count)
     :"rax","rbx","rcx","rdx"
    );

    return written;
}

ssize_t read(int fd, void *buf, size_t count)
{
  ssize_t nfd = fd;
  ssize_t readed = 0;

  asm volatile("\
    mov $3, %%rax\n\
    mov %1, %%rbx\n\
    mov %2, %%rcx\n\
    mov %3, %%rdx\n\
    int $0x80\n\
    mov %%rax, %0\n\
    ":"=r"(readed)
     :"r"(nfd),
      "r"(buf),
      "r"(count)
     :"rax","rbx","rcx","rdx"
    );

    return readed;
}

int open(const char *pathname, int flags)
{
  size_t fd;
  size_t nflags = flags;
  asm volatile("\
    mov $5, %%rax\n\
    mov %1, %%rbx\n\
    mov %2, %%rcx\n\
    int $0x80\n\
    mov %%rax, %0\n\
    ":"=r"(fd)
     :"r"(pathname),
      "r"(nflags)
     :"rax","rbx","rcx"
  );
  return fd;
}

int close(int fd)
{
  size_t nfd = fd;
  size_t ret;
  asm volatile("\
    mov $6, %%rax\n\
    mov %1, %%rbx\n\
    int $0x80\n\
    mov %%rax, %0\n\
    ":"=r"(ret)
     :"r"(nfd)
     :"rax","rbx"
  );
  return ret;
}

off_t lseek(int fd, off_t offset, int whence)
{
  size_t nfd = fd;
  size_t nwhence = whence;
  off_t pos;

  asm volatile("\
    mov $19, %%rax\n\
    mov %1, %%rbx\n\
    mov %2, %%rcx\n\
    mov %3, %%rdx\n\
    int $0x80\n\
    mov %%rax, %0\n\
    ":"=r"(pos)
     :"r"(nfd),"r"(offset),"r"(nwhence)
     :"rax","rbx","rcx","rdx"
  );

  return pos;
}

void exit(int code)
{
  size_t ncode = code;

  asm volatile("\
    mov $1, %%rax\n\
    mov %0, %%rbx\n\
    int $0x80\n\
    ":
     :"r"(ncode)
     :"rax","rbx"
   );

   for(;;);
}

void *mremap(void *old_address, size_t old_size,
                    size_t new_size, int flags, void *new_address)
{
  ssize_t nflags = flags;
  ssize_t newaddr = 0;

  asm volatile("\
    mov $163, %%rax\n\
    mov %1, %%rbx\n\
    mov %2, %%rcx\n\
    mov %3, %%rdx\n\
    mov %4, %%rsi\n\
    mov %5, %%rdi\n\
    int $0x80\n\
    mov %%rax, %0\n\
    ":"=r"(newaddr)
     :"r"(old_address),
      "r"(old_size),
      "r"(new_size),
      "r"(nflags),
      "r"(new_address)
     :"rax","rbx","rcx","rdx","rsi","rdi"
    );

    return newaddr;
}

#define MAVAIL __POOL_SIZE__

static char internal[MAVAIL+0x1000];
static const int mavail = MAVAIL;
static int mpos = 0;

void *malloc(size_t size)
{
  if (mpos >= mavail)
    return 0;

  long internal_addr = ((long)internal & (~0xFFF)) + 0x1000;

  size = (size&(~0xFFF)) + 0x1000;

  long ret = internal_addr + mpos;
  mpos += size;

  return ret;
}

int puts(const char *s)
{
  size_t len = strlen(s);
  char *nl = "\n";

  write(1, s, len);
  write(1, nl, 1);

  return 0;
}

