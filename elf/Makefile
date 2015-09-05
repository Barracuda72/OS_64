CFLAGS=-Wall -Wextra -g -O0 -I../x86_64 -I../elf -I../libc -I../kernel -nostdinc -ffreestanding -nostdlib -nodefaultlibs -mcmodel=kernel -D__HOSTED__ -D__POOL_SIZE__=0x200000
LDFLAGS=-Ttext=0x800000 -Tdata=0x900000 -Tbss=0xA00000 --section-start=.interp=0xB00000
CC=gcc
LD=ld
OBJS_MAIN=elf.o main.o ../libc/crt0.o start.o ../libc/string.o ../libc/hosted.o ../libc/linsys.o
OBJS_TEST=test.o ../libc/crt0.o ../libc/linsys.o

elf: $(OBJS_MAIN)
	$(LD) -o $@ $(OBJS_MAIN) $(LDFLAGS)

clean:
	-rm $(OBJS_MAIN) $(OBJS_TEST) elf test.elf

test:	test.c
	gcc test.c ../libc/crt0.S ../libc/linsys.c -o test.elf -D__HOSTED__ -I../libc -static -nostdlib -nodefaultlibs -ffreestanding -D__POOL_SIZE__=0x1000

test64:	test.c
	gcc -ffreestanding -nostdinc -nostdlib -nodefaultlibs test.c -I../libc -I../sys ../libc/crt0.S ../libc/string.c ../libc/os64sys.S ../libc/hosted.c ../libc/kprintf.c -o test.elf -I../libc -I../kernel -static -nostdlib -nodefaultlibs -ffreestanding -D__POOL_SIZE__=0x1000
