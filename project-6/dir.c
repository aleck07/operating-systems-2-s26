#include <stdlib.h>
#include <string.h>
#include "block.h"
#include "dir.h"
#include "inode.h"
#include "pack.h"

struct directory *directory_open(int inode_num)
{
    struct inode *in = iget(inode_num);
    if (in == NULL)
        return NULL;

    struct directory *dir = malloc(sizeof(struct directory));
    dir->inode  = in;
    dir->offset = 0;

    return dir;
}

int directory_get(struct directory *dir, struct directory_entry *ent)
{
    if (dir->offset >= dir->inode->size)
        return -1;

    int block_index = dir->offset / BLOCK_SIZE;
    int block_num = dir->inode->block_ptr[block_index];

    unsigned char block[BLOCK_SIZE];
    bread(block_num, block);

    int block_offset = dir->offset % BLOCK_SIZE;

    ent->inode_num = read_u16(block + block_offset);
    strcpy(ent->name, (char *)(block + block_offset + 2));

    dir->offset += DIR_ENTRY_SIZE;

    return 0;
}

void directory_close(struct directory *d)
{
    iput(d->inode);
    free(d);
}
