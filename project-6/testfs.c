#include <string.h>
#include <unistd.h>
#include "ctest.h"
#include "image.h"
#include "block.h"

#define TEST_IMAGE "test_image.img"

void test_image_open(void)
{
    int fd = image_open(TEST_IMAGE, 1);
    CTEST_ASSERT(fd >= 0, "image_open returns a valid file descriptor");
    image_close();
    unlink(TEST_IMAGE);
}

void test_image_close(void)
{
    image_open(TEST_IMAGE, 1);
    int result = image_close();
    CTEST_ASSERT(result == 0, "image_close returns 0 on success");
    unlink(TEST_IMAGE);
}

void test_bwrite_bread(void)
{
    unsigned char write_buf[BLOCK_SIZE];
    unsigned char read_buf[BLOCK_SIZE];

    memset(write_buf, 0xAB, BLOCK_SIZE);
    memset(read_buf, 0, BLOCK_SIZE);

    image_open(TEST_IMAGE, 1);
    bwrite(0, write_buf);
    bread(0, read_buf);
    image_close();
    unlink(TEST_IMAGE);

    CTEST_ASSERT(memcmp(write_buf, read_buf, BLOCK_SIZE) == 0,
                 "block read back matches what was written");
}

void test_multiple_blocks(void)
{
    unsigned char write_buf0[BLOCK_SIZE], write_buf7[BLOCK_SIZE];
    unsigned char read_buf0[BLOCK_SIZE], read_buf7[BLOCK_SIZE];

    memset(write_buf0, 0x11, BLOCK_SIZE);
    memset(write_buf7, 0x22, BLOCK_SIZE);

    image_open(TEST_IMAGE, 1);
    bwrite(0, write_buf0);
    bwrite(7, write_buf7);
    bread(0, read_buf0);
    bread(7, read_buf7);
    image_close();
    unlink(TEST_IMAGE);

    CTEST_ASSERT(memcmp(write_buf0, read_buf0, BLOCK_SIZE) == 0,
                 "block 0 unaffected by write to block 7");
    CTEST_ASSERT(memcmp(write_buf7, read_buf7, BLOCK_SIZE) == 0,
                 "block 7 data preserved correctly");
}

void test_truncate(void)
{
    unsigned char write_buf[BLOCK_SIZE];
    unsigned char read_buf[BLOCK_SIZE];
    unsigned char zero_buf[BLOCK_SIZE];

    memset(write_buf, 0xFF, BLOCK_SIZE);
    memset(zero_buf, 0, BLOCK_SIZE);

    image_open(TEST_IMAGE, 1);
    bwrite(0, write_buf);
    image_close();

    image_open(TEST_IMAGE, 1);
    memset(read_buf, 0xFF, BLOCK_SIZE);
    bread(0, read_buf);
    image_close();
    unlink(TEST_IMAGE);

    CTEST_ASSERT(memcmp(read_buf, zero_buf, BLOCK_SIZE) == 0,
                 "truncate zeroes existing data");
}

int main(void)
{
    CTEST_VERBOSE(1);

    test_image_open();
    test_image_close();
    test_bwrite_bread();
    test_multiple_blocks();
    test_truncate();

    CTEST_RESULTS();

    CTEST_EXIT();
}
