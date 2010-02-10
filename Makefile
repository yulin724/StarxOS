DEBUGCON=$(shell tty)

all: build update

build: clean
	make -C src

clean:
	make -C src clean

floppy.img:
	tools/create_floopy_image.sh
	mv allinone.img floppy.img

update: build
	@sudo mount -o loop floppy.img /mnt/ -t msdos	
	@sudo cp src/starxos /mnt/ -v
	@sudo cp taiobf.lst /mnt/boot/grub/
	@sudo cp menu.lst /mnt/boot/grub/
	@sudo umount /mnt

kvm: update
	kvm -fda floppy.img -m 64 -boot a

qemu: update
	@stty -F $(DEBUGCON) --save > stty.save
	/usr/local/bin/qemu -fda floppy.img -m 64 -boot a -debugcon $(DEBUGCON)
	@stty -F $(DEBUGCON) `cat stty.save`
	@rm stty.save

bochs: update
	#/usr/bin/bochs
	/usr/local/bin/bochs -f bochsrc_term -q

peter-bochs: update
	java -jar tools/peter-bochs-debugger20100203.jar /usr/local/bin/bochs -f bochsrc_peter_bochs
