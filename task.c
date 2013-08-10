#include <task.h>
#include <gdt.h>
#include <klibc.h>
#include <mem.h>
#include <multiboot.h>

#include <debug.h>

/*
 * Инициализируем многозадачность (громко сказано - просто 
 * устанавливаем стек для прерываний)
 */

TSS64 IntrTss;

unsigned long intr_s[1024];
unsigned short s = 0;  // Селектор задачи ядра
volatile task *curr = 0; // Текущая выполняемая задача
unsigned long next_pid = 1;

void tss_init()
{
  IntrTss.rsp0 = (unsigned long)&intr_s[1022];
  IntrTss.rsp1 = (unsigned long)&intr_s[1022];
  IntrTss.rsp2 = (unsigned long)&intr_s[1022];
  
  IntrTss.ist1 = (unsigned long)&intr_s[1022];
  IntrTss.ist2 = (unsigned long)&intr_s[1022];
  IntrTss.ist3 = (unsigned long)&intr_s[1022];
  IntrTss.ist4 = (unsigned long)&intr_s[1022];
  IntrTss.ist5 = (unsigned long)&intr_s[1022];
  IntrTss.ist6 = (unsigned long)&intr_s[1022];
  IntrTss.ist7 = (unsigned long)&intr_s[1022];
  
  s = GDT_smartaput(&IntrTss, sizeof(TSS64), SEG_PRESENT | SEG_TSS64);
  s = CALC_SELECTOR(s, SEG_GDT | SEG_RPL0);
  
  //BREAK();
  asm("ltr s");
}

void task_init()
{
  curr = kmalloc(sizeof(task));
  curr->pid = next_pid++;
  curr->rsp = 0;
  //curr->rbp = 0;
  curr->rflags = 0;
  curr->rip = 0;
  curr->cr3 = 0;
  curr->next = curr; // Закольцовываем
}

/*
 * Создает новый стек и переключается на него
 */
unsigned long change_stack()
{
  extern unsigned long stack;
  unsigned long stack_old = &stack;
  unsigned long stack_end = (unsigned long) stack_old + STACK_SIZE;
  unsigned long *new_stack = kmalloc(STACK_SIZE);
  memcpy(new_stack, stack_old, STACK_SIZE);
  unsigned long offset = (unsigned long)new_stack - (unsigned long)stack_old;
  unsigned long i;
  for (i = 0; i < STACK_SIZE; i++)
  {
    if ((new_stack[i] >= stack_end) && 
        (new_stack[i] <= (unsigned long)stack_old))
    {
      new_stack[i] += offset;
    }
  } 

  // Перемещаем стек
  /*asm volatile("add %0, %%rsp\n \
                add %0, %%rbp\n" :: "r" (offset)); */
  //BREAK();
  return offset;
}

/*
 * Своровано из учебника James'M
 * Позже перепишу более красиво
 */
unsigned long read_rip_old();
asm("\n \
read_rip_old: \n \
  pop %rax \n \
  jmp *%rax \n \
");

/*
 * Функция подготавливает стек для задачи таким образом,
 * чтобы все выглядело, будто задача была прервана.
 * Ну и за одним возвращает адрес возврата.
 */
unsigned long read_rip(unsigned long cr3, unsigned long *rsp_off);
asm("\n \
read_rip: \n \
  movq %rsp, %rax \n \
  addq (%rsi), %rax \n \
  subq $0x38, %rax # 0x40 займут регистры, минус адрес возврата \n \
  movq $0,   0x38(%rax) # RAX дочерней задачи \n \
  movq %rbx, 0x30(%rax) \n \
  movq %rcx, 0x28(%rax) \n \
  movq %rdx, 0x20(%rax) \n \
  movq %rsi, 0x18(%rax) \n \
  push %rbp \n \
  addq (%rsi), %rbp \n \
  movq %rbp, 0x10(%rax) \n \
  pop  %rbp \n \
  movq %rdi, 0x08(%rax) \n \
  movq %rdi, 0x00(%rax) # CR3 дочерней задачи \n \
  \n \
  addq $0x40, %rax \n \
  movq %rax, (%rsi) # Адрес стека дочерней задачи \n \
  movq (%rsp), %rax # Будущий адрес возврата из прерывания \n \
  ret \n \
");

