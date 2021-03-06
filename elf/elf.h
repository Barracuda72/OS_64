#ifndef __ELF_H__
#define __ELF_H__

#include <stdint.h>

/*
 * Типы, используемые в спецификации ELF
 */
// Беззнаковый адрес
typedef uint64_t Elf64_Addr;
// Беззнаковая половинка целого
typedef uint16_t Elf64_Half;
// Знаковая половинка целого
typedef int16_t  Elf64_SHalf;
// Беззнаковое смещение
typedef uint64_t Elf64_Off;
// Беззнаковое целое
typedef uint32_t Elf64_Word;
// Знаковое целое
typedef int32_t  Elf64_Sword;
// Беззнаковое целое двойной длины
typedef uint64_t Elf64_Xword;
// Знаковое целое двойной длины
typedef uint64_t Elf64_Sxword;

/*
 * Заголовок ELF-файла
 */
# define ELF_NIDENT 16  // Размер машиннонезависимой части

typedef struct {
  // Идентификатор и служебная машиннонезависимая информация
  uint8_t    e_ident[ELF_NIDENT];
  // Тип файла: исполняемый, перемещаемый, библиотека, etc
  Elf64_Half e_type;
  // Архитектура, для которой собран данный файл
  Elf64_Half e_machine;
  // Версия файла, по стандарту >=1, но обычно всегда 1
  Elf64_Word e_version;
  // Виртуальный адрес точки входа в секцию кода, или ноль в случае отсутствия
  // точки входа
  Elf64_Addr e_entry;
  // Смещение таблицы программных заголовков внутри файла, ноль в случае
  // отсутствия таблицы
  Elf64_Off  e_phoff;
  // Смещение таблицы заголовков секций внутри файла, ноль в случае отсутствия
  // таблицы
  Elf64_Off  e_shoff;
  // Разные процессорозависимые флаги
  Elf64_Word e_flags;
  // Размер этой структуры в байтах
  Elf64_Half e_ehsize;
  // Размер в байтах одной записи в таблице программных заголовков, все записи
  // имеют одинаковый размер
  Elf64_Half e_phentsize;
  // Количество программных заголовков
  Elf64_Half e_phnum;
  // Размер в байтах одной записи в таблице заголовков секций, все записи
  // имеют одинаковый размер
  Elf64_Half e_shentsize;
  // Количество заголовков секций
  Elf64_Half e_shnum;
  // Индекс заголовка секции таблицы строк
  Elf64_Half e_shstrndx;
} Elf64_Ehdr;

/*
 * Индексы элементов идентификационной информации
 */
enum Elf_Ident {
  EI_MAG0     = 0,   // 0x7F
  EI_MAG1     = 1,   // 'E'
  EI_MAG2     = 2,   // 'L'
  EI_MAG3     = 3,   // 'F'
  EI_CLASS    = 4,   // Разрядность (32/64 бита)
  EI_DATA     = 5,   // Порядок бит
  EI_VERSION  = 6,   // Версия стандарта ELF
  EI_OSABI    = 7,   // Специфичные для ОС флаги
  EI_PAD      = 8    // Выравнивание
};
 
# define ELFMAG0  0x7F       // e_ident[EI_MAG0]
# define ELFMAG1  'E'        // e_ident[EI_MAG1]
# define ELFMAG2  'L'        // e_ident[EI_MAG2]
# define ELFMAG3  'F'        // e_ident[EI_MAG3]
# define ELFMAG   "\177ELF"  // Строка целиком
# define SELFMAG  4          // Длина строки
 
# define ELFDATA2LSB  (1)  // Порядок бит Little Endian
# define ELFCLASS32   (1)  // Файл с 32-разрядным кодом
# define ELFCLASS64   (2)  // Файл с 64-разрядным кодом

# define EV_CURRENT   (1)  // Версия стандарта ELF

# define EM_386       (3)  // Устаревшая 32-битная архитектура Intel
# define EM_AMD64     (62) // Интересующая нас архитектура

