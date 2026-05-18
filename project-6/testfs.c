#include <string.h>
#include <unistd.h>
#include "ctest.h"
#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"

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

void test_set_free(void)
{
    unsigned char block[BLOCK_SIZE];

    memset(block, 0, BLOCK_SIZE);

    set_free(block, 0, 1);
    CTEST_ASSERT((block[0] & 0x01) == 1, "set_free sets bit 0 in byte 0");

    set_free(block, 0, 0);
    CTEST_ASSERT((block[0] & 0x01) == 0, "set_free clears bit 0 in byte 0");

    set_free(block, 10, 1);
    CTEST_ASSERT((block[1] & 0x04) != 0, "set_free sets bit 10 (byte 1, bit 2)");

    set_free(block, 10, 0);
    CTEST_ASSERT((block[1] & 0x04) == 0, "set_free clears bit 10");
}

void test_find_free(void)
{
    unsigned char block[BLOCK_SIZE];

    memset(block, 0, BLOCK_SIZE);
    CTEST_ASSERT(find_free(block) == 0, "find_free returns 0 when all bits clear");

    memset(block, 0xFF, BLOCK_SIZE);
    block[0] = 0xFE;
    CTEST_ASSERT(find_free(block) == 0, "find_free finds bit 0 when only bit 0 is clear");

    memset(block, 0xFF, BLOCK_SIZE);
    block[1] = 0x7F;
    CTEST_ASSERT(find_free(block) == 15, "find_free finds bit 15 (byte 1, bit 7)");

    memset(block, 0xFF, BLOCK_SIZE);
    CTEST_ASSERT(find_free(block) == -1, "find_free returns -1 when all bits set");
}

void test_ialloc(void)
{
    image_open(TEST_IMAGE, 1);

    int first = ialloc();
    CTEST_ASSERT(first == 0, "first ialloc returns inode 0");

    int second = ialloc();
    CTEST_ASSERT(second == 1, "second ialloc returns inode 1");

    image_close();
    unlink(TEST_IMAGE);
}

void test_alloc(void)
{
    image_open(TEST_IMAGE, 1);

    int first = alloc();
    CTEST_ASSERT(first == 0, "first alloc returns block 0");

    int second = alloc();
    CTEST_ASSERT(second == 1, "second alloc returns block 1");

    image_close();
    unlink(TEST_IMAGE);
}

void test_mkfs(void)
{
    unsigned char block[BLOCK_SIZE];

    image_open(TEST_IMAGE, 1);
    mkfs();

    bread(2, block);
    CTEST_ASSERT(block[0] == 0x7f, "mkfs marks first 7 blocks as allocated in block map");
    CTEST_ASSERT(block[1] == 0x00, "mkfs leaves remaining block map entries free");

    bread(0, block);
    unsigned char zero_buf[BLOCK_SIZE];
    memset(zero_buf, 0, BLOCK_SIZE);
    CTEST_ASSERT(memcmp(block, zero_buf, BLOCK_SIZE) == 0, "mkfs writes zero superblock");

    image_close();
    unlink(TEST_IMAGE);
}

int main(void)
{
    CTEST_VERBOSE(1);

    test_image_open();
    test_image_close();
    test_bwrite_bread();
    test_multiple_blocks();
    test_truncate();
    test_set_free();
    test_find_free();
    test_ialloc();
    test_alloc();
    test_mkfs();

    CTEST_RESULTS();

    CTEST_EXIT();
}
