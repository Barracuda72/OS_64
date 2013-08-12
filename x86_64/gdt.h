#ifndef __GDT_H__
#define __GDT_H__

#include <stdint.h>

void GDT_init();
uint32_t GDT_smartput(uint64_t seg, uint64_t base, uint64_t limit, uint64_t flags);
uint32_t GDT_smartaput(uint64_t base, uint64_t limit, uint64_t flags);

// Флаги для GDT_smart*()
#define SEG_DPL0  0x00000000
#define SEG_DPL1  0x00002000
#define SEG_DPL2  0x00004000
#define SEG_DPL3  0x00006000

#define SEG_NOTSYS  0x00001000
#define SEG_PRESENT  0x00008000

#define SEG_CODE_READ  0x00000A00  // Этот код можно читать
#define SEG_CODE_EO  0x00000800  // А этот - только выполнять
#define SEG_64BIT_CODE  0x00200000  // 64-битный сегмент кода
#define SEG_TSS64  0x00000900  // 64-битный Task State Segment
#define SEG_TSS64_BUSY  0x00000B00  // Занятая 64-битная TSS

#define SEG_DATA_RO  0x00000000
#define SEG_DATA_RW  0x00000200

#define SEG_BIG    0x00800000
#define SEG_32BIT  0x00400000
#define SEG_RESERV  0x00100000
#define SEG_LDT    0x00000004
#define SEG_GDT    0x00000000
#define SEG_RPL0  0x00000000
#define SEG_RPL1  0x00000001
#define SEG_RPL2  0x00000002
#define SEG_RPL3  0x00000003

#define CALC_SELECTOR(x,y) (((x)<<3)|(y))

#endif //__GDT_H__
