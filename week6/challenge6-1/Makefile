CC = gcc
CFLAGS = -Wall -Wextra

main: main.o adder.o
	$(CC) -o $@ $^

main.o: main.c
	$(CC) $(CFLAGS) -c $<

adder.o: adder.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean pristine

clean:
	rm -f *.o

pristine: clean
	rm -f main
