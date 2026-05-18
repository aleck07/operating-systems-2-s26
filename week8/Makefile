CC = gcc
CFLAGS = -Wall -Wextra 

SRCS = $(filter-out pack.c, $(wildcard *.c))
BINS = $(SRCS:.c=)

all: $(BINS)

animals: animals.c pack.c
	$(CC) $(CFLAGS) -o animals animals.c pack.c

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(BINS)

.PHONY: all clean
