/*
 * Простейший интерфейс для mutex'ов
 * mutex_init
 * mutex_lock
 * mutex_unlock
 */

.text
.code64

.globl mutex_init
// void mutex_init(void *mtx);
mutex_init:
movb $1, (%rdi)
ret

.globl mutex_lock
// void mutex_lock(void *mtx);
mutex_lock:
push %ax
lock:
xor %al, %al
xchg %al, (%rdi)
and %al, %al
jz lock
pop %ax
ret

.globl mutex_unlock
// void mutex_unlock(void *mtx);
mutex_unlock:
movb $1, (%rdi)
ret

