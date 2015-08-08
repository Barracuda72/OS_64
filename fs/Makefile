OBJ= \
  vfs/vfs.o \
	initrd/initrd.o \
	devfs/devfs.o \
	mdpart/mdpart.o \
	ext2/ext2.o \
	fat32/fat32.o \
	test.o \
	main.o

OPTIMIZE=-g -O0
CFLAGS_BASE=${OPTIMIZE} -I./mdpart -I./vfs -I./initrd -I./devfs -I./ext2 -I./fat32 -I../mm -I../kernel
CFLAGS=${CFLAGS_BASE} -nostdinc -ffreestanding -D__HOSTED__

all: ${OBJ}
	gcc -o main ${OBJ}

main.o: main.c
	gcc ${CFLAGS_BASE} -c main.c -o main.o

clean:
	-rm ${OBJ}
