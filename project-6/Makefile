CC = gcc
CFLAGS = -Wall -Wextra -Werror

LIB_OBJS = image.o block.o
LIB = libvvsfs.a
TARGET = testfs

.PHONY: all test clean pristine

all: $(TARGET)

$(LIB): $(LIB_OBJS)
	ar rcs $@ $^

$(TARGET): testfs.o $(LIB)
	$(CC) $(CFLAGS) -o $@ testfs.o -L. -lvvsfs

image.o: image.c image.h
	$(CC) $(CFLAGS) -c image.c

block.o: block.c block.h image.h
	$(CC) $(CFLAGS) -c block.c

testfs.o: testfs.c image.h block.h ctest.h
	$(CC) $(CFLAGS) -DCTEST_ENABLE -c testfs.c

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(LIB_OBJS) testfs.o

pristine: clean
	rm -f $(TARGET) $(LIB)
