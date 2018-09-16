CFLAGS ?=
CFLAGS += -Wall -Wextra -g

.PHONY: test
test: table
	@echo
	@./table

.PHONY: clean
clean:
	rm -f table *.o

table: table.o xalloc.o
table.c: xalloc.h test.h
xalloc.c: xalloc.h
