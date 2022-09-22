TARGET=	mkrom.exe

ifndef P
P=.
endif

all:	$(TARGET)

KERNEL=kernel16.bin
KERNEL16C=kernel16c.bin

loader.sec:
	$(MAKE) -C loader $@ P=`pwd`

loader.rom:
	$(MAKE) -C loader $@ P=`pwd`

fd16.img:	loader.sec $(KERNEL)
	cat $^ > $(P)/$@

romext16.bin:	loader.rom $(KERNEL)
	cat $^ > $@

romext16.rom:	mkrom.exe romext16.bin
	./mkrom.exe romext16.bin $(P)/$@

fd16c.img:	loader.sec $(KERNEL16C)
	cat $^ > $(P)/$@

romext16c.bin:	loader.rom $(KERNEL16C)
	cat $^ > $@

romext16c.rom:	mkrom.exe romext16c.bin
	./mkrom.exe romext16c.bin $(P)/$@

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

$(KERNEL):
	$(MAKE) -C kernel16 $@ P=`pwd`

$(KERNEL16C):
	$(MAKE) -C kernel16c $@ P=`pwd`

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
	$(MAKE) -C common $@
	$(MAKE) -C kernel16 $@
	$(MAKE) -C kernel16c $@
	$(MAKE) -C kernel32 $@
	$(MAKE) -C kernel64 $@

clobber: clean
	$(RM) *~
	$(MAKE) -C tools $@
	$(MAKE) -C loader $@
	$(MAKE) -C common $@
	$(MAKE) -C kernel16 $@
	$(MAKE) -C kernel16c $@
	$(MAKE) -C kernel32 $@
	$(MAKE) -C kernel64 $@
