#ifndef __MUTEX_H__
#define __MUTEX_H__

void mutex_init(void *mtx);

void mutex_lock(void *mtx);

void mutex_unlock(void *mtx);

#endif // __MUTEX_H__