/*
 * Тип файла
 */
enum Elf_Type {
  ET_NONE   = 0, // Неизвестный
  ET_REL    = 1, // Перемещаемый (объектный)
  ET_EXEC   = 2, // Исполняемый
  ET_DYN    = 3  // Динамическая библиотека
};

/*
 * Заголовок секции ELF-файла
 */
typedef struct {
  // Имя секции, индекс в таблице строк
  Elf64_Word   sh_name;
  // Тип секции
  Elf64_Word   sh_type;
  // Различные атрибуты
  Elf64_Xword  sh_flags;
  // Виртуальный адрес во время исполнения
  Elf64_Addr   sh_addr;
  // Смещение секции в файле
  Elf64_Off    sh_offset;
  // Размер секции в байтах
  Elf64_Xword  sh_size;
  // Индекс другой секции
  Elf64_Word   sh_link;
  // Дополнительная информация о секции
  Elf64_Word   sh_info;
  // Выравнивание секции
  Elf64_Xword  sh_addralign;
  // Размер элемента, если секция представляет собой таблицу
  Elf64_Xword  sh_entsize;
} Elf64_Shdr;

/*
 * Типы различных объектов в ELF-файле
 */
# define SHN_UNDEF  (0x00) // Неопределенный/несуществующий
 
/*
 * Типы секций в ELF-файле
 */
enum ShT_Types {
  SHT_NULL      = 0,   // Пустая секция
  SHT_PROGBITS  = 1,   // Информация, необходимая самой программе (загрузчику неинтересна)
  SHT_SYMTAB    = 2,   // Таблица символов
  SHT_STRTAB    = 3,   // Таблица строк
  SHT_RELA      = 4,   // Перемещаемые объекты с явными значениями корректировки
  SHT_NOBITS    = 8,   // Секция с данными программы, не присутствующая в файле
  SHT_REL       = 9,   // Перемещаемые объекты без явных значений корректировки
};

/*
 * Атрибуты секции
 */
enum ShT_Attributes {
  SHF_WRITE      = 0x01, // Записываемая
  SHF_ALLOC      = 0x02, // Должна быть загружена
  SHF_EXECINSTR  = 0x04  // Содержит исполняемый код
};

/*
 * Релокация с неявной корректировкой
 * Значение корректировки лежит по адресу r_offset
 */
typedef struct {
  // Адрес, по которому необходимо применить изменения
  Elf64_Addr  r_offset;
  // Индекс и тип релокации
  Elf64_Xword r_info;
} Elf64_Rel;

/*
 * Релокация с явной корректировкой
 * Значение корректировки указано в поле addend
 */
typedef struct {
  // Адрес, по которому необходимо применить изменения
  Elf64_Addr   r_offset;
  // Индекс и тип релокации
  Elf64_Xword  r_info;
  // Константа для вычисления
  Elf64_Sxword r_addend;
} Elf64_Rela;

/*
 * Макросы для извлечения информации из поля r_info вышележащих структур
 */
#define ELF64_R_SYM(i)      ((i) >> 32)
#define ELF64_R_TYPE(i)     ((i) & 0xffffffff)

/*
 * Типы релокаций для x86_64
 */
/*
 * Справка по обозначениям:
 *
 * A
 * Значение корректировки (addend) для вычисления релокации
 *
 * B
 * Базовый адрес, по которому разделяемый файл загружается в память на
 * выполнение. Как правило, разделяемый файл компонуется с базовым виртуальным
 * адресом, равным нулю. Однако, адрес исполнения - это другое.
 *
 * G
 * Смещение в GOT - глобальной таблице смещений, по которому во время
 * выполнения лежит символ, соответствующий данной релокации.
 *
 * GOT
 * Адрес глобальной таблицы смещений
 *
 * L
 * Смещение секции или адрес элемента PLT - таблицы связывания процедур для
 * символа
 *
 * P
 * Смещение секции или адрес объекта, вычисляется при помощи r_offset
 *
 * S
 * Значение символа, чей индекс лежит в релокации
 *
 * Z
 * Размер символа, чей индекс лежит в релокации
 */
