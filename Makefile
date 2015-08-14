TARGET:=boot.elf
# !!! mcmodel=kernel КРИТИЧЕСКИ ВАЖНО !!!
CPPFLAGS:=-m64 -I. -I./mm -I./x86_64 -I./kernel -I./fs/ata -I./fs/mdpart -I./fs/vfs -I./fs/initrd -I./fs/devfs -I./fs/ext2 -I./fs/fat32
CFLAGS:=${CPPFLAGS} -g -ffreestanding -nostdlib -nodefaultlibs -Wall -mcmodel=kernel -mno-red-zone -Wconversion
ASFLAGS:=${CPPFLAGS} -Wa,--64 -Wa,-g
#ASFLAGS:= --64 -g
LDFLAGS:=-z max-page-size=0x1000 -m elf_x86_64
OBJECTS:= \
	x86_64/boot.o \
	kernel/ktty.o \
	kernel/kernel.o \
	kernel/klibc.o \
	x86_64/cpuid.o \
	x86_64/ioport.o \
	x86_64/intr.o \
	x86_64/gdt.o \
	kernel/task.o \
	x86_64/smp.o \
	mm/page.o \
	mm/phys.o \
	mm/mem.o \
	x86_64/mutex.o \
	x86_64/timer.o \
	x86_64/apic.o \
	x86_64/regs.o \
  x86_64/ap_init.o \
	kernel/syscall.o \
	x86_64/ata_pio.o \
	fs/vfs/vfs.o \
	fs/initrd/initrd.o \
	fs/devfs/devfs.o \
	fs/mdpart/mdpart.o \
	fs/ext2/ext2.o \
	fs/fat32/fat32.o \
	fs/ata/ata.o \
	fs/test.o

MISC := \
  kernel.lds

PREFIX:=x86_64-pc-linux-gnu-
VERSION:=-4.8.4
CC:=$(PREFIX)gcc$(VERSION)
LD:=$(PREFIX)ld
RANLIB:=$(PREFIX)ranlib
NM:=$(PREFIX)nm
MBR:=/usr/lib/syslinux/mbr.bin

$(TARGET): $(OBJECTS) $(MISC)
	$(LD) -Tkernel.lds -o $@ $(OBJECTS) $(LDFLAGS)
	$(NM) $@ | cut -d' ' -f1,3 | sed -e 's/^\(ffffffff\|00000000\)//g' \
	  > ldsym
	$(NM) $@ | cut -d' ' -f1,3 >> ldsym

clean:
	-rm ${OBJECTS} *~ *.bin *.elf

disk.img:
	@echo "MBR..."
	@echo "dd..."
	@dd if=/dev/zero of=$@ bs=512 count=`echo 63*16*203 | bc` \
		2> /dev/null
	@echo "Fdisk..."
	@echo -e "o\nn\np\n1\n\n\nt\n83\na\n1\nw\n" \
		| fdisk -u -C203 -S63 -H16 $@ 2> /dev/null > /dev/null
	@echo "MKFS.ext2..."
	@sudo kpartx -a disk.img
	@sudo mkfs.ext2 /dev/mapper/loop0p1 > /dev/null
	@sudo kpartx -d disk.img
	@mkdir -p mnt
	@sudo mount -t ext2 -o loop,offset=`echo 512*2048 | bc` disk.img mnt
	@sudo mkdir -p mnt/boot
	@sudo cp misc/extlinux.conf misc/mboot.c32 mnt/boot
	@sudo extlinux -i mnt/boot
	@sudo umount mnt
	@rmdir mnt
	@dd if=disk.img of=trunk bs=440 skip=1 2> /dev/null
	@rm disk.img
	@cat ${MBR} trunk > disk.img
	@rm trunk


disk.img.grub2:
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

cp: $(TARGET)
	@mkdir -p mnt
	@sudo mount -o loop,offset=1048576 disk.img mnt
	@sudo cp boot.elf mnt
	@sync
	@sudo umount mnt
	@rmdir mnt

dist:
	@export CD="`echo ${PWD} | sed -e 's|.*/||'`"
	@cd ${PWD} && cd .. && tar -c ${CD} | bzip2 > "${CD}_`date +'%y%m%d_%H%M%S'`.tar.bz2"

gdb:
	@qemu-system-x86_64 -s -S -hda disk.img&
	@gdb boot.elf -ex "target remote localhost:1234" -ex "set arch i386" \
		-ex "b *0x100000"

vesa: vesa.S
	@gcc -c -o vesa.o vesa.S
	@ld --oformat binary -Ttext 0x0 vesa.o -o v
	@dd if=v of=vesa bs=1 skip=31744
	@rm v vesa.o

smp: smp.S
	@gcc -c -o smp.o smp.S
	@ld --oformat binary -Ttext 0x0 smp.o -o s
	@dd if=s of=smp bs=1 skip=31744
	@rm s smp.o

%.bin: %.o
	objcopy -O binary $^ $@
