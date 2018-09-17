CFLAGS ?=
CFLAGS += -Wall -Wextra -g

.PHONY: test
test: table_tests
	@echo
	@./table_tests

.PHONY: clean
clean:
	rm -f table *.o

table_tests: table_tests.o table.o xalloc.o
table_tests.o: table.h test.h xalloc.h
table.o: table.h xalloc.h
xalloc.o: xalloc.h
