#include <stdio.h>
#include <stdlib.h>

//#define __USE_GNU
//#include <sys/mman.h>
#include <errno.h>

#include "elf.h"

/*
 * Гранулярность буфера
 */
#define BUF_GRAN 16384

#define O_RDONLY 00
//#define SEEK_SET 
//#define SEEK_END

static const char fake_argc[] = 
  "/var/tmp/fake";

static const char fake_envp[] =
  "dummy=1";

void call_entry (long addr)
{
  asm volatile("\
    mov %0, %%r9 # Сохраним адрес \n\
    xor %%rax, %%rax\n\
    push %%rax   # Конец переменных окружения\n\
    mov %2, %%rax\n\
    push %%rax   # Одна-единственная переменная окружения\n\
    xor %%rax, %%rax\n\
    push %%rax   # Конец argv\n\
    mov %1, %%rax\n\
    push %%rax   # Начало argv\n\
    mov $1, %%rax\n\
    push %%rax   # argc\n\
    mov %%rsp, %%rdi\n\
    xor %%rsi, %%rsi\n\
    mov $1024, %%rdx\n\
    call memset  # Очистим стек\n\
    xor %%rax, %%rax\n\
    xor %%rbx, %%rbx\n\
    xor %%rcx, %%rcx\n\
    xor %%rdx, %%rdx\n\
    xor %%rdi, %%rdi\n\
    xor %%rsi, %%rsi\n\
    xor %%rbp, %%rbp\n\
    xor %%r8,  %%r8\n\
    xor %%r10, %%r10\n\
    xor %%r11, %%r11\n\
    xor %%r12, %%r12\n\
    xor %%r13, %%r13\n\
    xor %%r14, %%r14\n\
    xor %%r15, %%r15\n\
    jmp *%%r9\n\
    hlt\n\
  ":
   :"r"(addr), "r"(fake_argc), "r"(fake_envp));
}

int main(int argc, char *argv[])
{
  // Буфер под файл
  char *buffer = NULL;
  // Точка входа
  int (*entry)() = NULL;

  // Считаем файл в память
  int f = open("test.elf", O_RDONLY);
  int len = lseek(f, 0, SEEK_END);
  lseek(f, 0, SEEK_SET);

  buffer = malloc(len);
  read(f, buffer, len);
  close(f);

  // Проверим, сможет ли наш загрузчик обработать этот файл
  if (elf64_is_supported((Elf64_Ehdr *)buffer))
  {
    puts("Seems to be supported ELF file");

    // Файл поддерживается, обработаем
    entry = elf64_load_file((Elf64_Ehdr *)buffer);

    if (entry != NULL)
      call_entry(entry);
      //entry();
  }

  //printf("main : %x\n", main);

  return 0;
}

