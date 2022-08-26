TARGET:=fd.img romext.rom fd32.img romext32.rom fd64.img romext64.rom

QEMU:=qemu-system-i386 -cpu 486
QEMU64:=qemu-system-x86_64
QEMUOPTS:=-m 3
QEMUOPTS+=-vga std
ifdef T
QEMUOPTS+=-S -s
endif
GDB:=gdb

all:	$(TARGET)

check:	fd.img
#	$(QEMU) $(QEMUOPTS) -fda $< -boot a
	$(QEMU) $(QEMUOPTS) -drive format=raw,file=$<

checkr:	romext.rom
	$(QEMU) $(QEMUOPTS) -option-rom $<

check32:	fd32.img
#	$(QEMU) $(QEMUOPTS) -fda $< -boot a
	$(QEMU) $(QEMUOPTS) -drive format=raw,file=$<

check32r:	romext32.rom
	$(QEMU) $(QEMUOPTS) -option-rom $<

check64:	fd64.img
	$(QEMU64) $(QEMUOPTS) -fda $< -boot a
#	$(QEMU64) $(QEMUOPTS) -drive format=raw,file=$<

check64r:	romext64.rom
	$(QEMU64) $(QEMUOPTS) -option-rom $<

%_dbg:
	$(MAKE) $* T=1 &
	(cd debug ; $(GDB) -ix gdbinit_real_mode.txt -ex 'set tdesc filename target.xml' -ex 'target remote :1234')

check64_dbg:
	$(MAKE) check64 T=1 &
	$(GDB) -ex 'set confirm off' -ex 'target remote :1234' -ex 'disp/i$$cs*16+(unsigned short)$$rip' -ex 'disp/8bx$$cs*16+(unsigned short)$$rip' -ex 'disp/x$$rax' -ex 'disp/x$$rbx' -ex 'disp/x$$cs' -ex 'disp/x$$rip' -ex 'file src/kernel64/kernel64.elf' -ex 'b kernel_main'

db32:
	$(MAKE) check32 T=1 &
	$(GDB) -ex 'set confirm off' -ex 'target remote :1234' -ex 'disp/i$$cs*16+(unsigned short)$$eip' -ex 'disp/8bx$$cs*16+(unsigned short)$$eip' -ex 'disp/x$$eax' -ex 'disp/x$$ebx' -ex 'disp/x$$cs' -ex 'disp/x$$eip' -ex 'file src/kernel32/kernel32.elf' -ex 'b kernel_main'

db64:
	$(MAKE) check64 T=1 &
	$(GDB) -ex 'set confirm off' -ex 'target remote :1234' -ex 'disp/i$$cs*16+(unsigned short)$$rip' -ex 'disp/8bx$$cs*16+(unsigned short)$$rip' -ex 'disp/x$$rax' -ex 'disp/x$$rbx' -ex 'disp/x$$cs' -ex 'disp/x$$rip' -ex 'file src/kernel64/kernel64.elf' -ex 'b kernel_main'

%:
	$(MAKE) -C src $@ P=`pwd`

clean:
	@$(RM) $(TARGET)
	@$(MAKE) -s -C src $@

clobber: clean
	@$(RM) *~
	@$(MAKE) -s -C src $@ P=`pwd`
