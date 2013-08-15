#ifndef __APIC_H__
#define __APIC_H__
#include <stdint.h>

void apic_init(uint32_t lapic_addr);
uint32_t apic_get_base();
void apic_set_base(uint32_t base);
uint8_t apic_present();
void apic_enable();


/*
 * Регистры APIC
 * >>2, поскольку используются как индексы в массиве lapic_addr
 */
enum
{
  APIC_APICID           = 0x020>>2,
  APIC_APICVER          = 0x030>>2,
  APIC_TASKPRIOR        = 0x080>>2,
  APIC_EOI              = 0x0B0>>2,
  APIC_LDR              = 0x0D0>>2,
  APIC_DFR              = 0x0E0>>2,
  APIC_SPURIOUS         = 0x0F0>>2,
  APIC_ESR              = 0x280>>2,
  APIC_ICRL             = 0x300>>2,
  APIC_ICRH             = 0x310>>2,
  APIC_LVT_TMR          = 0x320>>2,
  APIC_LVT_PERF         = 0x340>>2,
  APIC_LVT_LINT0        = 0x350>>2,
  APIC_LVT_LINT1        = 0x360>>2,
  APIC_LVT_ERR          = 0x370>>2,
  APIC_TMRINITCNT       = 0x380>>2,
  APIC_LAST             = 0x38F>>2,
  APIC_TMRCURRCNT       = 0x390>>2,
  APIC_TMRDIV           = 0x3E0>>2,
} APIC_REGS;

#define APIC_BASE_MSR 0x1B
#define APIC_BASE_MSR_BSP    0x100 // Процессор является BSP
#define APIC_BASE_MSR_ENABLE 0x800
#define APIC_DISABLE   0x10000
#define APIC_SW_ENABLE 0x100
#define APIC_CPUFOCUS  0x200
#define APIC_NMI       (4<<8)
#define TMR_PERIODIC   0x20000
#define TMR_BASEDIV    (1<<20)

#define APIC_LAPIC_ADDR 0xFFFFFFFFC00F0000
#define APIC_IOAPIC_ADDR 0xFFFFFFFFC00F1000

#define IOAPIC_REGSEL 0x00>>2
#define IOAPIC_REGWIN 0x10>>2
#endif // __APIC_H__
