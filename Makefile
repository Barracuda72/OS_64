TARGET:=boot.elf
# !!! mcmodel=kernel КРИТИЧЕСКИ ВАЖНО !!!
CFLAGS:=-I. -g -ffreestanding -nostdlib -nodefaultlibs -Wall -mcmodel=kernel
CPPFLAGS:=-I.
OBJECTS:= ktty.o kernel.o klibc.o cpuid.o ioport.o intr.o gdt.o task.o smp.o page.o phys.o #start.o

PREFIX:=x86_64-linux-gnu
# AS:=$(PREFIX)-as -g
AS:=as -g
CC:=$(PREFIX)-gcc
# LD:=$(PREFIX)-ld
LD:=ld
RANLIB:=$(PREFIX)-ranlib

#$(TARGET): $(OBJECTS)
#	gcc ${CFLAGS} -c -o ktty.o ktty.c
#	gcc ${CFLAGS} -c -o kernel.o kernel.c
#	gcc ${CFLAGS} -c -o klibc.o klibc.c
#	gcc ${CFLAGS} -c -o cpuid.o cpuid.c
#	gcc ${CFLAGS} -c -o ioport.o ioport.c
#	gcc ${CFLAGS} -c -o intr.o intr.c
#	gcc ${CFLAGS} -c -o gdt.o gdt.c
#	gcc ${CFLAGS} -c -o task.o task.c
#	nasm -felf64 start.nasm -o start.o
#	$(LD) -melf_x86_64 --oformat binary -Ttext 0x20000 -o kernel.bin start.o kernel.o ktty.o klibc.o cpuid.o ioport.o intr.o gdt.o task.o -z max-page-size=0x1000
#	nasm -fbin boot.nasm -o $@

boot.elf: boot.o $(OBJECTS)
	$(LD) -Tkernel.lds -o $@ boot.o $(OBJECTS) -z max-page-size=0x1000 -m elf_x86_64

start.o: start.nasm
	nasm -felf64 start.nasm -o $@

clean:
	-rm *.o *~ *.bin *.elf

disk.img:
	@echo "BXImage..."
	@bximage -hd -mode=flat -size=100 -q $@
	@echo "FDisk..."
	@echo -e "o\nn\np\n1\n\n\nt\n83\na\n1\nw\n" | fdisk -u -C203 -S63 -H16 $@ 2> /dev/null > /dev/null
	@mkdir -p mnt
	@sudo losetup /dev/loop0 $@
	@echo "0 `ls -l $@ | cut -d' ' -f5 | awk '{print $$1/512}'` linear `ls -l /dev/loop0 | cut -d' ' -f5,6 |  sed 's/, /:/g'` 0" | sudo dmsetup create sdz
	@sudo partprobe
	@sudo mkfs.ext2 /dev/mapper/sdz1
	@sudo mount -t ext2 /dev/mapper/sdz1 mnt
	@sudo mkdir mnt/boot/grub -p
	@echo "(hd9) /dev/mapper/sdz\n(hd9,1) /dev/mapper/sdz1" | sudo tee mnt/boot/grub/device.map
	@sudo grub-install --force --root-directory=mnt "(hd9)"
	@echo "menuentry 'MyOS' {\nmultiboot /boot.elf\n}" | sudo tee mnt/boot/grub/grub.cfg
	@sudo umount mnt
	@sudo kpartx -d /dev/mapper/sdz
	@sudo dmsetup remove sdz
	@sudo losetup -d /dev/loop0
	@rmdir mnt
#	@rm disk.img

du:
	@sudo umount mnt
	@sudo kpartx -d /dev/mapper/sdz
	@sudo dmsetup remove sdz
	@sudo losetup -d /dev/loop0
	@sudo rmdir mnt

cp:
	@mkdir -p mnt
	@sudo mount -o loop,offset=1048576 disk.img mnt
	@sudo cp boot.elf mnt
	@sync
	@sudo umount mnt
	@rmdir mnt

boot.s: boot.S

boot.o: boot.s

dist:
	@export CD="`echo ${PWD} | sed -e 's|.*/||'`"
	@cd ${PWD} && cd .. && tar -c ${CD} | bzip2 > "${CD}_`date +'%y%m%d_%H%M%S'`.tar.bz2"

gdb:
	@bochs -q&
	@gdb boot.elf -ex "target remote localhost:1234"
