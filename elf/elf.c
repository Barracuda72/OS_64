#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elf.h"

/*
 * Проверяет заголовок ELF-файла на корректность
 */
int elf64_check_magic(Elf64_Ehdr *hdr)
{
  // Заголовок не должен быть пустым
  if (!hdr)
    return 0;

  // Проверка магической константы в начале заголовка
  if(memcmp(hdr->e_ident, ELFMAG, SELFMAG) != 0)
  {
    ktty_puts("ELF header magic incorrect!");
    return 0;
  }

  return 1;
}

/*
 * Проверяет, имеет ли файл нужный класс и архитектуру
 */
int elf64_is_supported(Elf64_Ehdr *hdr)
{
  // Проверим заголовок
  if (!elf64_check_magic(hdr))
  {
    ktty_puts("File isn't an ELF");
    return 0;
  }

  // Проверим соответствие классу
  if (hdr->e_ident[EI_CLASS] != ELFCLASS64)
  {
    ktty_puts("ELF file has unsupported class");
    return 0;
  }

  // Проверим порядок битов
  if (hdr->e_ident[EI_DATA] != ELFDATA2LSB)
  {
    ktty_puts("ELF file has unsupported byte order");
    return 0;
  }

  // Проверим архитектуру, для которой был собран файл
  if (hdr->e_machine != EM_AMD64)
  {
    ktty_puts("ELF file has unsupported architecture");
    return 0;
  }

  // Проверим версию (обычно 1, но все-таки)
  if ((hdr->e_version != EV_CURRENT) || (hdr->e_ident[EI_VERSION] != EV_CURRENT))
  {
    ktty_puts("ELF file has unsupported version");
    return 0;
  }

  // Наш загрузчик поддерживает только исполняемые и перемещаемые файлы
  if ((hdr->e_type != ET_EXEC) && (hdr->e_type != ET_REL))
  {
    ktty_puts("ELF file has unsupported type");
    return 0;
  }

  return 1;
}

/*
 * Предосталяет доступ к таблице секций файла
 */
Elf64_Shdr *elf64_sheader(Elf64_Ehdr *hdr) 
{
  return (Elf64_Shdr *)(((long)hdr) + hdr->e_shoff);
}

/*
 * Предоставляет доступ к таблице программных заголовков файла
 */
Elf64_Phdr *elf64_pheader(Elf64_Ehdr *hdr)
{
  return (Elf64_Phdr *)(((long)hdr) + hdr->e_phoff);
}

/*
 * Возвращает заголовок конкретной секции по ее индексу
 */
Elf64_Shdr *elf64_section(Elf64_Ehdr *hdr, int idx)
{
  // Проверка на выход за границы массива
  if (idx >= hdr->e_shnum)
    return NULL;

  return &elf64_sheader(hdr)[idx];
}

/*
 * Возвращает адрес таблицы строк
 */
char *elf64_str_table(Elf64_Ehdr *hdr)
{
  // Проверим, есть ли в файле вообще таблица строк
  Elf64_Half idx = hdr->e_shstrndx;
  if (idx == SHN_UNDEF)
    return NULL;

  // Таблица есть
  return (char *)hdr + elf64_section(hdr, idx)->sh_offset;
}

char *elf64_find_string(Elf64_Ehdr *hdr, int offset)
{
  char *strtab = elf64_str_table(hdr);
  // Если таблицы строк нет
  if (strtab == NULL)
    return NULL;

  return strtab + offset;
}

/*
 * Поиск значения символа
 */
// TODO!
long elf64_get_symval(Elf64_Ehdr *hdr, int table, uint idx){}

/*
 * Вспомогательная функция для вычисления базового адреса исполняемого файла
 */
