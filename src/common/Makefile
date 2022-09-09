
TARGET:=test_tasks

CFLAGS:=-Ipt -Ilibc -I.
CFLAGS+=-Wall -Werror
CFLAGS+=-g -O0

ifdef DBG
GDB:=gdb -q -nx --args
endif

all: $(TARGET)

test_tasks: test_tasks.o tasks.o libc.o
%.o: libc/%.c
	$(CC) -c -o $@ $(CFLAGS) $<

check: test_tasks
	$(GDB) ./test_tasks
check32 check64: check

clean:
	$(RM) $(TARGET) *.o

clobber: clean
