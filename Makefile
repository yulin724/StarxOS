all: build update

build:
	make -C src

floppy.img:
	tools/create_floopy_image.sh
	mv allinone.img floppy.img

update: build floppy.img
	sudo mount -o loop floppy.img /mnt/ -t msdos	
	sudo cp src/starxos /mnt/
	sudo cp taiobf.lst /mnt/boot/grub/
	sudo umount /mnt

kvm: update
	kvm -fda floppy.img -m 64
