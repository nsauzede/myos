TARGET=kernel16.bin kernel16.s

CC=gcc
BCC=bcc -ansi
AS86=as86
CP:=cp -f

ifndef P
P:=.
endif

all:	$(TARGET)

kernel16.bin: kernel16.s
	$(AS86) -b $(P)/$@ -u -w- -g -O -j -0 kernel16.s

1kernel16.bin: vid.s libc.s ioport.s kernel16.s
	$(AS86) -b $(P)/$@ -u -w- -g -O -j -0 kernel16.s

0kernel16.o: kernel16.s
	$(AS86) -o $(P)/$@ -u -w- -g -O -j -0 kernel16.s

%.bin:	%.s
	$(AS86) -b $@ -u -w- -g -O -j -0 $<

0kernel16.s:	kernel16.c ioport.c
	$(BCC) -S -o $@ -0 -C-c $^

%.s:	%.c
	$(BCC) -S -o $@ -0 -C-c $<

%_.c:	%.c
	$(CC) -E -P -o $@ $<

clean:
	$(RM) $(TARGET) kernel.bin

clobber: clean
	$(RM) *.s *.o
