/*
 * tls.c
 *
 * Ядерный интерфейс Thread-Local Storage
 */

#include <tls.h>
#include <task.h>

#include <errno.h>

int *__errno_location(void)
{
  return (int *)&(curr_cpu_task->tls->errno_tls);
}

