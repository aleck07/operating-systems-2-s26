#include <string.h>
#include <unistd.h>
#include "ctest.h"
#include "dir.h"
#include "free.h"
#include "image.h"
#include "block.h"
#include "inode.h"
#include "ls.h"
#include "mkfs.h"
#include "pack.h"

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
    mkfs();

    struct inode *first = ialloc();
    CTEST_ASSERT(first != NULL, "first ialloc returns non-NULL");
    CTEST_ASSERT(first->inode_num == 1, "first ialloc returns inode 1");
    CTEST_ASSERT(first->size == 0, "first ialloc inode size is 0");

    struct inode *second = ialloc();
    CTEST_ASSERT(second != NULL, "second ialloc returns non-NULL");
    CTEST_ASSERT(second->inode_num == 2, "second ialloc returns inode 2");

    incore_free_all();
    image_close();
    unlink(TEST_IMAGE);
}

void test_incore_find_free(void)
{
    incore_free_all();

    struct inode *in = incore_find_free();
    CTEST_ASSERT(in != NULL, "incore_find_free returns non-NULL on empty incore");

    in->ref_count = 1;
    in->inode_num = 42;

    struct inode *in2 = incore_find_free();
    CTEST_ASSERT(in2 != NULL, "incore_find_free returns different slot after first is used");
    CTEST_ASSERT(in2 != in, "incore_find_free returns a different pointer");

    incore_free_all();
}

void test_incore_find(void)
{
    incore_free_all();

    struct inode *slot = incore_find_free();
    slot->ref_count = 1;
    slot->inode_num = 7;

    struct inode *found = incore_find(7);
    CTEST_ASSERT(found != NULL, "incore_find returns non-NULL for existing inode_num");
    CTEST_ASSERT(found == slot, "incore_find returns correct pointer");
    CTEST_ASSERT(found->inode_num == 7, "incore_find returns inode with correct inode_num");

    struct inode *not_found = incore_find(99);
    CTEST_ASSERT(not_found == NULL, "incore_find returns NULL for missing inode_num");

    incore_free_all();
}

void test_incore_free_all(void)
{
    incore_free_all();

    struct inode *a = incore_find_free();
    a->ref_count = 5;
    struct inode *b = incore_find_free();
    b->ref_count = 3;

    incore_free_all();

    CTEST_ASSERT(incore_find(a->inode_num) == NULL, "incore_free_all clears all ref counts");

    struct inode *fresh = incore_find_free();
    CTEST_ASSERT(fresh != NULL, "incore_find_free works after incore_free_all");
}

void test_read_write_inode(void)
{
    image_open(TEST_IMAGE, 1);
    mkfs();

    struct inode out;
    out.inode_num   = 5;
    out.size        = 1234;
    out.owner_id    = 99;
    out.permissions = 0x7;
    out.flags       = 0x2;
    out.link_count  = 3;
    for (int i = 0; i < INODE_PTR_COUNT; i++)
        out.block_ptr[i] = (unsigned short)(10 + i);

    write_inode(&out);

    struct inode in;
    memset(&in, 0, sizeof(in));
    in.inode_num = 5;
    read_inode(&in, 5);

    CTEST_ASSERT(in.size == 1234, "read_inode restores size");
    CTEST_ASSERT(in.owner_id == 99, "read_inode restores owner_id");
    CTEST_ASSERT(in.permissions == 0x7, "read_inode restores permissions");
    CTEST_ASSERT(in.flags == 0x2, "read_inode restores flags");
    CTEST_ASSERT(in.link_count == 3, "read_inode restores link_count");
    CTEST_ASSERT(in.block_ptr[0] == 10, "read_inode restores block_ptr[0]");
    CTEST_ASSERT(in.block_ptr[15] == 25, "read_inode restores block_ptr[15]");

    image_close();
    unlink(TEST_IMAGE);
}

