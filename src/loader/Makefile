TARGET=

NASM=nasm

ifndef P
P=.
endif

all:	$(TARGET)

%.sec:	%.asm
	$(NASM) -o $(P)/$@ -DSECTOR $<

%.rom:	%.asm
	$(NASM) -o $(P)/$@ -DROM $<

clean:
	$(RM) $(TARGET) *.sec

clobber:
	$(RM) *~
