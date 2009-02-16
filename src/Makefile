TARGET=	mkrom kernel.bin

ifndef P
P=.
endif

all:	$(TARGET)

fd.img:	loader.sec kernel.bin
	cat $^ > $(P)/$@

fd32.img:	loader.sec kernel32.bin
	cat $^ > $(P)/$@

romext.bin:	loader.rom kernel.bin
	cat $^ > $@

romext32.bin:	loader.rom kernel32.bin
	cat $^ > $@

loader.sec:
	$(MAKE) -C loader $@ P=`pwd`

loader.rom:
	$(MAKE) -C loader $@ P=`pwd`

romext.rom:	mkrom romext.bin
	./mkrom romext.bin $(P)/$@

romext32.rom:	mkrom romext32.bin
	./mkrom romext32.bin $(P)/$@

kernel.bin:
	$(MAKE) -C kernel16 $@ P=`pwd`

kernel32.bin:
	$(MAKE) -C kernel32 $@ P=`pwd`

mkrom:
	$(MAKE) -C tools $@ P=`pwd`

clean:
	$(RM) $(TARGET) *.img *.sec *.bin *.rom
	$(MAKE) -C tools $@
	$(MAKE) -C loader $@
	$(MAKE) -C kernel16 $@
	$(MAKE) -C kernel32 $@
