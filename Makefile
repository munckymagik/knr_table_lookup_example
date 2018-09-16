CFLAGS ?=
CFLAGS += -Wall -Wextra

.PHONY: all
all: table

.PHONY: clean
clean:
	rm -f table
