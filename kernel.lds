OUTPUT_FORMAT(elf64-x86-64)
ENTRY(start)

PHDRS
{
  boot PT_LOAD;
  ap_boot PT_LOAD;
  text PT_LOAD;
  data PT_LOAD;
}

SECTIONS
{
  /*
   * По адресу VMA = LMA = 0x100000 расположен загрузочный код
   */

  . = 0x100000;
  .boot :
  {
     *boot.o(.text)
  } : boot

  /*
   * По адресу VMA = 0x8000, LMA = ap_bootcode расположен код инициализации AP
   */

  ap_bootcode = .;
  .ap_boot 0x8000:
  AT(ap_bootcode)
  {
     *ap_init.o(.ap_text)
  } : ap_boot

  /*
   * Корректируем испорченный счетчик LMA
   */
  . = ap_bootcode + SIZEOF(.ap_boot);
  ap_bootcode_end = .;

  /*
   * Ядро расположено в последнем гигабайте памяти, для него
   * VMA = LMA + 0xFFFFFFFFC0000000
   */
  . += 0xFFFFFFFFC0000000;
  kernel_begin = .;
  .text ALIGN(4096) :
  AT(ADDR(.text) - 0xFFFFFFFFC0000000)
  {
     *(EXCLUDE_FILE(*boot.o).text)
     *(EXCLUDE_FILE(*ap_init.o).text)
     *(.text)
  } : text

  .data ALIGN(4096) :
  AT(ADDR(.data) - 0xFFFFFFFFC0000000)
  {
    *(.data)
    *(.rodata*)
  } : data

  .bss ALIGN(4096) :
  AT(ADDR(.bss) - 0xFFFFFFFFC0000000)
  {
    *(COMMON*)
    *(.bss*)
  } : data
  kernel_end = .;
}
