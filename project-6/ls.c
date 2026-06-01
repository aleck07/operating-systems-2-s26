#include <stdio.h>
#include "dir.h"
#include "ls.h"

void ls(void)
{
    struct directory *dir = directory_open(ROOT_INODE_NUM);
    struct directory_entry ent;

    while (directory_get(dir, &ent) != -1)
        printf("%d %s\n", ent.inode_num, ent.name);

    directory_close(dir);
}
