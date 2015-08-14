OS_64
=====

An x86_64 OS written in C and Assembly.
Currently implemented features include:
* Full paging and physical memory managers
* Kernel heap and kmalloc/kfree working
* Multiple kernel threads, perhaps fork...
* Simple drivers for screen and kbd
* LAPIC and IO APIC working
* SMP init (AP init to 64-bit mode and halts)
* Interface for system calls
* Simple read-only ATA driver
* Inital implementation of VFS
* Inital implementation of EXT2 filesystem driver (currently read-only)
* Basic InitRD driver

On the way:
* VESA video mode
* True SMP support (multi-CPU system)
* Name for project =)
