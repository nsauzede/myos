TARGET=kernel16.bin kernel16.s

CC=gcc
BCC=bcc -ansi
AS86=as86

all:	$(TARGET)

%.bin:	%.s
	$(AS86) -b $@ -u -w- -g -O -j -0 $<

%.s:	%.c
	$(BCC) -S -o $@ -0 -C-c $<

%_.c:	%.c
	$(CC) -E -P -o $@ $<

clean:
	$(RM) $(TARGET)
