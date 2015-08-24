/*
 * tls.c
 *
 * Ядерный интерфейс Thread-Local Storage
 */

#include <tls.h>
#include <task.h>
#include <apic.h>

#include <errno.h>

int *__errno_location(void)
{
  return (int *)&(curr[apic_get_id()]->tls->errno_tls);
}

