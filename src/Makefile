CC=gcc

CFLAGS=-Wall -Wextra -Werror -nostdlib -nostartfiles -nodefaultlibs -I.
CFLAGS=-nostdlib -nostartfiles -nodefaultlibs -I.
LDFLAGS=-Tlinker.ld
ASFLAGS=-felf

BUILD_DIR=../output

SOURCES=boot.o kernel.o screen.o port.o stdio.o string.o gdt_idt.o gdt_idt_asm.o

all: $(SOURCES) link 

build:
	$(CC) -c -o kernel.o $(SOURCES) $(CFLAGS)

link:
	ld $(LDFLAGS) -o starxos $(SOURCES)

.s.o:
	-rm $*.o
	cpp -I. $*.s > $*.s.cpp
	as $*.s.cpp -o $*.o
	#rm boot.s.cpp

clean:
	-rm *.o
	-rm starxos
	-rm *.cpp
