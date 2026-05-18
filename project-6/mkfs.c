#include <string.h>
#include "block.h"
#include "mkfs.h"

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
}
