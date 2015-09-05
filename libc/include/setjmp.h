/*
 * setjmp.h
 *
 * Часть стандарта C2011
 *
 * Нелокальная передача управления
 *
 */

#ifndef _SETJMP_H
#define _SETJMP_H 1

/*
 * Структура для сохранения контекста
 *
 * Сохраняются 8 64-битных регистров
 */

struct _jmp_buf
{
  char data[8*8];
};

/*
 * Тип структуры согласно стандарту
 */
typedef struct _jmp_buf jmp_buf[1];

/*
 * Требуемый по стандарту макрос
 */
#define setjmp(x) _setjmp(x)

/*
 * Функции
 */

/*
 * Сохраняет контекст вызывающей процедуры для последующего использования
 * процедурой longjmp
 */
int _setjmp(jmp_buf env);

/*
 * Восстанавливает окружение, сохраненное предыдущим вызовом setjmp
 */
_Noreturn void longjmp(jmp_buf env, int val);

#endif // _SETJMP_H
