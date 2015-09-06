/*
 * signal.h
 *
 * Часть стандарта C2011
 *
 * Работа с сигналами
 *
 */

#ifndef _SIGNAL_H
#define _SIGNAL_H 1

#ifndef __IGNORE_UNIMPLEMENTED
#error Not implemented
#endif

// НЕ РЕАЛИЗОВАНО

// TODO: ЗАМЕНИТЬ!
typedef int sig_atomic_t;

/*
 * Значения, допустимые для второго параметра функции signal
 */

#define SIG_DFL 1
#define SIG_ERR 2
#define SIG_IGN 3

/*
 * Номера сигналов
 */
#define SIGABRT 1
#define SIGFPE  2
#define SIGILL  3
#define SIGINT  4
#define SIGSEGV 5
#define SIGTERM 6

/*
 * Функции
 */

/*
 * Устанавливает обработчик соответствующего сигнала
 */
void (*signal(int sig, void (*func)(int)))(int);

/*
 * Отправляет сигнал самому себе
 */
int raise(int sig);

#endif // _SIGNAL_H
