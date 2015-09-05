/*
 * inttypes.h
 *
 * Часть стандарта C2011
 *
 * Преобразования целочисленных типов
 *
 */

#ifndef _INTTYPES_H
#define _INTTYPES_H 1

#include <stdint.h>
#include <stddef.h> // Для wchar_t

/*
 * Макросы вывода для fprintf
 */

/*
 * Знаковые целые
 */
# define PRId8		"d"
# define PRId16		"d"
# define PRId32		"d"
# define PRId64		"ld"

# define PRIi8		"i"
# define PRIi16		"i"
# define PRIi32		"i"
# define PRIi64		"li"

# define PRIdLEAST8	"d"
# define PRIdLEAST16	"d"
# define PRIdLEAST32	"d"
# define PRIdLEAST64	"ld"

# define PRIiLEAST8	"i"
# define PRIiLEAST16	"i"
# define PRIiLEAST32	"i"
# define PRIiLEAST64	"li"

# define PRIdFAST8	"d"
# define PRIdFAST16	"ld"
# define PRIdFAST32	"ld"
# define PRIdFAST64	"ld"

# define PRIiFAST8	"i"
# define PRIiFAST16	"li"
# define PRIiFAST32	"li"
# define PRIiFAST64	"li"

# define PRIdMAX	"ld"

# define PRIiMAX	"li"

# define PRIdPTR	"ld"

# define PRIiPTR	"li"

/*
 * Беззнаковые целые
 */
# define PRIo8		"o"
# define PRIo16		"o"
# define PRIo32		"o"
# define PRIo64		"lo"

# define PRIu8		"u"
# define PRIu16		"u"
# define PRIu32		"u"
# define PRIu64		"lu"

# define PRIx8		"x"
# define PRIx16		"x"
# define PRIx32		"x"
# define PRIx64		"lx"

# define PRIX8		"X"
# define PRIX16		"X"
# define PRIX32		"X"
# define PRIX64		"lX"

# define PRIoLEAST8	"o"
# define PRIoLEAST16	"o"
# define PRIoLEAST32	"o"
# define PRIoLEAST64	"lo"

# define PRIuLEAST8	"u"
# define PRIuLEAST16	"u"
# define PRIuLEAST32	"u"
# define PRIuLEAST64	"lu"

# define PRIxLEAST8	"x"
# define PRIxLEAST16	"x"
# define PRIxLEAST32	"x"
# define PRIxLEAST64	"lx"

# define PRIXLEAST8	"X"
# define PRIXLEAST16	"X"
# define PRIXLEAST32	"X"
# define PRIXLEAST64	"lX"

# define PRIoFAST8	"o"
# define PRIoFAST16	"lo"
# define PRIoFAST32	"lo"
# define PRIoFAST64	"lo"

# define PRIuFAST8	"u"
# define PRIuFAST16	"lu"
# define PRIuFAST32	"lu"
# define PRIuFAST64	"lu"

# define PRIxFAST8	"x"
# define PRIxFAST16	"lx"
# define PRIxFAST32	"lx"
# define PRIxFAST64	"lx"

# define PRIXFAST8	"X"
# define PRIXFAST16	"lX"
# define PRIXFAST32	"lX"
# define PRIXFAST64	"lX"

# define PRIoMAX	"lo"
# define PRIuMAX	"lu"
# define PRIxMAX	"lx"
# define PRIXMAX	"lX"

# define PRIoPTR	"lo"
# define PRIuPTR	"lu"
# define PRIxPTR	"lx"
# define PRIXPTR	"lX"

/*
 * Макросы ввода для fscanf
 */

/*
 * Знаковые целые
 */
# define SCNd8		"hhd"
# define SCNd16		"hd"
# define SCNd32		"d"
# define SCNd64		"ld"

# define SCNi8		"hhi"
# define SCNi16		"hi"
# define SCNi32		"i"
# define SCNi64		"li"

# define SCNdLEAST8	"hhd"
# define SCNdLEAST16	"hd"
# define SCNdLEAST32	"d"
# define SCNdLEAST64	"ld"

# define SCNiLEAST8	"hhi"
# define SCNiLEAST16	"hi"
# define SCNiLEAST32	"i"
# define SCNiLEAST64	"li"

# define SCNdFAST8	"hhd"
# define SCNdFAST16	"ld"
# define SCNdFAST32	"ld"
# define SCNdFAST64	"ld"

# define SCNiFAST8	"hhi"
# define SCNiFAST16	"li"
# define SCNiFAST32	"li"
# define SCNiFAST64	"li"

# define SCNdMAX	"ld"

# define SCNiMAX	"li"

# define SCNdPTR	"ld"

# define SCNiPTR	"li"

/*
 * Беззнаковые целые
 */
# define SCNo8		"hho"
# define SCNo16		"ho"
# define SCNo32		"o"
# define SCNo64		"lo"

# define SCNu8		"hhu"
# define SCNu16		"hu"
# define SCNu32		"u"
# define SCNu64		"lu"

# define SCNx8		"hhx"
# define SCNx16		"hx"
# define SCNx32		"x"
# define SCNx64		"lx"

# define SCNoLEAST8	"hho"
# define SCNoLEAST16	"ho"
# define SCNoLEAST32	"o"
# define SCNoLEAST64	"lo"

# define SCNuLEAST8	"hhu"
# define SCNuLEAST16	"hu"
# define SCNuLEAST32	"u"
# define SCNuLEAST64	"lu"

# define SCNxLEAST8	"hhx"
# define SCNxLEAST16	"hx"
# define SCNxLEAST32	"x"
# define SCNxLEAST64	"lx"

# define SCNoFAST8	"hho"
# define SCNoFAST16	"lo"
# define SCNoFAST32	"lo"
# define SCNoFAST64	"lo"

# define SCNuFAST8	"hhu"
# define SCNuFAST16	"lu"
# define SCNuFAST32	"lu"
# define SCNuFAST64	"lu"

# define SCNxFAST8	"hhx"
# define SCNxFAST16	"lx"
# define SCNxFAST32	"lx"
# define SCNxFAST64	"lx"

# define SCNoMAX	"lo"
# define SCNuMAX	"lu"
# define SCNxMAX	"lx"

# define SCNoPTR	"lo"
# define SCNuPTR	"lu"
# define SCNxPTR	"lx"

typedef struct
{
  intmax_t quot;
  intmax_t rem;
} imaxdiv_t;

/*
 * Функции
 */

/*
 * Вычисляет абсолютное значение параметра
 */
intmax_t imaxabs(intmax_t j);

/*
 * Выполняет целочисленное деление
 */
imaxdiv_t imaxdiv(intmax_t numer, intmax_t denom);

/*
 * Преобразование строки в число со знаком
 */
intmax_t strtoimax(const char * restrict nptr,
                   char ** restrict endptr, int base);

/*
 * Преобразование строки в число без знака
 */
uintmax_t strtoumax(const char * restrict nptr,
                    char ** restrict endptr, int base);

/*
 * Преобразование "широкой" строки в число со знаком
 */
intmax_t wcstoimax(const wchar_t * restrict nptr,
                   wchar_t ** restrict endptr, int base);

/*
 * Преобразование "широкой" строки в число без знака
 */
uintmax_t wcstoumax(const wchar_t * restrict nptr,
                    wchar_t ** restrict endptr, int base);

#endif // _INTTYPES_H
