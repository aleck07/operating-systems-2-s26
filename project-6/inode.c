#include <string.h>
#include "block.h"
#include "free.h"
#include "inode.h"
#include "pack.h"

#define INODE_MAP_BLOCK 1

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

struct inode *incore_find_free(void)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        if (incore[i].ref_count == 0)
            return &incore[i];
    }
    return NULL;
}

struct inode *incore_find(unsigned int inode_num)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++) {
        if (incore[i].ref_count != 0 && incore[i].inode_num == inode_num)
            return &incore[i];
    }
    return NULL;
}

void incore_free_all(void)
{
    for (int i = 0; i < MAX_SYS_OPEN_FILES; i++)
        incore[i].ref_count = 0;
}

void read_inode(struct inode *in, int inode_num)
{
    unsigned char block[BLOCK_SIZE];

    int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset_bytes = (inode_num % INODES_PER_BLOCK) * INODE_SIZE;

    bread(block_num, block);

    unsigned char *p = block + block_offset_bytes;

    in->size        = read_u32(p + 0);
    in->owner_id    = read_u16(p + 4);
    in->permissions = read_u8(p + 6);
    in->flags       = read_u8(p + 7);
    in->link_count  = read_u8(p + 8);

    for (int i = 0; i < INODE_PTR_COUNT; i++)
        in->block_ptr[i] = read_u16(p + 9 + 2 * i);
}

void write_inode(struct inode *in)
{
    unsigned char block[BLOCK_SIZE];

    int inode_num = in->inode_num;
    int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset_bytes = (inode_num % INODES_PER_BLOCK) * INODE_SIZE;

    bread(block_num, block);

    unsigned char *p = block + block_offset_bytes;

    write_u32(p + 0, in->size);
    write_u16(p + 4, in->owner_id);
    write_u8(p + 6,  in->permissions);
    write_u8(p + 7,  in->flags);
    write_u8(p + 8,  in->link_count);

    for (int i = 0; i < INODE_PTR_COUNT; i++)
        write_u16(p + 9 + 2 * i, in->block_ptr[i]);

    bwrite(block_num, block);
}

struct inode *iget(int inode_num)
{
    struct inode *in = incore_find(inode_num);

    if (in != NULL) {
        in->ref_count++;
        return in;
    }

    in = incore_find_free();

    if (in == NULL)
        return NULL;

    read_inode(in, inode_num);
    in->ref_count = 1;
    in->inode_num = inode_num;

    return in;
}

void iput(struct inode *in)
{
    if (in->ref_count == 0)
        return;

    in->ref_count--;

    if (in->ref_count == 0)
        write_inode(in);
}

struct inode *ialloc(void)
{
    unsigned char block[BLOCK_SIZE];

    bread(INODE_MAP_BLOCK, block);

    int inode_num = find_free(block);

    if (inode_num == -1)
        return NULL;

    struct inode *in = iget(inode_num);

    if (in == NULL)
        return NULL;

    set_free(block, inode_num, 1);
    bwrite(INODE_MAP_BLOCK, block);

    in->size        = 0;
    in->owner_id    = 0;
    in->permissions = 0;
    in->flags       = 0;
    in->link_count  = 0;
    in->inode_num   = inode_num;

    memset(in->block_ptr, 0, sizeof(in->block_ptr));

    write_inode(in);

    return in;
}
