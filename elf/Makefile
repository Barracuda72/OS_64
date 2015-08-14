CFLAGS=-Wall -Wextra -g -ffreestanding -nostdlib -nodefaultlibs -mcmodel=kernel -D__POOL_SIZE__=0x200000
LDFLAGS=-Ttext=0x800000 -Tdata=0x900000 -Tbss=0xA00000 --section-start=.interp=0xB00000
CC=gcc
LD=ld
OBJS=elf.o main.o crt0.o libc.o

elf: $(OBJS)
	$(LD) -o $@ $(OBJS) $(LDFLAGS)

clean:
	-rm $(OBJS) elf

test:	test.c
	gcc test.c libc.c crt0.S -o test.elf -static -nostdlib -nodefaultlibs -ffreestanding -D__POOL_SIZE__=0x1000
