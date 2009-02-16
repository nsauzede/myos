TARGET=

ifndef P
P=.
endif

NASM=nasm

KERNEL_TEXT=0x7e00
KLDFLAGS=-nostdlib -nostartfiles
#KLDFLAGS=-nostartfiles
KLDFLAGS+=-Ttext=$(KERNEL_TEXT)
KCFLAGS=-DKERNEL_ADDR=$(KERNEL_TEXT)

all:	$(TARGET)

%.o:    %.asm
	$(NASM) -f elf -o $@ $<

boot.o: boot.S
	$(CC) -c -o $@ $(CFLAGS) $<

kernel32.elf:	CFLAGS+=-m32 -fno-stack-protector $(KCFLAGS)
kernel32.elf:	LDFLAGS+=-m32 $(KLDFLAGS)
kernel32.elf:	boot.o kernel32.o vid.o _vid.o
	$(CC) -o $@ $^ -static $(LDFLAGS) -e kernel_main

kernel32.bin:	kernel32.elf
	objcopy -O binary $^ $(P)/$@

clean:
	$(RM) $(TARGET) *.o *.bin *.elf
