#include <stdlib.h>
#include <string.h>
#include "block.h"
#include "dir.h"
#include "inode.h"
#include "pack.h"

#define MAX_PATH_LEN 256

static void path_dirname(char *path, char *parent)
{
    char *last_slash = strrchr(path, '/');
    if (last_slash == path) {
        parent[0] = '/';
        parent[1] = '\0';
    } else {
        int len = (int)(last_slash - path);
        strncpy(parent, path, (size_t)len);
        parent[len] = '\0';
    }
}

static void path_basename(char *path, char *name)
{
    char *last_slash = strrchr(path, '/');
    strncpy(name, last_slash + 1, DIR_NAME_LEN - 1);
    name[DIR_NAME_LEN - 1] = '\0';
}

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

struct inode *namei(char *path)
{
    if (strcmp(path, "/") == 0)
        return iget(ROOT_INODE_NUM);

    char path_copy[MAX_PATH_LEN];
    strncpy(path_copy, path, MAX_PATH_LEN - 1);
    path_copy[MAX_PATH_LEN - 1] = '\0';

    struct inode *cur = iget(ROOT_INODE_NUM);
    if (cur == NULL)
        return NULL;

    char *token = strtok(path_copy, "/");
    while (token != NULL) {
        struct directory dir;
        dir.inode  = cur;
        dir.offset = 0;

        struct directory_entry ent;
        int found          = 0;
        int next_inode_num = -1;

        while (directory_get(&dir, &ent) == 0) {
            if (strcmp(ent.name, token) == 0) {
                next_inode_num = (int)ent.inode_num;
                found          = 1;
                break;
            }
        }

        iput(cur);

        if (!found)
            return NULL;

        cur = iget(next_inode_num);
        if (cur == NULL)
            return NULL;

        token = strtok(NULL, "/");
    }

    return cur;
}

int directory_make(char *path)
{
    if (path[0] != '/')
        return -1;

    char parent_path[MAX_PATH_LEN];
    char new_name[DIR_NAME_LEN];

    path_dirname(path, parent_path);
    path_basename(path, new_name);

    struct inode *parent = namei(parent_path);
    if (parent == NULL)
        return -1;

    struct inode *new_dir = ialloc();
    if (new_dir == NULL) {
        iput(parent);
        return -1;
    }

    int data_block = alloc();
    if (data_block == -1) {
        iput(new_dir);
        iput(parent);
        return -1;
    }

    unsigned char dir_block[BLOCK_SIZE];
    memset(dir_block, 0, BLOCK_SIZE);

    write_u16(dir_block, new_dir->inode_num);
    strcpy((char *)(dir_block + 2), ".");

    write_u16(dir_block + DIR_ENTRY_SIZE, parent->inode_num);
    strcpy((char *)(dir_block + DIR_ENTRY_SIZE + 2), "..");

    new_dir->flags        = INODE_FLAG_DIRECTORY;
    new_dir->size         = 2 * DIR_ENTRY_SIZE;
    new_dir->block_ptr[0] = (unsigned short)data_block;

    bwrite(data_block, dir_block);

    int parent_block_index  = (int)(parent->size / BLOCK_SIZE);
    int parent_block_offset = (int)(parent->size % BLOCK_SIZE);

    unsigned char parent_block[BLOCK_SIZE];

    if (parent_block_offset == 0) {
        int new_block = alloc();
        if (new_block == -1) {
            iput(new_dir);
            iput(parent);
            return -1;
        }
        memset(parent_block, 0, BLOCK_SIZE);
        parent->block_ptr[parent_block_index] = (unsigned short)new_block;
    } else {
        bread(parent->block_ptr[parent_block_index], parent_block);
    }

    unsigned char *entry_ptr = parent_block + parent_block_offset;
    write_u16(entry_ptr, new_dir->inode_num);
    strncpy((char *)(entry_ptr + 2), new_name, DIR_NAME_LEN - 1);

    bwrite(parent->block_ptr[parent_block_index], parent_block);

    parent->size += DIR_ENTRY_SIZE;

    iput(new_dir);
    iput(parent);

    return 0;
}
