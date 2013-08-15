#include <stdint.h>
#include <apic.h>
#include <cpuid.h>
#include <ioport.h>
#include <intr.h>
#include <page.h>

#include <debug.h>

uint32_t *lapic_addr = 0;
uint32_t *ioapic_addr = 0;
void apic_spur_isr();
void apic_tmr_isr();
// Количество срабатываний таймера в секунду
uint32_t quantum = 1;//1000;

asm("\n \
apic_spur_isr: \n \
  iretq \n \
");

asm("\n \
apic_tmr_isr: \n \
  push %rax \n \
  mov lapic_addr, %rax \n \
  cmp $0, %rax \n \
  je .ret \n \
  add $0xB0, %rax #APIC_EOI \n \
  movl $0, (%rax) \n \
.ret: \n \
  pop %rax \n \
  iretq \n \
");

void apic_init(uint32_t lapic_a)
{
  intr_disable();
  uint32_t tmp, cpubusfreq;
  // Отключим PIC
  outb(0x21, 0xFF);
  outb(0xA1, 0xFF);
  // Примонтируем страницу с регистрами APIC на адрес
  // 0xFFFFFFFFC00F0000. По этому адресу в пространство
  // ядра отображено ПЗУ BIOS. Зачем оно нам?
  lapic_addr = APIC_LAPIC_ADDR;
  mount_page_hw(lapic_a, lapic_addr);
  // Установим прерывания
  ext_intr_install(0x20, apic_tmr_isr);
  ext_intr_install(0x27, apic_spur_isr);
 
  // Инициализируем LAPIC 
  lapic_addr[APIC_DFR] = 0xFFFFFFFF;
  lapic_addr[APIC_LDR] = (lapic_addr[APIC_LDR]&0x00FFFFFF)|1;
  lapic_addr[APIC_LVT_TMR] = APIC_DISABLE;
  lapic_addr[APIC_LVT_PERF] = APIC_NMI;
  lapic_addr[APIC_LVT_LINT0] = APIC_DISABLE;
  lapic_addr[APIC_LVT_LINT1] = APIC_DISABLE;
  lapic_addr[APIC_TASKPRIOR] = 0;
 
  // Включаем APIC, разрешая "странное" прерывание
  lapic_addr[APIC_SPURIOUS] = 39|APIC_SW_ENABLE;
  //apic_enable();
  // Указываем таймеру APIC номер прерывания, и этим
  // включая его в режиме единичного срабатывания
  lapic_addr[APIC_LVT_TMR] = 32|APIC_SW_ENABLE;
  // Установим делитель 16
  lapic_addr[APIC_TMRDIV] = 0x03;
 
  // Установим канал 2 PIT в режим единичного срабатывания
  // Ждем 1/100 секунды
  outb(0x61, (inb(0x61)&0xFD)|1);
  outb(0x43, 0xB2);
  // 1193180/100 Hz = 11931 = 0x2E9B
  outb(0x42, 0x9B); // LSB
  inb(0x60); // маленькая задержка
  outb(0x42, 0x2E); // MSB
 
  // Сбросим счетчик PIT (начнем отсчет)
  tmp = inb(0x61)&0xFE;
  outb(0x61, (uint8_t)tmp);     // выкл
  outb(0x61, (uint8_t)tmp|1);   // вкл
  // Сбросим счетчик таймера APIC (установим в -1)
  lapic_addr[APIC_TMRINITCNT] = 0xFFFFFFFF;
 
  // Ждем обнуления счетчика PIT
  while(!(inb(0x61)&0x20));
 
  // Остановим таймер APIC
  lapic_addr[APIC_LVT_TMR] = APIC_DISABLE;
 
  // Посчитаем...
  cpubusfreq = ((0xFFFFFFFF - lapic_addr[APIC_TMRINITCNT]) + 1)*16*19;//*100;
  printf("CPU bus freq: %d\n", cpubusfreq);
  BREAK();
  tmp = cpubusfreq / quantum / 16;
 
  // Теперь в tmp - нужное нам значение
  lapic_addr[APIC_TMRINITCNT] = (tmp < 16 ? 16 : tmp);
  // Окончательно включаем таймер в периодическом режиме
  lapic_addr[APIC_LVT_TMR] = 32|TMR_PERIODIC;
  // Повторная установка делителя необходима для некоторого
  // кривого железа, по мануалам она не нужна
  lapic_addr[APIC_TMRDIV] = 0x03;
  intr_enable();
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
  mount_page_hw(ioapic_phys, ioapic_addr);
  // FIXME!
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

