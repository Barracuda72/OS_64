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
	@echo -e "o\nn\np\n1\n\n\nt\n83\nw\n" | fdisk -u -C203 -S63 -H16 $@ 2> /dev/null > /dev/null
	@mkdir mnt
	@sudo losetup /dev/loop0 $@
	echo "0 `ls -l $@ | cut -d' ' -f5 | awk '{print $$1/512}'` linear `ls -l /dev/loop0 | cut -d' ' -f5,6 |  sed 's/, /:/g'`"
	@sudo losetup -d /dev/loop0
	@sudo rmdir mnt
	@rm disk.img

floppy: floppy.img

floppy.img: $(TARGET)
#	@dd if=/dev/zero of=$@ bs=1024 count=1440
	@echo "BXImage..."
	@echo "yes\n" | bximage -hd -mode=flat -size=100 -q floppy.img > /dev/null
	@echo "Fdisk..."
	@echo -e "o\nn\np\n1\n\n\nt\n83\nw\n" | fdisk -u -C203 -S63 -H16 floppy.img 2> /dev/null > /dev/null
#	@echo -e "o\nn\np\n1\n\n\nt\nb\nw\n" | fdisk -u -C203 -S63 -H16 floppy.img 2> /dev/null > /dev/null
	@mkdir -p fdir
	@sudo losetup /dev/loop0 $@ -o0
	@sudo losetup /dev/loop1 $@ -o32256 # 32256 = 63*512
	@echo "Mkfs..."
#	@sudo mkfs.vfat /dev/loop1 > /dev/null
	@sudo mkfs.ext2 /dev/loop1 > /dev/null
	@sync
	@sudo mount /dev/loop1 fdir #-o gid=1000,uid=1000
	@sudo chown -R 1000:1000 fdir/
	@mkdir -p fdir/boot/grub
	@echo -e "(hd0) /dev/loop0\n(hd0,msdos1) /dev/loop1" > fdir/boot/grub/device.map
	@cp /boot/grub/{boot,core}.img /boot/grub/{fat,multiboot,ext2}.mod fdir/boot/grub
	@echo "GRUB2..."
	@sudo grub-install --root-directory=fdir/ --grub-probe=./grub-probe "(hd0)" #2> /dev/null > /dev/null
#	@sudo grub-setup -v -m fdir/boot/grub/device.map "(hd0)" -r "(hd0,msdos1)"
	@cp $(TARGET) fdir
#	@cp core.img fdir/boot/grub
	@echo -e "set root=(hd0,msdos1)\nset default=0\nset timeout=10\nmenuentry \"MyOS\" {\n multiboot /boot.elf\n }" > fdir/boot/grub/grub.cfg
	@sleep 0.1
	@sudo umount fdir
#	@echo "device (fd0) $@ \n root (fd0) \n setup (fd0) \n quit \n" | sudo ./grub/usr/sbin/grub --device-map=/dev/null --batch
	@sudo losetup -d /dev/loop1
	@sudo losetup -d /dev/loop0
	@rmdir fdir

up:
	@-sudo umount fdir
	@-sudo losetup -d /dev/loop1
	@-sudo losetup -d /dev/loop0
	@-rmdir fdir
	@-rm floppy.img

boot.s: boot.S

boot.o: boot.s

dist:
	@export CD="`echo ${PWD} | sed -e 's|.*/||'`"
	@cd ${PWD} && cd .. && tar -c ${CD} | bzip2 > "${CD}_`date +'%y%m%d_%H%M%S'`.tar.bz2"

gdb:
	@bochs -q&
	@gdb boot.elf -ex "target remote localhost:1234"
