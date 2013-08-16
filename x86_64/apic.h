#ifndef __APIC_H__
#define __APIC_H__
#include <stdint.h>

void apic_init(uint32_t lapic_addr);
uint32_t apic_get_base();
void apic_set_base(uint32_t base);
uint8_t apic_present();
void apic_enable();
void apic_eoi();

void ioapic_init(uint32_t ioapic_addr);
uint32_t ioapic_read(uint8_t reg);
void ioapic_write(uint8_t reg, uint32_t val);

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
/*
Local APIC Register Addresses 
Offset  Register Name  Software Read/Write
0x0000h - 0x0010  reserved  -
0x0020h  Local APIC ID Register Read/Write
0x0030h  Local APIC ID Version Register Read only
0x0040h - 0x0070h  reserved  -
0x0080h  Task Priority Register Read/Write
0x0090h  Arbitration Priority Register Read only
0x00A0h  Processor Priority Register Read only
0x00B0h  EOI Register  Write only
0x00C0h  reserved  -
0x00D0h  Logical Destination Register Read/Write
0x00E0h  Destination Format Register Bits 0-27 Read only, Bits 28-31 Read/Write
0x00F0h  Spurious-Interrupt Vector Register Bits 0-3 Read only, Bits 4-9 Read/Write
0x0100h - 0x0170  ISR 0-255 Read only
0x0180h - 0x01F0h  TMR 0-255 Read only
0x0200h - 0x0270h  IRR 0-255 Read only
0x0280h  Error Status Register Read only
0x0290h - 0x02F0h  reserved  -
0x0300h  Interrupt Command Register 0-31 Read/Write
0x0310h  Interrupt Command Register 32-63 Read/Write
0x0320h  Local Vector Table (Timer) Read/Write
0x0330h  reserved  -
0x0340h  Performance Counter LVT Read/Write
0x0350h  Local Vector Table (LINT0) Read/Write
0x0360h  Local Vector Table (LINT1) Read/Write
0x0370h  Local Vector Table (Error) Read/Write
0x0380h  Initial Count Register for Timer Read/Write
0x0390h  Current Count Register for Timer Read only
0x03A0h - 0x03D0h  reserved  -
0x03E0h  Timer Divide Configuration Register Read/Write
0x03F0h  reserved  -
*/
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
#define IOAPIC_ENABLED 0x01
#endif // __APIC_H__
