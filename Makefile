TARGET:=fd16.img romext16.rom fd32.img romext32.rom fd64.img romext64.rom
#TARGET+=fd16c.img romext16c.rom

QEMU:=qemu-system-i386 -cpu 486
QEMU64:=qemu-system-x86_64
QEMUOPTS:=-m 3
QEMUOPTS+=-vga std
ifdef T
QEMUOPTS+=-S -s
endif
GDB:=gdb

all:	$(TARGET)

check: check16 check32 check64
	@true

check16:	fd16.img
#	$(QEMU) $(QEMUOPTS) -fda $< -boot a
	$(QEMU) $(QEMUOPTS) -drive format=raw,file=$<

check16r:	romext16.rom
	$(QEMU) $(QEMUOPTS) -option-rom $<

check16c:	fd16c.img
#	$(QEMU) $(QEMUOPTS) -fda $< -boot a
	$(QEMU) $(QEMUOPTS) -drive format=raw,file=$<

check16cr:	romext16c.rom
	$(QEMU) $(QEMUOPTS) -option-rom $<

check32:	fd32.img
#	$(QEMU) $(QEMUOPTS) -fda $< -boot a
	$(QEMU) $(QEMUOPTS) -drive format=raw,file=$<

check32r:	romext32.rom
	$(QEMU) $(QEMUOPTS) -option-rom $<

check64:	fd64.img
#	$(QEMU64) $(QEMUOPTS) -fda $< -boot a
	$(QEMU64) $(QEMUOPTS) -drive format=raw,file=$<

check64r:	romext64.rom
	$(QEMU64) $(QEMUOPTS) -option-rom $<

%_dbg:
#	$(MAKE) $* T=1 &
	(cd debug ; $(GDB) -ix gdbinit_real_mode.txt -ex 'set tdesc filename target.xml' -ex 'target remote :1234')

check64_dbg:
#	$(MAKE) check64 T=1 &
#	$(GDB) -ex 'set confirm off' -ex 'target remote :1234' -ex 'disp/i$$cs*16+(unsigned short)$$rip' -ex 'disp/8bx$$cs*16+(unsigned short)$$rip' -ex 'disp/x$$rax' -ex 'disp/x$$rbx' -ex 'disp/x$$cs' -ex 'disp/x$$rip' -ex 'file src/kernel64/kernel64.elf' -ex 'b kernel_main'
	$(GDB) -ex 'set confirm off' -ex 'target remote :1234' -ex 'disp/i$$cs*16+(unsigned short)$$rip' -ex 'disp/8bx$$cs*16+(unsigned short)$$rip' -ex 'disp/x$$rax' -ex 'disp/x$$rbx' -ex 'disp/x$$cs' -ex 'disp/x$$rip' -ex 'b *0x7c00'

db32:
#	$(MAKE) check32 T=1 &
	$(GDB) -ex 'set confirm off' -ex 'target remote :1234' -ex 'disp/i$$cs*16+(unsigned short)$$eip' -ex 'disp/8bx$$cs*16+(unsigned short)$$eip' -ex 'disp/x$$eax' -ex 'disp/x$$ebx' -ex 'disp/x$$cs' -ex 'disp/x$$eip' -ex 'file src/kernel32/kernel32.elf' -ex 'b kernel_main'

db64:
	$(MAKE) check64 T=1 &
	$(GDB) -ex 'set confirm off' -ex 'target remote :1234' -ex 'disp/i$$cs*16+(unsigned short)$$rip' -ex 'disp/8bx$$cs*16+(unsigned short)$$rip' -ex 'disp/x$$rax' -ex 'disp/x$$rbx' -ex 'disp/x$$cs' -ex 'disp/x$$rip' -ex 'file src/kernel64/kernel64.elf' -ex 'b kernel_main'

iso32:  fd32.img
	mkdir -p isodir/boot/grub
	cp -f src/kernel32/kernel32.elf isodir/boot/
	echo 'menuentry "kernel32 elf" {' > isodir/boot/grub/grub.cfg
	echo 'multiboot /boot/kernel32.elf' >> isodir/boot/grub/grub.cfg
	echo '}' >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o iso32.iso isodir
#	qemu-system-i386 -cdrom iso32.iso -S -s
	qemu-system-i386 -cdrom iso32.iso -s

%:
	$(MAKE) -C src $@ P=`pwd`

clean:
	@$(RM) $(TARGET)
	@$(MAKE) -s -C src $@

clobber: clean
	@$(RM) *~
	@$(RM) -Rf isodir iso32.iso
	@$(MAKE) -s -C src $@ P=`pwd`
