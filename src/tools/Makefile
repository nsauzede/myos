TARGET=mkrom.exe sum

CFLAGS=-Wall -Werror
CFLAGS+=-g -O0 -fno-omit-frame-pointer

ifndef P
P=.
endif

all:	$(TARGET)

%.exe:	%.c
	$(CC) -o $(P)/$@ $(CFLAGS) $^ $(LDFLAGS)

clean:
	$(RM) $(TARGET)

clobber:
	$(RM) *~
