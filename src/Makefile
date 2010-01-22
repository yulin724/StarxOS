CC=gcc

CFLAGS=-O0 -Wall -Wextra -Werror -nostdlib -nostartfiles -nodefaultlibs -I.
CFLAGS=-O0 -nostdlib -nostartfiles -nodefaultlibs -I. -fno-builtin
LDFLAGS=-Tlinker.ld
ASFLAGS=-felf

BUILD_DIR=../output

SOURCES=boot.o kernel.o screen.o port.o stdio.o string.o gdt_idt.o \
		gdt_idt_asm.o timer.o irq.o mem.o list.o emulator.o test.o


all: $(SOURCES) link

build:
	$(CC) -c -o kernel.o $(SOURCES) $(CFLAGS)

link:
	ld $(LDFLAGS) -o starxos $(SOURCES)
	nm starxos | awk '{print $$1,$$3}' > starxos.sym

.s.o:
	-rm $*.o
	cpp -I. $*.s > $*.s.cpp
	as $*.s.cpp -o $*.o
	#rm boot.s.cpp

clean:
	-rm *.o
	-rm starxos
	-rm *.cpp
