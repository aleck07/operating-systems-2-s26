#ifndef DIR_H
#define DIR_H

#include "inode.h"

#define DIR_ENTRY_SIZE        32
#define DIR_NAME_LEN          16
#define DIR_ENTRIES_PER_BLOCK (BLOCK_SIZE / DIR_ENTRY_SIZE)
#define INODE_FLAG_DIRECTORY  2
#define INODE_FLAG_FILE       1
#define ROOT_INODE_NUM        0

struct directory {
    struct inode *inode;
    unsigned int  offset;
};

struct directory_entry {
    unsigned int inode_num;
    char         name[DIR_NAME_LEN];
};

struct directory *directory_open(int inode_num);
int               directory_get(struct directory *dir, struct directory_entry *ent);
void              directory_close(struct directory *d);

#endif
