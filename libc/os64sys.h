/*
 * os64sys.h
 *
 * Системные вызовы, интерфейс уровня пользователя
 */

#ifndef __OS64SYS_H__
#define __OS64SYS_H__

// Украдено у JamesM и переработано

#define DECL_SYSCALL0(fn) int syscall_##fn();
#define DECL_SYSCALL1(fn,p1) int syscall_##fn(p1);
#define DECL_SYSCALL2(fn,p1,p2) int syscall_##fn(p1,p2);
#define DECL_SYSCALL3(fn,p1,p2,p3) int syscall_##fn(p1,p2,p3);
#define DECL_SYSCALL4(fn,p1,p2,p3,p4) int syscall_##fn(p1,p2,p3,p4);

#define DEFN_SYSCALL0(fn, num) \
int syscall_##fn() \
{ \
 int a; \
 asm volatile("int $0x80" : "=a" (a) : "0" (num)); \
 return a; \
}

#define DEFN_SYSCALL1(fn, num, P1) \
int syscall_##fn(P1 p1) \
{ \
 int a; \
 asm volatile("int $0x80" : "=a" (a) : "0" (num), "D" ((long)p1)); \
 return a; \
}

#define DEFN_SYSCALL2(fn, num, P1, P2) \
int syscall_##fn(P1 p1, P2 p2) \
{ \
 int a; \
 asm volatile("int $0x80" : "=a" (a) : "0" (num), "D" ((long)p1), "S" ((long)p2)); \
 return a; \
}

#define DEFN_SYSCALL3(fn, num, P1, P2, P3) \
int syscall_##fn(P1 p1, P2 p2, P3 p3) \
{ \
 int a; \
 asm volatile("int $0x80" : "=a" (a) : "0" (num), "D" ((long)p1), "S" ((long)p2), "d" ((long)p3)); \
 return a; \
}

#define DEFN_SYSCALL4(fn, num, P1, P2, P3, P4) \
int syscall_##fn(P1 p1, P2 p2, P3 p3, P4 p4) \
{ \
 int a; \
 asm volatile("int $0x80" : "=a" (a) : "0" (num), "D" ((long)p1), "S" ((long)p2), "d"((long)p3), "c"((long)p4)); \
 return a; \
}


// Тестовый системый вызов
DECL_SYSCALL2(test_out, const char *, unsigned long)

#endif // __OS64SYS_H__
