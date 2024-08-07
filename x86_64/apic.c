#include <stdint.h>
#include <apic.h>
#include <cpuid.h>
#include <ioport.h>
#include <intr.h>
#include <page.h>
#include <smp.h>
#include <kprintf.h>

#include <debug.h>

uint32_t *lapic_addr = 0;
uint32_t lapic_hw_addr = 0;
uint32_t *ioapic_addr = 0;
void apic_spur_isr();
void apic_tmr_isr();
// Количество срабатываний таймера в секунду
uint32_t quantum = APIC_CLOCKS_PER_SEC; // 10;
uint32_t apic_divisor = 16;

asm("\n \
apic_spur_isr: \n \
  iretq \n \
");

asm("\n \
.globl apic_eoi \n \
apic_eoi: \n \
  push %rax \n \
  mov lapic_addr, %rax \n \
  cmp $0, %rax \n \
  je .ret \n \
  add $0xB0, %rax #APIC_EOI \n \
  movl $0, (%rax) \n \
.ret: \n \
  pop %rax \n \
  ret \n \
");

/*
 * Вспомогательная процедура, организующая задержку на основе хитрого
 * манипулирования каналом 2 PIT
 */
static void pit_wait(uint32_t microsec)
{
  uint8_t tmp;
  uint64_t divisor = 1193182*microsec/100000;
  // Установим канал 2 PIT в режим единичного срабатывания
  // через 1/100 секунды
  outb(0x61, (inb(0x61)&0xFD)|1);
  // Команда - "принять делитель в двоичном формате для 
  // канала 2 и установить режим единичного срабатывания"
  // (2<<6) - второй канал
  // (3<<4) - полный делитель
  // (1<<1) - единичное срабатывание
  // (0<<0) - делитель - двоичное число (не BCD)
  outb(0x43, 0xB2); 
  outb(0x42, divisor&0xFF); // Младший байт
  inb(0x60); // маленькая задержка
  outb(0x42, (divisor>>8)&0xFF); // Старший байт
 
  // Сбросим счетчик PIT (начнем отсчет)
  tmp = inb(0x61)&0xFE;
  outb(0x61, (uint8_t)tmp);     // выкл
  outb(0x61, (uint8_t)tmp|1);   // вкл
 
  // Ждем обнуления счетчика PIT
  while(!(inb(0x61)&0x20));
}