int elf64_get_baseaddr(Elf64_Ehdr *hdr)
{
  // Согласно документации, базовый адрес исполняемого файла = наименьшему из
  // адресов сегментов PT_LOAD, взятому по границе страницы
  // Здесь мы пользуемся тем, что сегменты PT_LOAD должны быть расположены в
  // файле по возрастанию виртуальных адресов

  // Базовый адрес имеет смысл только для исполняемых и разделяемых файлов, но
  // не для перемещаемых
  if (hdr->e_type != ET_EXEC)
    return 0;

  // Получаем таблицу программных заголовков
  Elf64_Phdr *phdr = elf64_pheader(hdr);

  unsigned int i;
  // Переберем программные заголовки
  for (i = 0; i < hdr->e_phnum; i++)
  {
    // Взяли конкретный сегмент
    Elf64_Phdr *segment = &phdr[i];

    // Это загружаемый сегмент
    if (segment->p_type == PT_LOAD)
    {
      return segment->p_vaddr&(~0xFFF);
    }
  }

  return 0;
}

/*
 * Обработка записи с явной и неявной корректировкой
 */
// TODO!
int elf64_do_rela(Elf64_Ehdr *hdr, Elf64_Rela *rel, Elf64_Shdr *reltab)
{
  // Находим целевую секцию
  Elf64_Shdr *target = elf64_section(hdr, reltab->sh_info);
  
  // Начало секции данного символа
  long addr = (long)hdr + target->sh_offset;
  // Ссылка на изменяемое значение
  long *ref = 0;
  if (hdr->e_type == ET_REL)
    ref = (long *)(addr + rel->r_offset);
  else // ET_EXEC
    // TODO!!!!
    ref = rel->r_offset;
  // Выбор явного или неявного значения корректировки
  long addend = 0;
  if (reltab->sh_type == SHT_REL)
    addend = *ref;
  else // SHT_RELA
    addend = rel->r_addend;

  // Получаем значение символа
  long symval = 0;
  if (ELF64_R_SYM(rel->r_info) != SHN_UNDEF)
  {
    symval = elf64_get_symval(hdr, reltab->sh_link, ELF64_R_SYM(rel->r_info));
    if (symval == ELF_REL_ERR)
      return ELF_REL_ERR;
  }

  // Для релокации типа IRELATIVE
  long (*irela)() = NULL;

  // Выполняем релокацию
  switch (ELF64_R_TYPE(rel->r_info))
  {
    case R_AMD64_NONE:
      // Нет релокации
      break;

    case R_AMD64_64:
      // Символ + смещение
      *ref = DO_AMD64_64(symval, addend);
      break;

    case R_AMD64_PC32:
      // Символ + смещение - смещение секции
      *((int *)ref) = DO_AMD64_PC32(symval, addend, (long)ref);
      break;

    case R_AMD64_RELATIVE:
      // Символ + базовый адрес
      *ref = DO_AMD64_RELATIVE(elf64_get_baseaddr(hdr), addend);
      break;
    case R_X86_64_IRELATIVE:
      // Значение функции, лежащей по адресу = символ + базовый адрес
      //irela = DO_AMD64_RELATIVE(elf64_get_baseaddr(hdr), addend);
      irela = addend;
      *ref = irela();
      break;

    default:
      ktty_puts("Unsupported relocation type");
      return ELF_REL_ERR;     
  }

  return symval;
}

/*
 * Маленькая вспомогательная функция по извлечению элемента из таблицы
 * релокаций
 */
Elf64_Rela *elf64_get_rela(Elf64_Ehdr *hdr, Elf64_Shdr *section, unsigned int idx)
{
  if (section->sh_type == SHT_REL)
    return (Elf64_Rela *)(&((Elf64_Rel *)((long)hdr + section->sh_offset))[idx]);
  else // SHT_RELA
    return &((Elf64_Rela *)((long)hdr + section->sh_offset))[idx];
}

/*
 * Первая стадия загрузки:
 * Выделение памяти под секции, помеченные как NOBITS,
 * для перемещаемых файлов
 */
// TODO!
int elf64_load_stage1(Elf64_Ehdr *hdr){}

/*
 * Вторая стадия загрузки:
 * Обработка релокаций в исполняемом/перемещаемом файле
 */
