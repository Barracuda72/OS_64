/*
 * strxfrm.c
 *
 * Часть стандарта C2011
 *
 * Функция преобразует строку s2 в s1 так, чтобы результат применения 
 * strcmp на двух модифицированных строках совпадал с результатом 
 * применения strcoll на немодифицированных
 *
 */

#include <stdint.h>
#include <string.h>

// TODO: поскольку поддержки локалей нет, strcmp и strcoll - одна и та 
// же функция. Соответственно, никаких преобразований не требуется и
// данная функция представляет собой просто вариант strncpy
size_t strxfrm(char * restrict s1, const char * restrict s2, size_t n)
{
  uint64_t i,l;
  
  l = strlen(s2);
  
  for(i = 0; (i < n) && (s2[i] != 0); i++)
    s1[i] = s2[i];
    
  for(; i < n; i++)
    s1[i] = 0;
    
  return l;
}
