TARGET:=fd.img fd32.img romext.rom romext32.rom

QEMU:=qemu-system-i386
QEMUOPTS:=-m 3
QEMUOPTS+=-cpu 486 -vga std
ifdef T
QEMUOPTS+=-S -s
endif
GDB:=gdb

all:	$(TARGET)

check:	fd.img
	$(QEMU) $(QEMUOPTS) -fda $< -boot a

check32:	fd32.img
	$(QEMU) $(QEMUOPTS) -fda $< -boot a

checkr:	romext.rom
	$(QEMU) $(QEMUOPTS) -option-rom $<

check32r:	romext32.rom
	$(QEMU) $(QEMUOPTS) -option-rom $<

%_dbg:
	$(MAKE) $* QEMU=qemu-system-i386 T=1 &
	(cd debug ; $(GDB) -ix gdbinit_real_mode.txt -ex 'set tdesc filename target.xml' -ex 'target remote :1234')

%:
	$(MAKE) -C src $@ P=`pwd`

clean:
	@$(RM) $(TARGET)
	@$(MAKE) -s -C src $@

clobber: clean
	@$(RM) *~
	@$(MAKE) -s -C src $@ P=`pwd`
