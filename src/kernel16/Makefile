TARGET=

NASM=nasm

ifndef P
P=.
endif

all:	$(TARGET)

%.bin:	%.asm
	$(NASM) -o $(P)/$@ $<

clean:
	$(RM) $(TARGET) *.bin

clobber: clean
	$(RM) *~
