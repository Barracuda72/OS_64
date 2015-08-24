/*
 * tls.c
 *
 * Интерфейс пользователя к Thread-Local Storage
 */

#include <errno.h>
#include <tls.h>

int *__errno_location(void)
{
  return (int *)&(((thread_ls *)TLS_ADDR)->errno_tls);
}