/*
asm(" \n \
.globl task_fork \n \
task_fork: \n \
  push %rdi \n \
  push %rbx \n \
  cli \n \
  mov $0x30, %edi \n \
  call kmalloc \n \
  mov $next_pid, %rdi \n \
  mov (%rdi), %rbx \n \
  inc %rbx \n \
  mov %rbx, (%rdi) \n \
  mov %rax, %rdi \n \
  mov %rbx, (%rdi) \n \
  push %rdi \n \
  call change_stack \n \
  pop %rdi \n \
  mov %rax, %rbx \n \
  add %rsp, %rbx \n \
  mov %rbx, 0x10(%rdi) \n \
  add %rbp, %rax \n \
  mov %rax, 0x18(%rdi) \n \
  push %rdi \n \
  call copy_pages \n \
  pop %rdi \n \
  mov %rax, 0x20(%rdi) \n \
  mov $curr, %rax \n \
  mov (%rax), %rax \n \
  mov 0x28(%rax), %rbx \n \
  mov %rbx, 0x28(%rdi) \n \
  mov %rdi, 0x28(%rax) \n \
  xchg %bx, %bx \n \
  sti \n \
  hlt \n \
  mov $0, %rax \n \
  mov $curr, %rbx \n \
  cmp %rdi, (%rbx) \n \
  je rt         \n \
  mov (%rdi), %rax \n \
rt: \n \
  pop %rbx \n \
  pop %rdi \n \
  ret \n \
");
*/
int task_fork()
{
  unsigned long rip, off, cr3, rflags;
  intr_disable();
  
  off = change_stack();
  cr3 = copy_pages();
  asm ("pushfq \n \
        pop %0":"=r"(rflags));
  rip = read_rip(cr3, &off);
  if (rip == 0)
    return 0; // Дочерняя задача

  // Родительская задача - готовим все
  task *new = kmalloc(sizeof(task));
  new->pid = next_pid++;
  new->rsp = off;
  new->rip = rip;  
  new->rflags = rflags|(1<<9); // Разрешаем дочерней задаче прерывания
  new->cr3 = cr3; // Вроде уже и не нужно

  new->next = curr->next;
  curr->next = new;
  intr_enable();
  return new->pid;
}

int __task_fork()
{
  unsigned long rsp, rbp, rip, cr3, off;
  intr_disable();
  task *new = kmalloc(sizeof(task));
  new->pid = next_pid++;
  asm volatile("mov %%rsp, %0":"=r"(rsp)); 
  asm volatile("mov %%rbp, %0":"=r"(rbp));
  off = change_stack();
  new->rsp = rsp+off;
  //new->rbp = rbp+off;
  new->rip = 0;
  new->cr3 = copy_pages();
  // Добавляем задачу в очередь выполнения
  new->next = curr->next; 
  curr->next = new;


  // Включаем прерывания и ждем переключения задачи
  //intr_enable();
  asm volatile(" \n \
                sti \n \
                hlt");

  if (curr->pid == new->pid)
  {
    // Мы в дочерней задаче
    return 0;
  } else {
    return new->pid;
  }
}

/*
 * Мне лень разбираться, почему куча мусора task_switch,
 * написанная "по мотивам" JamesM kernel tutorials не пашет.
 * Я проще напишу на ассемблере.
 */
/*
asm("\n \
.globl task_switch \n \
task_switch: \n \
  xchg %bx, %bx \n \
  mov $curr, %rbx \n \
  mov (%rbx), %rbx \n \
  and %rbx, %rbx \n \
  jz ts_end \n \
  \n \
  mov $0x53, %edi \n \
  call ktty_putc \n \
  \n \
  call read_rip \n \
  mov $0xDEADC0DEDEADBEEF, %rcx \n \
  cmp %rcx, %rax \n \
  je ts_end \n \
  \n \
  mov %rax, 0x8(%rbx) \n \
  mov %rsp, 0x10(%rbx) \n \
  mov %rbp, 0x18(%rbx) \n \
  mov %cr3, %rcx \n \
  mov %rcx, 0x20(%rbx) \n \
  \n \
  cli \n \
  mov 0x28(%rbx), %rbx \n \
  \n \
  mov $curr, %rcx \n \
  mov %rbx, (%rcx) \n \
  \n \
  mov %rax, %rcx \n \
  mov 0x10(%rbx), %rsp \n \
  mov 0x18(%rbx), %rbp \n \
  mov 0x20(%rbx), %rax \n \
  mov %rax, %cr3 \n \
  mov $0xDEADC0DEDEADBEEF, %rax \n \
  sti \n \
  jmp *%rcx \n \
  \n \
ts_end:     \n \
  ret        \n \
");
*/

unsigned long task_switch(task_regs *r)
{
  // В r - стек прерывания
  if (curr != 0)
  {
    curr->rip = r->rip;
    curr->rflags = r->rflags;
    curr->rsp = r->rsp;

    curr = curr->next;

    r->rip = curr->rip;
    r->rflags = curr->rflags;
    r->rsp = curr->rsp;
  }
  return r->rsp;
}

/*
 * А вот и куча мусора
 */
void __task_switch()
{
  if (curr == 0)
    return;
  ktty_putc('S');

  unsigned long rsp, rbp, rip, cr3;
  asm ("mov %%rsp, %0":"=r"(rsp));
  asm ("mov %%rbp, %0":"=r"(rbp));
  asm ("mov %%cr3, %0":"=r"(cr3));

  //rip = read_rip();
  // Если мы только что переключили задачу
  if (rip == 0xDEADC0DEDEADBEEF)
    return;

  // Иначе - начнем переключение
  curr->rip = rip;
  curr->rsp = rsp;
  //curr->rbp = rbp;
  curr->cr3 = cr3;

  curr = curr->next;

  rsp = curr->rsp;
  //rbp = curr->rbp;
  cr3 = curr->cr3&0x000FFFFFFFFFF000; // Hack?
  
  asm volatile("         \
    cli;                 \
    xchg %%bx, %%bx; \
    mov %0, %%rcx;       \
    mov %1, %%rsp;       \
    mov %2, %%rbp;       \
    mov %3, %%cr3;       \
    mov $0xDEADC0DEDEADBEEF, %%rax; \
    sti;                 \
    jmp *%%rcx           "
  : : "r"(rip), "r"(rsp), "r"(rbp), "r"(cr3));
}

void get_c()
{
  printf("Curr: %l\n", curr);
}
