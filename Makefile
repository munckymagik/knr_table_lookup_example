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
table.o: xalloc.h test.h
xalloc.o: xalloc.h
