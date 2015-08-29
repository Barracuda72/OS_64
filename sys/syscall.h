#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <syscall_nr.h>

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
