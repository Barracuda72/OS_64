#ifndef __SYSCALL_H__
#define __SYSCALL_H__

enum
{
  _N_testcall =  0,
  _N_exit     =  1,
  _N_close    =  2,
  _N_execve   =  3,
  _N_fork     =  4,
  _N_fstat    =  5,
  _N_getpid   =  6,
  _N_isatty   =  7,
  _N_kill     =  8,
  _N_link     =  9,
  _N_lseek    = 10,
  _N_open     = 11,
  _N_read     = 12,
  _N_sbrk     = 13,
  _N_stat     = 14,
  _N_times    = 15,
  _N_unlink   = 16,
  _N_wait     = 17,
  _N_write    = 18,
};

// Обработчик прерывания 0x80
void syscall_handler(void);

#endif // __SYSCALL_H__
