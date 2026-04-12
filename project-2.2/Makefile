CC = gcc
CFLAGS = -Wall -Wextra -pthread

TARGET = rwspinlock

$(TARGET): rwspinlock.c
	$(CC) $(CFLAGS) -o $(TARGET) rwspinlock.c

clean:
	rm -f $(TARGET)
