OUTPUT_FORMAT(elf64-x86-64)
ENTRY(start)

SECTIONS
{

  . = 0x100000;
  .boot :
  {
     boot.o(.text)
  }  
  
  . += 0xFFFFFFFFC0000000;
  kernel_begin = .;
  .text :
  AT(ADDR(.text) - 0xFFFFFFFFC0000000)
  {
     *(EXCLUDE_FILE(*boot.o).text)
     *(.text)
  }

  .data ALIGN(4096) :
  AT(ADDR(.data) - 0xFFFFFFFFC0000000)
  {
    *(.data)
    *(.rodata*)
  }

  .bss ALIGN(4096) :
  AT(ADDR(.bss) - 0xFFFFFFFFC0000000)
  {
    *(COMMON*)
    *(.bss*)
  }
  kernel_end = .;
}
