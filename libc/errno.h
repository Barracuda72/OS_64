/*
 * errno.h
 *
 * Стандартный заголовочный файл
 */

#ifndef __ERRNO_H__
#define __ERRNO_H__

int *__errno_location(void);
#define errno (*__errno_location ())

/*
 * Коды ошибок
 */

#define EBADF   23
#define EINVAL  24
#define ENFILE  25
#define ENOENT  26
#define EBUSY   27
#define ENOTDIR 28
#define EXDEV   29
#define ENOMEM  30
#define EAGAIN  31
#define EMLINK  32
#define ECHILD  33

#endif // __ERRNO_H__
