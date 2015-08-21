/*
 * linsys.c
 *
 * Обертка системных вызовов Linux 
 * для hosted-режима
 *
 */
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
*/

#include <stdint.h>

//#include <fcntl.h>

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

void *sbrk(int len)
{
  ssize_t newaddr = 0;
  asm volatile("\
    mov $45, %%rax\n\
    xor %%rbx, %%rbx\n\
    int $0x80\n\
    push %%rax\n\
    mov %1, %%ebx\n\
    add %%rax, %%rbx\n\
    mov $45, %%rax\n\
    int $0x80\n\
    pop %%rax\n\
    mov %%rax, %0\n\
    ":"=r"(newaddr)
     :"r"(len)
     :"rax","rbx"
  );

  return newaddr;
}