int elf64_load_stage2(Elf64_Ehdr *hdr)
{
  // Получаем указатель на таблицу секций
  Elf64_Shdr *shdr = elf64_sheader(hdr);

  // Пройдемся по заголовкам секций
  unsigned int i, idx;
  for (i = 0; i < hdr->e_shnum; i++)
  {
    // Выделим текущую секцию
    Elf64_Shdr *section = &shdr[i];

    switch (section->sh_type)
    {
      case SHT_REL:  // Если это перемещаемая секция без допинформации
      case SHT_RELA: // Перемещаемая секция с допинформацией
        // Пройдемся по элементам секции
        for (idx = 0; idx < section->sh_size / section->sh_entsize; idx++)
        {
          // Выделим релокацию
          Elf64_Rela *relatab = elf64_get_rela(hdr, section, idx);
          int result = elf64_do_rela(hdr, relatab, section);
          if (result == ELF_REL_ERR)
          {
            ktty_puts("Failed to relocate symbol");
            return ELF_LOAD_ERR;
          }
        }
        break;

      default:
        ktty_puts("Unsupported symbol type");
        //return ELF_REL_ERR;
    }
  }

  return 0;
}

/*
 * Третья стадия загрузки:
 * Обработка программных заголовков и размещение их в памяти
 */
int elf64_load_stage3(Elf64_Ehdr *hdr)
{
  // Получаем таблицу программных заголовков
  Elf64_Phdr *phdr = elf64_pheader(hdr);

  unsigned int i;
  // Переберем программные заголовки
  for (i = 0; i < hdr->e_phnum; i++)
  {
    // Взяли конкретный сегмент
    Elf64_Phdr *segment = &phdr[i];

    // Разместим в памяти сегменты, помеченные как загружаемые
    if (segment->p_type == PT_LOAD)
    {
      // TODO: HACK: нужно не копировать данные, а сразу перемещать на нужный
      // адрес
      // Выделим память под сегмент (предполагается, что он уже выровнен на
      // границу страницы)
      int tail = segment->p_vaddr&0xFFF;
      char *addr = kmalloc(segment->p_memsz + tail);
      // Отобразим сегмент по виртуальному адресу
      addr = mremap(addr, segment->p_memsz + tail, segment->p_memsz+tail, 3, segment->p_vaddr & (~0xFFF));

      // Теперь нас интересует только виртуальный адрес
      addr = segment->p_vaddr;
      // Забъем нулями весь сегмент
      memset(addr, 0, segment->p_memsz);
      // Скопируем туда данные из файла
      memcpy(addr, (long)hdr + segment->p_offset, segment->p_filesz);
    }
  }

  return 0;
}

/*
 * Выполняет обработку перемещаемого файла
 */
void *elf64_load_rel(Elf64_Ehdr *hdr)
{
  int result;
  // Первая стадия
  result = elf64_load_stage1(hdr);

  if (result == ELF_LOAD_ERR)
  {
    ktty_puts("Loading REL file failed at Stage 1");
    return NULL;
  }

  // Вторая стадия
  result = elf64_load_stage2(hdr);

  if (result == ELF_LOAD_ERR)
  {
    ktty_puts("Loading REL file failed at Stage 2");
    return NULL;
  }

  return (void *)(hdr->e_entry);
}

/*
 * Выполняет обработку исполняемого файла
 */
void *elf64_load_exec(Elf64_Ehdr *hdr)
{
  int result;


  // Загружаем программные заголовки
  result = elf64_load_stage3(hdr);

  if (result == ELF_LOAD_ERR)
  {
    ktty_puts("Loading EXEC file failed at Stage 3");
    return NULL;
  }

  // Обрабатываем релоки
  result = elf64_load_stage2(hdr);

  if (result == ELF_LOAD_ERR)
  {
    ktty_puts("Loading EXEC file failed at Stage 2");
    return NULL;
  }

  return (void *)(hdr->e_entry);
}

/*
 * Общая функция обработки файла
 */
void *elf64_load_file(Elf64_Ehdr *hdr)
{
  // Проверим, сможем ли мы загрузить данный файл
  if (!elf64_is_supported(hdr))
  {
    ktty_puts("File can't be loaded");
    return NULL;
  }

  // Разные типы обрабатываются немного по-разному
  switch (hdr->e_type)
  {
    case ET_EXEC:
      return elf64_load_exec(hdr);

    case ET_REL:
      return elf64_load_rel(hdr);
  }

  // Что-то пошло не так, сюда мы попасть не должны, так как фцнкция проверки
  // файла не пропустит ничего кроме ET_EXEC и ET_REL, которые оба обработаны
  // выше
  return NULL;
}

