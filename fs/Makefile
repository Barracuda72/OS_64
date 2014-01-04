OBJ= \
  vfs/vfs.o \
	initrd/initrd.o \
	devfs/devfs.o \
	mdpart/mdpart.o \
	test.o \
	main.o

CFLAGS_BASE=-I./mdpart -I./vfs -I./initrd -I./devfs -I../mm -I../kernel
CFLAGS=${CFLAGS_BASE} -nostdinc -ffreestanding

all: ${OBJ}
	gcc -o main ${OBJ}

main.o: main.c
	gcc ${CFLAGS_BASE} -c main.c -o main.o

clean:
	-rm ${OBJ}
