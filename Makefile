# Farcasanu Bianca Ioana 313CAb 2023-2024

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# define targets
TARGETS = sfl

build: $(TARGETS)

sfl: sfl.c
	$(CC) $(CFLAGS) sfl.c -lm -g -o sfl

run_sfl: build
	./$(TARGETS)

clean:
	rm -f $(TARGETS)