void apic_init(uint32_t lapic_a)
{
  uint32_t cpubusfreq;
//  uint32_t apic_divisor;
  // Отключим PIC
  outb(0x21, 0xFF);
  outb(0xA1, 0xFF);
  // Примонтируем страницу с регистрами APIC на адрес
  // 0xFFFFFFFFC00F0000. По этому адресу в пространство
  // ядра отображено ПЗУ BIOS. Зачем оно нам?
  lapic_addr = APIC_LAPIC_ADDR;
  lapic_hw_addr = lapic_a;
  mount_page_hw((void*)lapic_a, lapic_addr);
  // Установим прерывания
  //ext_intr_install(0x20, apic_tmr_isr);
  ext_intr_install(0x27, apic_spur_isr);
 
  // Инициализируем LAPIC 
  lapic_addr[APIC_DFR] = 0xFFFFFFFF;
  lapic_addr[APIC_LDR] = (lapic_addr[APIC_LDR]&0x00FFFFFF)|1;
  lapic_addr[APIC_LVT_TMR] = APIC_DISABLE;
  lapic_addr[APIC_LVT_PERF] = APIC_NMI;
  //lapic_addr[APIC_LVT_LINT0] = APIC_DISABLE;
  lapic_addr[APIC_LVT_LINT0] &= 0xFFFE58FF;  // Delivery = fixed
  //lapic_addr[APIC_LVT_LINT1] = APIC_DISABLE;
  lapic_addr[APIC_LVT_LINT1] &= 0xFFFE58FF;
  lapic_addr[APIC_LVT_LINT1] |= 0x400; // Delivery = NMI
  lapic_addr[APIC_TASKPRIOR] = 0;

  // Подключим линии INTR и NMI к LAPIC
  outb(0x22, 0x70);
  outb(0x23, 1);
 
  // Включаем APIC, разрешая "странное" прерывание
  lapic_addr[APIC_SPURIOUS] = 39|APIC_SW_ENABLE;
  //apic_enable();
  // Указываем таймеру APIC номер прерывания, и этим
  // включая его в режиме единичного срабатывания
  lapic_addr[APIC_LVT_TMR] = 32;
  // Установим делитель 16
  lapic_addr[APIC_TMRDIV] = 0x03;
#if 0 
  // Установим канал 2 PIT в режим единичного срабатывания
  // через 1/100 секунды
  outb(0x61, (inb(0x61)&0xFD)|1);
  // Команда - "принять делитель в двоичном формате для 
  // канала 2 и установить режим единичного срабатывания"
  // (2<<6) - второй канал
  // (3<<4) - полный делитель
  // (1<<1) - единичное срабатывание
  // (0<<0) - делитель - двоичное число (не BCD)
  outb(0x43, 0xB2); 
  // 1193180/100 Hz = 11931 = 0x2E9B
  outb(0x42, 0x9B); // Младший байт
  inb(0x60); // маленькая задержка
  outb(0x42, 0x2E); // Старший байт
 
  // Сбросим счетчик PIT (начнем отсчет)
  tmp = inb(0x61)&0xFE;
  outb(0x61, (uint8_t)tmp);     // выкл
  outb(0x61, (uint8_t)tmp|1);   // вкл
  //BREAK();
  // Сбросим счетчик таймера APIC (установим в -1)
  lapic_addr[APIC_TMRINITCNT] = 0xFFFFFFFF;
 
  // Ждем обнуления счетчика PIT
  while(!(inb(0x61)&0x20));
#else
  lapic_addr[APIC_TMRINITCNT] = 0xFFFFFFFF;
  pit_wait(1000);
#endif
  // Остановим таймер APIC
  lapic_addr[APIC_LVT_TMR] = APIC_DISABLE;
 
  // Посчитаем...
  cpubusfreq = ((0xFFFFFFFF - lapic_addr[APIC_TMRCURRCNT]) + 1)*16*100;
  kprintf("CPU bus freq: %d\n", cpubusfreq);
  apic_divisor = cpubusfreq / quantum / 16;
 
  // Теперь в tmp - нужное нам значение
  lapic_addr[APIC_TMRINITCNT] = (apic_divisor < 16 ? 16 : apic_divisor);
  // Окончательно включаем таймер в периодическом режиме
  lapic_addr[APIC_LVT_TMR] = 32|TMR_PERIODIC;
  // Повторная установка делителя необходима для некоторого
  // кривого железа, по мануалам она не нужна
  lapic_addr[APIC_TMRDIV] = 0x03;
}

void apic_init_ap()
{
  mount_page_hw((void*)lapic_hw_addr, lapic_addr);

  lapic_addr[APIC_DFR] = 0xFFFFFFFF;
  lapic_addr[APIC_LDR] = (lapic_addr[APIC_LDR]&0x00FFFFFF)|1;
  lapic_addr[APIC_LVT_TMR] = APIC_DISABLE;
  lapic_addr[APIC_LVT_PERF] = APIC_NMI;
  lapic_addr[APIC_LVT_LINT0] &= 0xFFFE58FF;  // Delivery = fixed
  lapic_addr[APIC_LVT_LINT1] &= 0xFFFE58FF;
  lapic_addr[APIC_LVT_LINT1] |= 0x400; // Delivery = NMI
  lapic_addr[APIC_TASKPRIOR] = 0;

  // Подключим линии INTR и NMI к LAPIC
  outb(0x22, 0x70);
  outb(0x23, 1);

  lapic_addr[APIC_SPURIOUS] = 39|APIC_SW_ENABLE;

  lapic_addr[APIC_TMRINITCNT] = (apic_divisor < 16 ? 16 : apic_divisor);
  lapic_addr[APIC_LVT_TMR] = 32|TMR_PERIODIC;
  lapic_addr[APIC_TMRDIV] = 0x03;
}

