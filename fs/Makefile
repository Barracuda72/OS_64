OBJ= \
  vfs/vfs.o \
	initrd/initrd.o \
	test.o

CFLAGS_BASE=-I../kernel -I./vfs -I./initrd -I../mm 
CFLAGS=${CFLAGS_BASE} -nostdinc -ffreestanding

all: ${OBJ}
	gcc -o main ${OBJ}

test.o: test.c
	gcc ${CFLAGS_BASE} -c test.c -o test.o
