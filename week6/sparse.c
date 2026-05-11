#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

//.rw-------@ 400M alec  6 May 11:10   foo.txt
//4.0k    foo.txt
// The size of the file after executing is 400M.
// But running `du -h foo.txt" it shows that it is only 4.0K (I'm assuming its 4096 bytes).
// The difference is because its a sparse file. Only one block is actually being used to store data, the other thousands of blocks aren't stored on disk.

int main(void)
{
    int fd = open("foo.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);
    lseek(fd, 400000000, SEEK_SET);
    write(fd, "F", 1);
    close(fd);

}