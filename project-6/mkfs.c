#include <string.h>
#include "block.h"
#include "dir.h"
#include "inode.h"
#include "mkfs.h"
#include "pack.h"

#define BLOCK_MAP_BLOCK 2

void mkfs(void)
{
    unsigned char block[BLOCK_SIZE];

    memset(block, 0, BLOCK_SIZE);
    bwrite(0, block);
    bwrite(1, block);
    bwrite(3, block);
    bwrite(4, block);
    bwrite(5, block);
    bwrite(6, block);

    // 0x7f = 0111 1111: marks blocks 0-6 as allocated
    block[0] = 0x7f;
    bwrite(BLOCK_MAP_BLOCK, block);

    struct inode *root = ialloc();
    int data_block = alloc();

    root->flags        = INODE_FLAG_DIRECTORY;
    root->size         = 2 * DIR_ENTRY_SIZE;
    root->block_ptr[0] = (unsigned short)data_block;

    unsigned char dir_block[BLOCK_SIZE];
    memset(dir_block, 0, BLOCK_SIZE);

    write_u16(dir_block, root->inode_num);
    strcpy((char *)(dir_block + 2), ".");

    write_u16(dir_block + DIR_ENTRY_SIZE, root->inode_num);
    strcpy((char *)(dir_block + DIR_ENTRY_SIZE + 2), "..");

    bwrite(data_block, dir_block);
    iput(root);
}