enum RtT_Types {
  R_AMD64_NONE       = 0, // Без релокации
  R_AMD64_64         = 1, // Символ + смещение (S + A)
  R_AMD64_PC32       = 2, // Символ + смещение - смещение секции (S + A - P)
  // Еще много всяких
  R_AMD64_RELATIVE   = 8, // Базовый адрес + смещение (B + A)
  R_X86_64_IRELATIVE = 37 // Аналогично
};

/*
 * Макросы для упрощения обработки
 */
# define DO_AMD64_64(S, A)       ((S) + (A))
# define DO_AMD64_PC32(S, A, P)  (((S) + (A) - (P))&0xFFFFFFFF)
# define DO_AMD64_RELATIVE(B, A) ((B) + (A))

/*
 * Элемент таблицы символов
 */
typedef struct {
  // Имя символа, индекс в таблице строк
  Elf64_Word    st_name;
  // Тип и информация о связывании
  unsigned char st_info;
  // Зарезервировано
  unsigned char st_other;
  // Индекс соответствующей секции в таблице секций
  Elf64_Half    st_shndx;
  // Значение символа
  Elf64_Addr    st_value;
  // Размер соответствующего символа
  Elf64_Xword   st_size;
} Elf64_Sym;

/*
 * Макросы для извлечения информации из поля st_info структуры выше
 */
#define ELF_ST_BIND(x)    ((x) >> 4)
#define ELF_ST_TYPE(x)    (((unsigned int) x) & 0xf)
#define ELF64_ST_BIND(x)  ELF_ST_BIND(x)
#define ELF64_ST_TYPE(x)  ELF_ST_TYPE(x)

/*
 * Типы связывания символа
 */
enum StT_Binding {
  STB_LOCAL  = 0, // Локальное
  STB_GLOBAL = 1, // Глобальное
  STB_WEAK   = 2  // Слабое
};

/*
 * Тип самого символа
 */
enum StT_Type {
  STT_NOTYPE = 0, // Неизвестный
  STT_OBJECT = 1, // Переменная, массив, другие данные
  STT_FUNC   = 2  // Процедура, функция, другой код
};

/*
 * Программный заголовок
 */
typedef struct {
  // Тип заголовка
  Elf64_Word p_type;
  // Флаги доступа
  Elf64_Word p_flags;
  // Смещение в файле
  Elf64_Off  p_offset;
  // Виртуальный адрес
  Elf64_Addr p_vaddr;
  // Физический адрес
  Elf64_Addr p_paddr;
  // Размер в файле
  Elf64_Xword p_filesz;
  // Размер в памяти
  Elf64_Xword p_memsz;
  // Выравнивание, в памяти и в файле
  Elf64_Xword p_align;
} Elf64_Phdr;

/*
 * Тип программного заголовка
 */
enum PhT_Type {
  PT_NULL    = 0, // Пустой
  PT_LOAD    = 1, // Подлежит загрузке
  PT_DYNAMIC = 2, // Информация для динамической компоновки
  PT_INTERP  = 3, // Описание динамического загрузчика
  PT_NOTE    = 4  // Разнообразная информация, помещаемая компилятором
};

/*
 * Флаги доступа, поле p_flags программного заголовка
 */
enum PhT_Flags {
  PF_X = 0x1, // Исполняемый
  PF_W = 0x2, // Записываемый
  PF_R = 0x4  // Читаемый
};

/*
 * Проверяет, может ли загрузчик обработать данный файл
 */
int elf64_is_supported(Elf64_Ehdr *hdr);

/*
 * Загружает файл и возвращает точку входа
 */
void *elf64_load_file(Elf64_Ehdr *hdr);

/*
 * Ошибка при загрузке файла
 */
# define ELF_LOAD_ERR (-1)

/*
 * Ошибка при релокации
 */
# define ELF_REL_ERR  (-2)
#endif // __ELF_H__
