#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

void spinlock_init(void *mtx);

void spinlock_acquire(void *mtx);

void spinlock_release(void *mtx);

#endif // __SPINLOCK_H__
