TARGET=	mkrom.exe

ifndef P
P=.
endif

all:	$(TARGET)

KERNEL16=kernel.bin
#KERNEL16=kernel16.bin

fd.img:	loader.sec $(KERNEL16)
	cat $^ > $(P)/$@

fd32.img:	loader.sec kernel32.bin
	cat $^ > $(P)/$@

romext.bin:	loader.rom $(KERNEL16)
	cat $^ > $@

romext32.bin:	loader.rom kernel32.bin
	cat $^ > $@

loader.sec:
	$(MAKE) -C loader $@ P=`pwd`

loader.rom:
	$(MAKE) -C loader $@ P=`pwd`

romext.rom:	mkrom.exe romext.bin
	./mkrom.exe romext.bin $(P)/$@

romext32.rom:	mkrom.exe romext32.bin
	./mkrom.exe romext32.bin $(P)/$@

$(KERNEL16):
	$(MAKE) -C kernel16 $@ P=`pwd`

kernel32.bin:
	$(MAKE) -C kernel32 $@ P=`pwd`

mkrom.exe:
	$(MAKE) -C tools $@ P=`pwd`

clean:
	$(RM) $(TARGET) *.img *.sec *.bin *.rom
	$(MAKE) -C tools $@
	$(MAKE) -C loader $@
	$(MAKE) -C kernel16 $@
	$(MAKE) -C kernel32 $@

clobber: clean
	$(RM) *~
	$(MAKE) -C tools $@
	$(MAKE) -C loader $@
	$(MAKE) -C kernel16 $@
	$(MAKE) -C kernel32 $@