// FIXME!
uint8_t apic_present()
{
/*
  uint32_t eax, edx;
  cpuid(1, &eax, &edx);
  return edx & CPUID_FLAG_APIC;
*/
  return 1;
}
 
void apic_set_base(uint32_t apic)
{
  uint32_t edx = 0;
  uint32_t eax = (apic & 0xfffff100) | APIC_BASE_MSR_ENABLE;
 
  CPU_write_MSR(APIC_BASE_MSR, eax, edx);
}
 
uint32_t apic_get_base()
{
  return CPU_read_MSR(APIC_BASE_MSR)&0xFFFFF100;
}
 
void apic_enable()
{
  apic_set_base(apic_get_base());

  // Включим "странное" прерывание, чтобы начать
  // получать все остальные прерывания
  lapic_addr[APIC_SPURIOUS] |= APIC_SW_ENABLE;
}

void ioapic_init(uint32_t ioapic_phys)
{
  ioapic_addr = APIC_IOAPIC_ADDR;
  mount_page_hw((void*)ioapic_phys, ioapic_addr);

  // Маскируем IRQ0 (это PIC)
  ioapic_write(0x10, 0x10000);
  ioapic_write(0x11, 0);

  // Настройка IRQ1 (клавиатура) на Int21
  ioapic_write(0x12, 0x21);
  ioapic_write(0x13, 0);

  // Настройка IRQ16 (PCI 1) на Int30
  ioapic_write(0x30, 0xA030);
  ioapic_write(0x31, 0);
  kprintf("IO APIC init complete\n");
}

uint32_t ioapic_read(uint8_t reg)
{
  ioapic_addr[IOAPIC_REGSEL] = reg;
  return ioapic_addr[IOAPIC_REGWIN];
}

void ioapic_write(uint8_t reg, uint32_t val)
{
  ioapic_addr[IOAPIC_REGSEL] = reg;
  ioapic_addr[IOAPIC_REGWIN] = val;
}

/*
 * Инициализация AP
 */
void ap_init(uint8_t apic_id)
{
  /*
   * Формат команды:
   * 63..56 : назначение
   * 19..18 : сокращение:
   *   00 - нет сокращения
   *   01 - самому себе
   *   10 - всем, в том числе и себе
   *   11 - всем, кроме себя
   * 15 : trigger mode (edge/level)
   * 14 : level (de-assert/assert)
   * 12 : состояние доставки: доставлено/в процессе
   * 11 : тип места назначения: физический/логический
   * 10..8 : режим доставки
   *   000 - фиксированный
   *   001 - минимальный приоритет
   *   010 - как SMI
   *   011 - зарезервировано
   *   100 - как NMI
   *   101 - INIT IPI
   *   110 - Startup IPI
   *   111 - зарезервировано
   * 7..0 : вектор>>12
   */

  // Запоминаем текущее количество процессоров
  // uint8_t cpu_nr_curr = cpu_nr;

  // INIT IPI
  lapic_addr[APIC_ICRH] = (apic_id<<24); // Получатель
  lapic_addr[APIC_ICRL] = 0x00004500;    // level-triggered 

  // Ждем доставки
  while(lapic_addr[APIC_ICRL]&0x1000);

  // Согласно Intel, ждем 10 миллисекунд
  pit_wait(10000);

  // Startup IPI
  lapic_addr[APIC_ICRH] = (apic_id<<24);
  lapic_addr[APIC_ICRL] = 0x00004600 | (AP_CODE>>12);
  //BREAK();

  // Согласно Intel, ждем 200 микросекунд и повторяем команду
  pit_wait(200);


  lapic_addr[APIC_ICRH] = (apic_id<<24);
  lapic_addr[APIC_ICRL] = 0x00004600 | (AP_CODE>>12);

  // Процессор инициализирован, ждем, пока полностью не проснется
/*  while (cpu_nr == cpu_nr_curr)
    ;*/
}

