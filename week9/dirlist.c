#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

/*
 * Takes a dirname as a string ("." for current directory) and shows the
 * contents of that directory.
 */
void ls(const char *dirname)
{
    DIR *dir = opendir(dirname);
    struct dirent *entry;
    struct stat statbuf;
    while ((entry = readdir(dir)) != NULL) {
        char fullpath[4096];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", dirname, entry->d_name);
        stat(fullpath, &statbuf);
        printf("Name:       %s\n", entry->d_name);
        printf("Inode:      %lu\n", statbuf.st_ino);
        printf("Mode:       %o\n", statbuf.st_mode);
        printf("User ID:    %d\n", statbuf.st_uid);
        printf("Group ID:   %d\n", statbuf.st_gid);
        printf("Size:       %ld\n", statbuf.st_size);
        printf("Block size: %ld\n", statbuf.st_blksize);
        printf("Blocks:     %ld\n\n", statbuf.st_blocks);
    }
    closedir(dir);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: myls dirname\n");
        exit(1);
    }

    char *dirname = argv[1];
    
    ls(dirname);
}