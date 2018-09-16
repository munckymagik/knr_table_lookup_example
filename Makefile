CFLAGS ?=
CFLAGS += -Wall -Wextra


.PHONY: all
all: table

.PHONY: clean
clean:
	rm -f table *.o

table: table.o xalloc.o
table.c: xalloc.h test.h
xalloc.c: xalloc.h
