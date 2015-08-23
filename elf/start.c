#include <gdt.h>
#include <elf.h>
#include <task.h>
#include <smp.h>
#include <apic.h>

#include <debug.h>

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
    # TODO: \n \
    # Стек еще выделить нужно! \n \
    # call memset  # Очистим стек\n\
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
    # jmp *%%r9\n\
    mov $0x23, %%ax\n\
    mov %%ax, %%ds\n\
    mov %%ax, %%es\n\
    mov %%ax, %%fs\n\
    mov %%ax, %%gs\n\
    mov %%ax, %%ss\n\
    pushq $0x1B\n\
    push %0\n\
    lretq\n\
    hlt\n\
  ":
   :"r"(addr), "r"(fake_argc), "r"(fake_envp));
}

int start_process(void *buffer)
{
  // Точка входа
  int (*entry)() = NULL;

  // Проверим, сможет ли наш загрузчик обработать этот файл
  if (elf64_is_supported((Elf64_Ehdr *)buffer))
  {
    ktty_puts("Seems to be supported ELF file");

    // Файл поддерживается, обработаем
    entry = elf64_load_file((Elf64_Ehdr *)buffer);
  }

  if (entry != NULL)
  {
    uint8_t id = apic_get_id();
    intr_disable();
    curr[id]->state = TASK_STARTING;
    memset(&(curr[id]->r.c), 0, sizeof(comm_regs));
    curr[id]->r.i.rip = entry;
    curr[id]->r.i.cs = CALC_SELECTOR(3, SEG_GDT | SEG_RPL3);
    alloc_pages_user(0x100000000, 0x2000);
    curr[id]->r.i.rsp = 0x100001FF0;
    curr[id]->r.i.ss = CALC_SELECTOR(4, SEG_GDT | SEG_RPL3);
    intr_enable();
    // Не возвращаемся
    for(;;);
  }

  return 0;
}

