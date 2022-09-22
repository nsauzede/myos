TARGET=	kernel32.bin

ifndef P
P=.
endif
COMMON:=../common
LIBC:=../common/libc
PT:=../common/pt

CFLAGS=-Wall -Werror
CFLAGS+=-g -O0 -fno-omit-frame-pointer
CFLAGS+=-I$(COMMON) -I$(LIBC) -I$(PT)

NASM=nasm

KERNEL_TEXT=0x7e00
KLDFLAGS=-nostdlib -nostartfiles
#KLDFLAGS=-nostartfiles
KLDFLAGS+=-Ttext=$(KERNEL_TEXT)

KCFLAGS=-DKERNEL_ADDR=$(KERNEL_TEXT) -I . -I libc -I pt
KCFLAGS+=$(shell $(COMMON)/myos_tag.sh)

all:	$(TARGET)

%.o:    $(COMMON)/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.o:    %.asm
	$(NASM) -f elf -o $@ $<

boot.o: boot.S
	$(CC) -c -o $@ $(CFLAGS) $<

idt_wrappers.o: idt_wrappers.S
	$(CC) -c -o $@ $(CFLAGS) $< -DASSEMBLY

libc.o:	$(LIBC)/libc.c
	$(CC) -c -o $@ $(CFLAGS) $<

kernel32.elf:	CFLAGS+=-m32 -fno-stack-protector $(KCFLAGS)
kernel32.elf:	LDFLAGS+=-m32 $(KLDFLAGS)
kernel32.elf:	boot.o kernel.o vid.o _vid.o vid.o libc.o tasks.o idt.o idt_wrappers.o
	$(CC) -o $@ $^ -static $(LDFLAGS) -e kernel_main

kernel32.bin:	kernel32.elf
	objcopy --remove-section=.note.gnu.build-id -O binary $^ $(P)/$@

clean:
	$(RM) $(TARGET) *.o *.bin *.elf

clobber:
	$(RM) *~
	$(RM) libc/*~
