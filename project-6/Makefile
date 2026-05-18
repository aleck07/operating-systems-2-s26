CC = gcc
CFLAGS = -Wall -Wextra -Werror

LIB_OBJS = image.o block.o free.o inode.o mkfs.o
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

block.o: block.c block.h free.h image.h
	$(CC) $(CFLAGS) -c block.c

free.o: free.c free.h block.h
	$(CC) $(CFLAGS) -c free.c

inode.o: inode.c inode.h block.h free.h
	$(CC) $(CFLAGS) -c inode.c

mkfs.o: mkfs.c mkfs.h block.h
	$(CC) $(CFLAGS) -c mkfs.c

testfs.o: testfs.c image.h block.h free.h inode.h mkfs.h ctest.h
	$(CC) $(CFLAGS) -DCTEST_ENABLE -c testfs.c

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(LIB_OBJS) testfs.o

pristine: clean
	rm -f $(TARGET) $(LIB)