void test_iget_iput(void)
{
    image_open(TEST_IMAGE, 1);
    mkfs();
    incore_free_all();

    struct inode *in = iget(0);
    CTEST_ASSERT(in != NULL, "iget returns non-NULL for inode 0");
    CTEST_ASSERT(in->ref_count == 1, "iget sets ref_count to 1 on first load");
    CTEST_ASSERT(in->inode_num == 0, "iget sets correct inode_num");

    struct inode *in2 = iget(0);
    CTEST_ASSERT(in2 == in, "second iget(0) returns same pointer");
    CTEST_ASSERT(in2->ref_count == 2, "second iget increments ref_count to 2");

    iput(in2);
    CTEST_ASSERT(in->ref_count == 1, "iput decrements ref_count to 1");

    in->size = 777;
    iput(in);
    CTEST_ASSERT(in->ref_count == 0, "final iput sets ref_count to 0");

    struct inode verify;
    memset(&verify, 0, sizeof(verify));
    read_inode(&verify, 0);
    CTEST_ASSERT(verify.size == 777, "iput writes inode to disk when ref_count hits 0");

    incore_free_all();
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
    CTEST_ASSERT(block[0] == 0xff, "mkfs marks first 8 blocks as allocated in block map");
    CTEST_ASSERT(block[1] == 0x00, "mkfs leaves remaining block map entries free");

    bread(0, block);
    unsigned char zero_buf[BLOCK_SIZE];
    memset(zero_buf, 0, BLOCK_SIZE);
    CTEST_ASSERT(memcmp(block, zero_buf, BLOCK_SIZE) == 0, "mkfs writes zero superblock");

    image_close();
    unlink(TEST_IMAGE);
}

void test_directory_open(void)
{
    image_open(TEST_IMAGE, 1);
    mkfs();
    incore_free_all();

    struct directory *dir = directory_open(ROOT_INODE_NUM);
    CTEST_ASSERT(dir != NULL, "directory_open returns non-NULL");
    CTEST_ASSERT(dir->offset == 0, "directory_open sets offset to 0");

    directory_close(dir);
    image_close();
    unlink(TEST_IMAGE);
}

void test_directory_get(void)
{
    image_open(TEST_IMAGE, 1);
    mkfs();
    incore_free_all();

    struct directory *dir = directory_open(ROOT_INODE_NUM);
    struct directory_entry ent;

    int r1 = directory_get(dir, &ent);
    CTEST_ASSERT(r1 == 0, "directory_get returns 0 on first entry");
    CTEST_ASSERT(ent.inode_num == 0, "first entry inode_num is 0");
    CTEST_ASSERT(strcmp(ent.name, ".") == 0, "first entry name is '.'");

    int r2 = directory_get(dir, &ent);
    CTEST_ASSERT(r2 == 0, "directory_get returns 0 on second entry");
    CTEST_ASSERT(ent.inode_num == 0, "second entry inode_num is 0");
    CTEST_ASSERT(strcmp(ent.name, "..") == 0, "second entry name is '..'");

    int r3 = directory_get(dir, &ent);
    CTEST_ASSERT(r3 == -1, "directory_get returns -1 past end");

    directory_close(dir);
    image_close();
    unlink(TEST_IMAGE);
}

void test_directory_close(void)
{
    image_open(TEST_IMAGE, 1);
    mkfs();
    incore_free_all();

    struct directory *dir = directory_open(ROOT_INODE_NUM);
    CTEST_ASSERT(dir != NULL, "directory_open returns non-NULL");

    directory_close(dir);

    CTEST_ASSERT(incore_find(ROOT_INODE_NUM) == NULL, "directory_close frees the inode");

    image_close();
    unlink(TEST_IMAGE);
}

void test_namei_root(void)
{
    image_open(TEST_IMAGE, 1);
    mkfs();
    incore_free_all();

    struct inode *in = namei("/");
    CTEST_ASSERT(in != NULL, "namei('/') returns non-NULL");
    CTEST_ASSERT(in->inode_num == ROOT_INODE_NUM, "namei('/') returns root inode");
    CTEST_ASSERT(in->ref_count >= 1, "namei('/') returns inode with ref_count >= 1");
    iput(in);

    image_close();
    unlink(TEST_IMAGE);
}

