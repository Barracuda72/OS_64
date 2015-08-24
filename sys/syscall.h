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

// Объявления системных вызовов
void sys_exit(int status);
int sys_close(int file); 
int sys_execve(const char *name, char *const argv[], char *const envp[]);
int sys_fork(void);
int sys_fstat(int file, /*struct stat*/void *buf);
int sys_getpid(void);
int sys_isatty(int file);
int sys_kill(int pid, int sig);
int sys_link(const char *oldpath, const char *newpath);
int sys_lseek(int file, int off, int dir);
int sys_open(const char *name, int flags, int mode);
int sys_read(int file, char *ptr, int len);
int sys_sbrk(int incr);
int sys_stat(const char *file, /*struct stat*/void *buf);
int sys_times(/*struct tms*/void *buf);
int sys_unlink(const char *name);
int sys_wait(int *status);
int sys_write(int file, const char *ptr, int len);

#endif // __SYSCALL_H__
