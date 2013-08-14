#include <smp.h>
#include <klibc.h>
#include <ktty.h>
#include <stdint.h>
#include <page.h>
#include <apic.h>

void smp_init(void)
{
/*
 * Попытка прочитать SMP
 */
  uint32_t *addr = 0xFFFFFFFFC0000000;  // По этому адресу отображен первый мегабайт памяти
  int i;
  uint8_t found = 0;
/*
 * Сигнатура может быть в нескольких местах:
 * 1) Первый килобайт Extended BIOS Data Area (хз где это)
 * 2) Последний килобайт основной памяти (639-640Кб)
 * 3) В области BIOS ROM (то ли 0xe0000-0xfffff, то ли 0xf0000-0xfffff)
 * 4) В конце физической памяти (опять хз где конкретно)
 * Посему проверим только варианты 2 и 3
 */
  // Подключим страницу, ее нет в адресном пространстве адра
  mount_page(0x9F000, 0xFFFFFFFFC009F000);
  for(i = (0x9FC00>>2); i < (0xA0000>>2); i++)
    if(addr[i] == SMP_MAGIC)
    {
      //printf("Found at addr %X\n",addr+(i));
      found = 1;
      break;
    }

  // А этот адрес уже примонтирован
  if(!found)
    for(i = (0xF0000>>2); i < (0x100000>>2); i++) // FIXME: должно быть 0xE0000!
      if(addr[i] == SMP_MAGIC)
      {
        //printf("Found at addr %X\n",addr+(i));
        found = 1;
        break;
      }
  if(!found)
  {
    ktty_puts("SMP not found\n");
  } else {
    SMP *s = (SMP *)(&addr[i]);
    //printf("SMP addr %l\n", s);
    printf("SMP version 1.%d found\n",s->version);
    if(s->features[0] == 0)
    {
      char oem[9] = {0};
      char product[13] = {0};
      // Описание системы расположено по адресу s->config
      SMP_config *sc = (SMP_config *)( (uint64_t)addr + (uint64_t)(s->config) );
      //printf("SC is %l\n", sc);
      printf("Specification revision 1.%d\n", sc->revision);
      strncpy(oem, sc->oemid, 8);
      oem[8] = 0;
      strncpy(product, sc->productid, 12);
      product[12] = 0;
      printf("Manufacturer %s, board %s\n", oem, product);
      printf("Local APIC at addr %X\n", sc->lapic_addr);
      // Инициализируем APIC
      apic_init(sc->lapic_addr);
      char *addr2 = (char *)((uint64_t)sc + sizeof(SMP_config));
      SMP_proc *p;
      for (i = 0; i < sc->count; i++)
      {
        switch(*addr2)
        {
          case 0:
            // Процессор
            p = (SMP_proc *)addr2;
            printf("Processor, LAPIC ID %d, En : %d, BSP : %d\n", p->lapic_id, p->enabled, p->bsp);
            addr2 += 20;//sizeof(SMP_proc);
            break;

          default:
            printf("Unknown byte %x at %l\n", *addr2, addr2);
            i = sc->count;  // TODO: Удалить!
            addr2 += 8;
            break;
        }
      }

    } else {
      // Система представляет собой одну из стандартных конфигураций
      printf("Using configuration %d\n", s->features[0]);
    }
  }
  // Отмонтируем
  umount_page(0xFFFFFFFFC009F000);
}

