#include <stdint.h>
#include <apic.h>
#include <cpuid.h>
#include <ioport.h>
#include <intr.h>
#include <page.h>

uint32_t *lapic_addr = 0;
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
  mov (%rax), %rax \n \
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
 
  // Примонтируем страницу с регистрами APIC на адрес
  // 0xFFFFFFFFC00F0000. По этому адресу в пространство
  // ядра отображено ПЗУ BIOS. Зачем оно нам?
  lapic_addr = 0xFFFFFFFFC00F0000;
  mount_page(lapic_a, lapic_addr);
  //set up isrs
  ext_intr_install(0x20, apic_tmr_isr);
  ext_intr_install(0x27, apic_spur_isr);
 
  //initialize LAPIC to a well known state
  lapic_addr[APIC_DFR] = 0xFFFFFFFF;
  lapic_addr[APIC_LDR] = (lapic_addr[APIC_LDR]&0x00FFFFFF)|1;
  lapic_addr[APIC_LVT_TMR] = APIC_DISABLE;
  lapic_addr[APIC_LVT_PERF] = APIC_NMI;
  lapic_addr[APIC_LVT_LINT0] = APIC_DISABLE;
  lapic_addr[APIC_LVT_LINT1] = APIC_DISABLE;
  lapic_addr[APIC_TASKPRIOR] = 0;
 
  //okay, now we can enable APIC
  //global enable
  lapic_addr[APIC_SPURIOUS] = 39;
  apic_enable();
  //map APIC timer to an interrupt, and by that enable it in one-shot mode
  lapic_addr[APIC_LVT_TMR] = 32;
  //set up divide value to 16
  lapic_addr[APIC_TMRDIV] = 0x03;
 
  //initialize PIT Ch 2 in one-shot mode
  //waiting 1 sec could slow down boot time considerably,
  //so we'll wait 1/100 sec, and multiply the counted ticks
  outb(0x61, (inb(0x61)&0xFD)|1);
  outb(0x43, 0xB2);
  //1193180/100 Hz = 11931 = 2e9bh
  outb(0x42, 0x9B); //LSB
  inb(0x60); //short delay
  outb(0x42, 0x2E); //MSB
 
  //reset PIT one-shot counter (start counting)
  tmp = inb(0x61)&0xFE;
  outb(0x61, (uint8_t)tmp);		//gate low
  outb(0x61, (uint8_t)tmp|1);		//gate high
  //reset APIC timer (set counter to -1)
  lapic_addr[APIC_TMRINITCNT] = 0xFFFFFFFF;
 
  //now wait until PIT counter reaches zero
  while(!(inb(0x61)&0x20));
 
  //stop APIC timer
  lapic_addr[APIC_LVT_TMR] = APIC_DISABLE;
 
  //now do the math...
  cpubusfreq = ((0xFFFFFFFF - lapic_addr[APIC_TMRINITCNT]) + 1)*16*100;
  tmp = cpubusfreq / quantum / 16;
 
  //sanity check, now tmp holds appropriate number of ticks, use it as APIC timer counter initializer
  lapic_addr[APIC_TMRINITCNT] = (tmp < 16 ? 16 : tmp);
  //finally re-enable timer in periodic mode
  lapic_addr[APIC_LVT_TMR] = 32|TMR_PERIODIC;
  //setting divide value register again not needed by the manuals
  //although I have found buggy hardware that required it
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
   uint32_t eax = (apic & 0xfffff000) | APIC_BASE_MSR_ENABLE;
 
   CPU_write_MSR(APIC_BASE_MSR, eax, edx);
}
 
uint32_t apic_get_base()
{
   return CPU_read_MSR(APIC_BASE_MSR)&0xFFFFF000;
}
 
void apic_enable()
{
    apic_set_base(apic_get_base());
 
    /* Set the Spourious Interrupt Vector Register bit 8 to start receiving interrupts */
    lapic_addr[APIC_SPURIOUS] |= APIC_SW_ENABLE;
}
