CC=gcc
AFLAGS=-f elf32
CFLAGS=-fno-stack-protector -m32 -c
install:
	nasm $(AFLAGS) kernel.asm -o kasm.o
	$(CC) kernel.c -o kc.o $(CFLAGS)
	ld -m elf_i386 -T link.ld -o kernel kasm.o kc.o

run:
	qemu-system-x86_64 -kernel kernel

clean:
	rm *.o kernel
