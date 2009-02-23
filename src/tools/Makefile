TARGET=mkrom sum

CFLAGS=-Wall -Werror
CFLAGS+=-g -O0 -fno-omit-frame-pointer

ifndef P
P=.
endif

all:	$(TARGET)

%:	%.c
	$(CC) -o $(P)/$@ $(CFLAGS) $^ $(LDFLAGS)

clean:
	$(RM) $(TARGET)
