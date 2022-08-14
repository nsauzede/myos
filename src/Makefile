TARGET=	mkrom.exe

ifndef P
P=.
endif

all:	$(TARGET)

KERNEL16=kernel.bin
#KERNEL16=kernel16.bin

loader.sec:
	$(MAKE) -C loader $@ P=`pwd`

loader.rom:
	$(MAKE) -C loader $@ P=`pwd`

fd.img:	loader.sec $(KERNEL16)
	cat $^ > $(P)/$@

romext.bin:	loader.rom $(KERNEL16)
	cat $^ > $@

romext.rom:	mkrom.exe romext.bin
	./mkrom.exe romext.bin $(P)/$@

fd32.img:	loader.sec kernel32.bin
	cat $^ > $(P)/$@

romext32.bin:	loader.rom kernel32.bin
	cat $^ > $@

romext32.rom:	mkrom.exe romext32.bin
	./mkrom.exe romext32.bin $(P)/$@

fd64.img:	loader.sec kernel64.bin
	cat $^ > $(P)/$@

romext64.bin:	loader.rom kernel64.bin
	cat $^ > $@

romext64.rom:	mkrom.exe romext64.bin
	./mkrom.exe romext64.bin $(P)/$@

$(KERNEL16):
	$(MAKE) -C kernel16 $@ P=`pwd`

kernel32.bin:
	$(MAKE) -C kernel32 $@ P=`pwd`

kernel64.bin:
	$(MAKE) -C kernel64 $@ P=`pwd`

mkrom.exe:
	$(MAKE) -C tools $@ P=`pwd`

clean:
	$(RM) $(TARGET) *.img *.sec *.bin *.rom
	$(MAKE) -C tools $@
	$(MAKE) -C loader $@
	$(MAKE) -C kernel16 $@
	$(MAKE) -C kernel32 $@
	$(MAKE) -C kernel64 $@

clobber: clean
	$(RM) *~
	$(MAKE) -C tools $@
	$(MAKE) -C loader $@
	$(MAKE) -C kernel16 $@
	$(MAKE) -C kernel32 $@
	$(MAKE) -C kernel64 $@
