TARGET=fd.img fd32.img romext.rom romext32.rom

QEMU=qemu-system-x86_64
QEMU+=-m 3
QEMU+=-cpu 486 -vga std
ifdef T
QEMU+=-S -s
endif

all:	$(TARGET)

check:	fd.img
	$(QEMU) -fda $< -boot a

check32:	fd32.img
	$(QEMU) -fda $< -boot a

checkr:	romext.rom
	$(QEMU) -option-rom $< /dev/zero

check32r:	romext32.rom
	$(QEMU) -option-rom $< /dev/zero

%:
	$(MAKE) -C src $@ P=`pwd`

clean:
	$(RM) $(TARGET)
	$(MAKE) -C src $@

clobber: clean
