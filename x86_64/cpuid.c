#include <cpuid.h>
#include <stdint.h>

/*
 * Функции получения инфо о процессоре
 */

static int tmp[5];

/*
 * Возвращает строку-название производителя
 */
char *CPU_get_vendor_string(void)
{
  tmp[0] = 0;
  tmp[5] = 0;

  asm("cpuid":
    "=a"(tmp[0]),
    "=b"(tmp[1]),
    "=c"(tmp[2]),
    "=d"(tmp[3]):
    "a"(tmp[0])
  );
  // Поменяем местами 2 и 3 слова - они почему-то наоборот
  tmp[0] = tmp[3];
  tmp[3] = tmp[2];
  tmp[2] = tmp[0];

  return (char *)(&tmp[1]);
}

/*
 * Возвращает основную информацию
 */
uint32_t CPU_get_info(void)
{
  int data[5];
  data[0] = 1;
  asm("cpuid":
    "=a"(data[0]),
    "=b"(data[1]),
    "=c"(data[2]),
    "=d"(data[3]):
    "a"(data[0])
  );
  return data[0];
}

/*
 * Возвращает информацию о расширениях
 */
uint32_t CPU_get_xinfo(void)
{
  int data[5];
  data[0] = 1;
  asm("cpuid":
    "=a"(data[0]),
    "=b"(data[1]),
    "=c"(data[2]),
    "=d"(data[3]):
    "a"(data[0])
  );
  return data[3];
}

