#include <string.h>
#include <unistd.h>
#include "block.h"
#include "image.h"

unsigned char *bread(int block_num, unsigned char *block)
{
    (void)lseek(image_fd, (off_t)block_num * BLOCK_SIZE, SEEK_SET);
    ssize_t bytes_read = read(image_fd, block, BLOCK_SIZE);
    if (bytes_read < BLOCK_SIZE)
        memset(block + bytes_read, 0, BLOCK_SIZE - bytes_read);
    return block;
}

void bwrite(int block_num, unsigned char *block)
{
    (void)lseek(image_fd, (off_t)block_num * BLOCK_SIZE, SEEK_SET);
    (void)write(image_fd, block, BLOCK_SIZE);
}
