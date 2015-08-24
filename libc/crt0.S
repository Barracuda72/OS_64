.section .text

.global _start
_start:
  # Установим конец связного списка кадров стека
  movq $0, %rbp
  pushq %rbp # rip=0
  pushq %rbp # rbp=0
  movq %rsp, %rbp
 
  # Прибережем для main
  pushq %rsi
  pushq %rdi
 
  # Иницализируем сигналы, менеджер памяти, стандартный ввод/вывод и прочее
  #call initialize_standard_library
 
  # Выполним глобальные конструкторы
  #call _init
 
  # Восстановим argc и argv.
  popq %rdi
  popq %rsi
 
  # Вызовем main
  call main
 
  # Завершим процесс
  mov %rax, %rdi
  # call exit
loop:
  jmp loop
