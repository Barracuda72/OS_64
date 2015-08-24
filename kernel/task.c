#include <task.h>
#include <gdt.h>
#include <kprintf.h>
#include <mem.h>
#include <multiboot.h>
#include <stdint.h>
#include <intr.h>
#include <smp.h>
#include <apic.h>

#include <debug.h>

/*
 * Инициализируем многозадачность (громко сказано - просто 
 * устанавливаем стек для прерываний)
 */

TSS64 IntrTss[MAX_CPU_NR];

// Стек для прерываний
uint64_t intr_s[MAX_CPU_NR][1024];
// Стек для страничной ошибки
uint64_t fault_s[MAX_CPU_NR][1024];
// Стек для системных вызовов
uint64_t call_s[MAX_CPU_NR][1024];
volatile task *curr[MAX_CPU_NR]; // Текущая выполняемая задача
uint64_t next_pid = 1;

void tss_init_cpu(uint8_t id)
{
  uint16_t s = 0;  // Селектор задачи ядра
  IntrTss[id].rsp0 = (uint64_t)&intr_s[id][1022];
  IntrTss[id].rsp1 = (uint64_t)&intr_s[id][1022];
  IntrTss[id].rsp2 = (uint64_t)&intr_s[id][1022];
  
  IntrTss[id].ist1 = (uint64_t)&intr_s[id][1022];
  IntrTss[id].ist2 = (uint64_t)&fault_s[id][1022];
  IntrTss[id].ist3 = (uint64_t)&call_s[id][1022];
  IntrTss[id].ist4 = (uint64_t)&intr_s[id][1022];
  IntrTss[id].ist5 = (uint64_t)&intr_s[id][1022];
  IntrTss[id].ist6 = (uint64_t)&intr_s[id][1022];
  IntrTss[id].ist7 = (uint64_t)&intr_s[id][1022];
  
  s = GDT_smartaput(&IntrTss[id], sizeof(TSS64), SEG_PRESENT | SEG_TSS64 | SEG_DPL3);
  s = CALC_SELECTOR(s, SEG_GDT | SEG_RPL3);
  
  curr[id] = 0;

  //BREAK();
  asm("ltr %0"::"m"(s));
}

void tss_init()
{
  tss_init_cpu(0);
}

void task_init()
{
  intr_disable();
  curr[0] = kmalloc(sizeof(task));
  curr[0]->pid = next_pid++;
  zeromem(&(curr[0]->r), sizeof(all_regs));
  curr[0]->next = curr[0]; // Закольцовываем
  curr[0]->state = TASK_RUNNING;
  alloc_pages_user(TLS_ADDR, sizeof(thread_ls));
  curr[0]->tls = TLS_ADDR;
  intr_enable();
}

void task_init_cpu(uint8_t id)
{
  intr_disable();
  curr[id] = kmalloc(sizeof(task));
  curr[id]->pid = next_pid++;
  zeromem(&(curr[id]->r), sizeof(all_regs));
  curr[id]->state = TASK_RUNNING;
  alloc_pages_user(TLS_ADDR, sizeof(thread_ls));
  curr[id]->tls = TLS_ADDR;
  task *tmp = curr[0]->next;
  curr[0]->next = curr[id]; // Закольцовываем
  curr[id]->next = tmp;
  intr_enable();
}

/*
 * Создает новый стек 
 */
uint64_t copy_stack()
{
  // HACK: это не то, что нужно!
  // Исправить срочно!
  //extern uint64_t stack;
  extern uint64_t kernel_stack_new;
  uint64_t stack_old = kernel_stack_new; //&stack;
  uint64_t stack_end = (uint64_t) stack_old + STACK_SIZE;
  uint64_t *new_stack = kmalloc(STACK_SIZE);
  memcpy(new_stack, stack_old, STACK_SIZE);
  uint64_t offset = (uint64_t)new_stack - (uint64_t)stack_old;
  uint64_t i;
  for (i = 0; i < STACK_SIZE; i++)
  {
    if ((new_stack[i] <= stack_end) && 
        (new_stack[i] >= (uint64_t)stack_old))
    {
      new_stack[i] += offset;
    }
  } 

  return offset;
}

uint64_t change_stack()
{
  uint64_t offset = copy_stack();
  // Перемещаем стек
  asm volatile("add %0, %%rsp\n \
                add %0, %%rbp\n" :: "r" (offset));

  return offset;
}

/*
 * Функция подготавливает стек для задачи таким образом,
 * чтобы все выглядело, будто задача была прервана.
 * Ну и за одним возвращает адрес возврата.
 */
uint64_t read_rip(uint64_t cr3, uint64_t rsp_off, all_regs *r);
asm("\n \
read_rip_old: \n \
  movq %rsp, %rax \n \
  addq (%rsi), %rax \n \
  subq $0x48, %rax # 0x50 займут регистры, минус адрес возврата \n \
  movq %r8,  0x48(%rax) \n \
  movq %r9,  0x40(%rax) \n \
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
  addq $0x50, %rax \n \
  movq %rax, (%rsi) # Адрес стека дочерней задачи \n \
  movq (%rsp), %rax # Будущий адрес возврата из прерывания \n \
  ret \n \
");

asm("\n \
read_rip: \n \
  mov %rsp, %rax \n \
  add %rsi, %rax # %rax = стек дочерней задачи\n \
  xchg %rbp, %rsi # Подменим и скорректируем rbp \n \
  add %rsi, %rbp \n \
  add $8, %rax # Выкинем адрес возврата \n \
  xchg %rdx, %rsp \n \
  \n \
  pushq $0x10 # SS \n \
  push %rax # RSP \n \
  pushfq \n \
  pop %rax \n \
  or $0x200, %rax # Разрешим дочерней задаче прерывания\n \
  push %rax # RFLAGS \n \
  pushq $0x08 # CS \n \
  mov (%rdx), %rax \n \
  push %rax # RIP \n \
  \n \
  push %rdi # CR3 дочерней задачи \n \
  mov $0, %rax # RAX дочерней задачи \n \
  call save_regs \n \
  xchg %rdx, %rsp \n \
  xchg %rbp, %rsi \n \
  mov (%rsp), %rax \n \
  ret \n \
");

int task_fork()
{
  uint64_t rip, off, cr3;
  intr_disable();
  
  off = copy_stack();
  cr3 = copy_pages();

  // Родительская задача - готовим все
  task *new = kmalloc(sizeof(task));
  new->pid = next_pid++;
  //BREAK();
  rip = read_rip(cr3, off, (((uint64_t)&(new->r)) + sizeof(all_regs)));
  if (rip == 0)
    return 0; // Дочерняя задача

  new->next = curr[0]->next;
  curr[0]->next = new;
  //BREAK();
  intr_enable();
  return new->pid;
}

uint64_t task_switch(all_regs *r)
{
  uint8_t id = apic_get_id();

  if (curr[id] != 0)
  {
    // Если задача запускается, для нее подготовлены регистры
    if (curr[id]->state == TASK_STARTING)
    {
      curr[id]->state = TASK_RUNNING;
      curr[id]->r.reg1 = r->reg1;
    } else {
      memcpy(&(curr[id]->r), r, sizeof(all_regs));
      curr[id]->state = TASK_ACTIVE;
      do
      {
        curr[id] = curr[id]->next;
      } while(curr[id]->state != TASK_ACTIVE);
    }
    // Стек, с которого будем восстанавливать регистры
    return &(curr[id]->r);
  }
  return r;
}
/*
void get_c()
{
  kprintf("Curr: %l\n", curr);
}
*/