void test_namei_component(void)
{
    image_open(TEST_IMAGE, 1);
    mkfs();
    incore_free_all();

    directory_make("/foo");

    struct inode *in = namei("/foo");
    CTEST_ASSERT(in != NULL, "namei('/foo') returns non-NULL after directory_make");
    CTEST_ASSERT(in->flags == INODE_FLAG_DIRECTORY, "namei('/foo') inode is a directory");
    CTEST_ASSERT(in->size == 2 * DIR_ENTRY_SIZE, "namei('/foo') inode has correct size");
    iput(in);

    image_close();
    unlink(TEST_IMAGE);
}

void test_namei_invalid(void)
{
    image_open(TEST_IMAGE, 1);
    mkfs();
    incore_free_all();

    struct inode *in = namei("/nonexistent");
    CTEST_ASSERT(in == NULL, "namei returns NULL for nonexistent path");

    image_close();
    unlink(TEST_IMAGE);
}

void test_directory_make(void)
{
    image_open(TEST_IMAGE, 1);
    mkfs();
    incore_free_all();

    int result = directory_make("/foo");
    CTEST_ASSERT(result == 0, "directory_make returns 0 on success");

    struct directory *dir = directory_open(ROOT_INODE_NUM);
    struct directory_entry ent;

    directory_get(dir, &ent);
    directory_get(dir, &ent);

    int r = directory_get(dir, &ent);
    CTEST_ASSERT(r == 0, "root directory has a third entry after directory_make");
    CTEST_ASSERT(strcmp(ent.name, "foo") == 0, "third entry name is 'foo'");

    directory_close(dir);
    image_close();
    unlink(TEST_IMAGE);
}

void test_directory_make_entries(void)
{
    image_open(TEST_IMAGE, 1);
    mkfs();
    incore_free_all();

    directory_make("/foo");

    struct inode *foo_inode = namei("/foo");
    CTEST_ASSERT(foo_inode != NULL, "namei finds new directory");

    int foo_inum = (int)foo_inode->inode_num;
    iput(foo_inode);

    struct directory *dir = directory_open(foo_inum);
    struct directory_entry ent;

    int r1 = directory_get(dir, &ent);
    CTEST_ASSERT(r1 == 0, "foo has a first entry");
    CTEST_ASSERT(strcmp(ent.name, ".") == 0, "foo first entry is '.'");
    CTEST_ASSERT((int)ent.inode_num == foo_inum, "foo '.' points to itself");

    int r2 = directory_get(dir, &ent);
    CTEST_ASSERT(r2 == 0, "foo has a second entry");
    CTEST_ASSERT(strcmp(ent.name, "..") == 0, "foo second entry is '..'");
    CTEST_ASSERT(ent.inode_num == ROOT_INODE_NUM, "foo '..' points to root");

    int r3 = directory_get(dir, &ent);
    CTEST_ASSERT(r3 == -1, "foo has no more entries");

    directory_close(dir);
    image_close();
    unlink(TEST_IMAGE);
}

void test_directory_make_invalid(void)
{
    image_open(TEST_IMAGE, 1);
    mkfs();
    incore_free_all();

    int result = directory_make("no_leading_slash");
    CTEST_ASSERT(result == -1, "directory_make returns -1 for path without leading slash");

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
    test_incore_find_free();
    test_incore_find();
    test_incore_free_all();
    test_read_write_inode();
    test_iget_iput();
    test_directory_open();
    test_directory_get();
    test_directory_close();
    test_namei_root();
    test_namei_component();
    test_namei_invalid();
    test_directory_make();
    test_directory_make_entries();
    test_directory_make_invalid();

    CTEST_RESULTS();

    CTEST_EXIT();
}
