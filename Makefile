all: build update

build:
	make -C src

update: build
	sudo mount -o loop floppy.img /mnt/ -t msdos	
	sudo cp src/starxos /mnt/
	sudo umount /mnt
