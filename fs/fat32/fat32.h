#ifndef __FAT32_H__
#define __FAT32_H__

#include <stdint.h>
#include <vfs.h>

/*
 * Блок параметров BIOS
 * С него начинается FAT
 */
typedef struct __attribute__((packed))
{
  /*
   * Поля, общие для всех FAT[12,16,32]
   */
  char jump[3];      // jmp short 0x??; nop (EB ?? 90), ?? = 58 для FAT32
  char oem_id[8];    // Метка системы, форматировавшей диск
  uint16_t bytes_ps; // Байт в секторе
  uint8_t  sec_pc;   // Секторов в кластере
  uint16_t num_rs;   // Число зарезервированных секторов (включая загрузочный)
  uint8_t  num_fat;  // Количество таблиц FAT, обычно 2
  uint16_t num_de;   // Число записей каталогов
  uint16_t num_sec;  // Общее число секторов в томе, для FAT32 всегда 0
  uint8_t  media_dt; // Media Descriptor Type, для всего, кроме флоппи 0xF8
  uint16_t sec_pf;   // Секторов на таблицу FAT, всегда 0 для FAT32
  uint16_t sec_pt;   // Секторов в дорожке
  uint16_t head_pv;  // Головок на носителе
  uint32_t hidden;   // Число скрытых секторов (адрес начала тома), обычно 0
  uint32_t num_sec2; // Число секторов в томе для FAT32
  /*
   * Поля, специфичные для FAT32
   */
  uint32_t sec_pf2;  // Секторов на таблицу FAT
  uint16_t flags;    // Флаги
  uint16_t version;  // Версия FAT (обычно 0)
  uint32_t root_c;   // Кластер корневой директории, обычно 2
  uint16_t fsinfo_c; // Кластер структуры FSInfo
  uint16_t boot_bkp; // Кластер с копией загрузочного сектора
  uint8_t  rvd0[12]; // Зарезервировано
  uint8_t  drv_num;  // Номер диска, 0x80 для всего, кроме FDD
  uint8_t  nt_flags; // Флаги Windows NT, обычно 0
  uint8_t  sign;     // Сигнатура, 0x28 или 0x29
  uint32_t vol_id;   // Идентификатор диска, обычно - случайное число
  char   label[11];  // Метка диска, для пользователя
  char   fs_id[8];   // Идентификатор ФС, всегда "FAT32   "
  uint8_t boot[420]; // Код загрузчика
  uint8_t magic;     // 0xAA55
} fat32_bpb;

/*
 * Запись в каталоге
 */
typedef struct __attribute__((packed))
{
  char name[8];   // Имя файла
  char ext[3];    // Расширение
  uint8_t attr;   // Атрибуты
  uint8_t rsrvd0; // Зарезервировано
  uint8_t unk1;   // Время создания в десятках секунд (?)
  uint16_t 
    c_hour : 5, 
    c_min : 6,
    c_sec : 5;      // Время создания
  uint16_t
    c_year : 7,
    c_month : 4,
    c_day : 5;      // Дата создания
  uint16_t
    a_year : 7,
    a_month : 4,
    a_day : 5;      // Дата последнего обращения
  uint16_t first_clus_h;  // Старшие 16 бит номера первого кластера
  uint16_t 
    m_hour : 5, 
    m_min : 6,
    m_sec : 5;      // Время изменения
  uint16_t
    m_year : 7,
    m_month : 4,
    m_day : 5;      // Дата изменения
  uint16_t first_clus_l;  // Младшие 16 бит номера первого кластера
  uint32_t size;    // Размер файла
} fat32_direntry;

enum FAT32_ATTRIBUTES
{
  FAT32_DIREND    = 0x00,  // Конец каталога
  FAT32_READONLY  = 0x01,  // Только чтение
  FAT32_HIDDEN    = 0x02,  // Скрытый
  FAT32_SYSTEM    = 0x04,  // Системный
  FAT32_VOLUMEID  = 0x08,  // Идентификатор тома
  FAT32_LFN       = 0x0F,  // Да и не файл это вовсе, а кусок имени
  FAT32_DIRECTORY = 0x10,  // Каталог
  FAT32_ARCHIVE   = 0x20,  // Архивный
  FAT32_DELETED   = 0xE5,  // Файл удален
};

vfs_node_t *fat32_init(vfs_node_t *node);
int fat32_fini(vfs_node_t *node);

#endif //__FAT32_H__

