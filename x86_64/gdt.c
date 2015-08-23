#include <gdt.h>
#include <stdint.h>

#include <debug.h>

uint64_t GDT_addr[16] = {0};
uint64_t g_gdtr[2];

/*************************************************
      ВАЖНОЕ ЗАМЕЧАНИЕ

  На самом деле, необходимости в строгой GDT нет.
  Процессор в 64-разрядном режиме ложит кирпич на
  селекторы, находящиеся в сегментных регистрах,
  и ведет себя так, будто их и нет вовсе, используя
  непосредственно линейные адреса.
  
  Однако, и тут есть свои нюансы:
  - процессоры Intel забивают болт на абсолютно ВСЕ
    сегментные регистры;
  - процессоры AMD НЕ забивают на регистры FS и GS
    (интересно, ими хоть кто-то пользуется?), для
    которых сегментация работает аналогично 
    старому 32-разрядному режиму.
  То есть использовать сегментацию, по меньшей мере,
  глупо (несовместимо). Однако, на всякий случай я 
  все-таки устанавливаю селекторы сегментов кода и
  данных,несмотря на то, что процессор никогда к 
  ним не обратится.
  
  Единственные дескрипторы сегментов, имеющие смысл
  в 64-разрядном режиме - это дескриптор LDT (хотя 
  и он тоже не ясно, за каким хреном нужен - 
  дескрипторы LDT и TSS внутри него хранить нельзя,
  а остальные все равно игнорируются) и дескриптор
  64-разрядной TSS (наверное, единственный вообще 
  используемый сегмент в 64-разрядном режиме).
  
  TSS64 используется исключительно для получения
  адреса новой вершины стека при прерываниях (ну
  и еще для битовой карты ввода-вывода, да).
  
**************************************************/

/*
 * Помещает запись в GDT о КОНКРЕТНОМ сегменте
 */
void GDT_put(uint64_t seg, uint64_t data)
{
  GDT_addr[seg] = data;
}

/*
 * Находит в GDT свободный дескриптор и помещает туда запись о сегменте
 */
uint64_t GDT_autoput(uint64_t data)
{
  uint64_t seg;
  for(seg = 1; (seg < 8192)&&(GDT_addr[seg] != 0); seg++);
  GDT_put(seg, data);
  return seg;
}

void GDT_load(void)
{
  g_gdtr[0]=(((uint64_t)GDT_addr)<<16)|0xFFFF;
  g_gdtr[1]=(((uint64_t)GDT_addr)>>48)&0xFFFF;
  asm("lgdt g_gdtr");
  
  // Заменим значения в сегментных регистрах (там после GRUB остался мусор)
  asm(" \n \
    mov $0x10, %%ax\n \
    mov %%ax, %%ds \n \
    mov %%ax, %%ss \n \
    mov %%ax, %%es \n \
    mov %%ax, %%gs \n \
    mov %%ax, %%fs \n \
  ":::"rax");
}

void GDT_init(void)
{  
  // Нулевой дескриптор GDT не используется
  GDT_put(0,0);
  // Сегмент кода ядра
  GDT_smartput(1, 0x00000000, 0xFFFFFFFF, SEG_64BIT_CODE | SEG_NOTSYS | SEG_PRESENT | SEG_CODE_EO | SEG_DPL0);
  // Сегмент данных ядра
  GDT_smartput(2, 0x00000000, 0xFFFFFFFF, SEG_NOTSYS | SEG_PRESENT | SEG_DATA_RW | SEG_DPL0);
  // Сегмент кода пользователя
  GDT_smartput(3, 0x00000000, 0xFFFFFFFF, SEG_64BIT_CODE | SEG_NOTSYS | SEG_PRESENT | SEG_CODE_EO | SEG_DPL3);
  // Сегмент данных пользователя
  GDT_smartput(4, 0x00000000, 0xFFFFFFFF, SEG_NOTSYS | SEG_PRESENT | SEG_DATA_RW | SEG_DPL3);

  GDT_load();
}

/*
 * Для AP уже все готово
 */
void GDT_init_ap(void)
{
  GDT_load();
}

/* 
 * Функция берет большую часть некрасивой работы по преобразованию 
 * удобочитаемого представления лимита, базы и флагов в кривое
 * Intel'овское 
 */
uint64_t GDT_smartput(uint64_t seg, uint64_t base, uint64_t limit, uint64_t flags)
{
  uint64_t data;
  
  if(limit>0x000FFFFF)
  {
    limit = limit >> 12;
    flags |= SEG_BIG;
  }

  // Кошмар просто
  data = (limit&0x0000FFFF) | ((base<<16)&0xFFFF0000) | ((((base>>16)&0x000000FF)|(base&0xFF000000)|(limit&0x000F0000)|flags) << 32);
  GDT_put(seg, data);

  if((flags&SEG_TSS64) == SEG_TSS64)
  {
    /*
     * Обрабатываем 64-бит TSS. Для этого нам необходимы два дескриптора:
     * В первом будет лежать оставшаяся часть адреса,
     * во втором - все как обычно
     */
    GDT_put(++seg, base>>32);
    //base &= 0xFFFFFFFF;
    seg--;
  }

  /*if((flags&SEG_TSS64) == SEG_TSS64)
    return seg-1;
  else*/
    return seg;
}

/* 
 * Да-да, вы догадались =)
 */
uint64_t GDT_smartaput(uint64_t base, uint64_t limit, uint64_t flags)
{
  uint64_t seg;
  for(seg = 1; (seg < 8192)&&(GDT_addr[seg] != 0); seg++);
  seg = GDT_smartput(seg, base, limit, flags);
  return seg;
}
