#include "block.h"
#include "free.h"
#include "inode.h"

#define INODE_MAP_BLOCK 1

int ialloc(void)
{
    unsigned char block[BLOCK_SIZE];

    bread(INODE_MAP_BLOCK, block);

    int inode_num = find_free(block);

    if (inode_num == -1)
        return -1;

    set_free(block, inode_num, 1);
    bwrite(INODE_MAP_BLOCK, block);

    return inode_num;
}
