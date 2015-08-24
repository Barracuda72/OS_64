/*
 * tls.h
 *
 * Структура Thread-Local Storage
 */

#ifndef __TLS_H__
#define __TLS_H__

typedef struct _thread_ls
{
  int errno_tls;
} thread_ls;

// Линейный адрес, по которому будет лежать TLS в пространстве пользователя
#define TLS_ADDR 0x00000000FFFFF000

#endif // __TLS_H__
