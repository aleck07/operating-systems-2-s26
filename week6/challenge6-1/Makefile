CC = gcc
CFLAGS = -Wall -Wextra

SRCS = $(wildcard *.c)
BINS = $(SRCS:.c=)

main:
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) -c adder.c
	$(CC) $(CFLAGS) -o main main.o adder.o

.PHONY: clean

clean:
	rm -f *.o

pristine: clean
	rm -f main