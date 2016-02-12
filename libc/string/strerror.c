/*
 * strerror.c
 *
 * Часть стандарта C2011
 *
 * Функция возвращает строку-описание ошибки по ее коду
 *
 */

#include <stdint.h>
#include <string.h>

#include <errno.h>

static const char *msg[] =
{
  [0]       = "Unknown error",
  [EDOM]    = "Math argument out of domain of func",
  [EILSEQ]  = "Illegal byte sequence", 
  [ERANGE]  = "Math result not representable",
  [EBADF]   = "Bad file number",
  [EINVAL]  = "Invalid argument",
  [ENFILE]  = "File table overflow",
  [ENOENT]  = "No such file or directory",
  [EBUSY]   = "Device or resource busy",
  [ENOTDIR] = "Not a directory",
  [EXDEV]   = "Cross-device link",
  [ENOMEM]  = "Out of memory",
  [EAGAIN]  = "Try again",
  [EMLINK]  = "Too many links",
  [ECHILD]  = "No child processes",
};

char *strerror(int errnum)
{
  if (errnum <= ELASTNUM)
    return (char *)msg[errnum];
  else
    return (char *)msg[0];
}
