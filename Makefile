TARGET:=boot.elf
# !!! mcmodel=kernel КРИТИЧЕСКИ ВАЖНО !!!
INCLUDES:=-I. -I./mm -I./x86_64 -I./kernel \
	-I./fs/ata -I./fs/mdpart -I./fs/vfs -I./fs/initrd \
	-I./fs/devfs -I./fs/ext2 -I./fs/fat32 -I./fs/tty \
	-I./libc -I./elf -I./sys -I./fs -I./libc/include
CPPFLAGS:=-m64 -nostdinc ${INCLUDES}
CFLAGS:=${CPPFLAGS} -g -ffreestanding -nostdlib -nodefaultlibs -Wall -fno-pie -mcmodel=kernel -mno-red-zone -Wconversion -std=gnu11 -D__IGNORE_UNIMPLEMENTED_STRING
ASFLAGS:=${CPPFLAGS} -Wa,--64 -Wa,-g
#ASFLAGS:= --64 -g
LDFLAGS:=-z max-page-size=0x1000 -m elf_x86_64
OBJ_PLATFORM:= \
	x86_64/boot.o \
	x86_64/cpuid.o \
	x86_64/ioport.o \
	x86_64/intr.o \
	x86_64/gdt.o \
	x86_64/smp.o \
	x86_64/spinlock.o \
	x86_64/timer.o \
	x86_64/apic.o \
	x86_64/regs.o \
  x86_64/ap_init.o \
	x86_64/ata_pio.o \
	x86_64/vesa/vesa.o
OBJ_FS:= \
  fs/fs.o \
	fs/vfs/vfs.o \
	fs/initrd/initrd.o \
	fs/devfs/devfs.o \
	fs/mdpart/mdpart.o \
	fs/ext2/ext2.o \
	fs/fat32/fat32.o \
	fs/ata/ata.o \
	fs/tty/tty.o \
	fs/test.o
OBJ_MM:= \
	mm/page.o \
	mm/phys.o \
	mm/mem.o
OBJ_LIBC:= \
	libc/kprintf.o \
	libc/string/memcpy.o \
	libc/string/memcmp.o \
	libc/string/strncpy.o \
	libc/string/memset.o \
	libc/string/strncat.o \
	libc/string/strncmp.o \
	libc/string/strlen.o
OBJ_KERNEL:= \
	kernel/ktty.o \
	kernel/kernel.o \
	kernel/tls.o \
	kernel/task.o
OBJ_SYS:= \
	sys/syscall.o \
  sys/sys_close.o \
  sys/sys_unlink.o \
  sys/sys_kill.o \
  sys/sys_isatty.o \
  sys/sys_fstat.o \
  sys/sys_write.o \
  sys/sys_open.o \
  sys/sys_lseek.o \
  sys/sys_read.o \
  sys/sys_times.o \
  sys/sys_getpid.o \
  sys/sys_exit.o \
  sys/sys_fork.o \
  sys/sys_stat.o \
  sys/sys_execve.o \
  sys/sys_sbrk.o \
  sys/sys_link.o \
  sys/sys_wait.o 
OBJ_MISC:= \
	elf/elf.o \
	elf/start.o
OBJECTS:= \
  $(OBJ_PLATFORM) \
	$(OBJ_FS) \
	$(OBJ_MM) \
	$(OBJ_LIBC) \
	$(OBJ_KERNEL) \
	$(OBJ_SYS) \
	$(OBJ_MISC)

MISC := \
  kernel.lds

PREFIX:=x86_64-pc-linux-gnu-
VERSION:=-14
CC:=$(PREFIX)gcc$(VERSION)
LD:=$(PREFIX)ld
RANLIB:=$(PREFIX)ranlib
NM:=$(PREFIX)nm
MBR:=/usr/share/syslinux/mbr.bin

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
		| /sbin/fdisk -u -C203 -S63 -H16 $@ 2> /dev/null > /dev/null
	@echo "MKFS.ext2..."
	@sudo kpartx -a disk.img
	@sleep 1
	@sudo mkfs.ext2 /dev/mapper/loop0p1 > /dev/null
	@sudo kpartx -d disk.img
	@mkdir -p mnt
	@sudo mount -t ext2 -o loop,offset=`echo 512*2048 | bc` disk.img mnt
	@sudo mkdir -p mnt/boot mnt/dev
	@sudo cp misc/extlinux.conf misc/mboot.c32 misc/libcom32.c32 mnt/boot
	@sudo extlinux -i mnt/boot
	@sudo umount mnt
	@rmdir mnt
	@dd if=disk.img of=trunk bs=440 skip=1 2> /dev/null
	@rm disk.img
	@cat ${MBR} trunk > disk.img
	@rm trunk


disk.img.grub2:
	@echo "BXImage..."
	@bximage -func=create -imgmode=flat -hd=100M -q $@
	@echo "FDisk..."
	@echo -e "o\nn\np\n1\n\n\nt\n83\na\n1\nw\n" | /sbin/fdisk -u -C203 -S63 -H16 $@ 2> /dev/null > /dev/null
	@sudo losetup /dev/loop0 $@
	@sudo losetup /dev/loop1 $@ -o 1048576
	@sudo mkfs.ext2 /dev/loop1
	@sudo mount -t ext2 /dev/loop1 /mnt
	@sudo mkdir /mnt/boot/grub -p
	@sudo grub-install --root-directory=/mnt --no-floppy --modules="normal part_msdos ext2 multiboot biosdisk" /dev/loop0
	@echo "menuentry 'MyOS' { multiboot /boot.elf }" | sudo tee /mnt/boot/grub/grub.cfg
	@sudo umount /mnt
	@sudo losetup -d /dev/loop1
	@sudo losetup -d /dev/loop0
#	@rm disk.img

du:
	@sudo umount /mnt
	@sudo kpartx -d /dev/mapper/sdz
	@sudo dmsetup remove sdz
	@sudo losetup -d /dev/loop0

cp: $(TARGET)
	@sudo mount -o loop,offset=1048576 disk.img /mnt
	@sudo cp boot.elf /mnt
	@sudo cp elf/test.elf /mnt
	@sync
	@sudo umount /mnt

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
