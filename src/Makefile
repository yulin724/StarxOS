CC=gcc

CFLAGS=-Wall -Wextra -Werror -nostdlib -nostartfiles -nodefaultlibs -I.
CFLAGS=-nostdlib -nostartfiles -nodefaultlibs -I.
LDFLAGS=-Tlinker.ld
ASFLAGS=-felf

BUILD_DIR=../output

SOURCES=boot.o kernel.o screen.o

all: $(SOURCES) link 

build:
	$(CC) -c -o kernel.o $(SOURCES) $(CFLAGS)

link:
	ld $(LDFLAGS) -o starxos $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<

clean:
	rm *.o
	rm starxos
