CC = gcc
CFLAGS = -Wall -Wextra -pthread

TARGET = ticketlock

$(TARGET): ticketlock.c
	$(CC) $(CFLAGS) -o $(TARGET) ticketlock.c

clean:
	rm -f $(TARGET)
