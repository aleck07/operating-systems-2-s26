#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * Display a file on stdout.
 */
void cat(char *filename)
{
    char buf[4096];
    int count;

    int fd = open(filename, O_RDONLY);

    if (fd < 0) {
        perror("cat");
        return;
    }

    do {
        count = read(fd, buf, sizeof buf);
        write(1, buf, count);
    } while (count > 0);

    write(1, "\n", 1);

    close(fd);
}

/**
 * Main.
 */
int main(void)
{
    // Allowed functions: open, lseek, write, close

    int fd = open("seeker.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

    for (int i = 0; i < 60; i++) {
        write(fd, "-", 1);
    }

    lseek(fd, 0, SEEK_SET);
    write(fd, "interrupting", 12);
    cat("seeker.txt");

    lseek(fd, 40, SEEK_SET);
    write(fd, "cow", 3);
    cat ("seeker.txt");

    lseek(fd, 20, SEEK_SET);
    write(fd, "MOOOOOOO!", 9);
    cat("seeker.txt");

    close(fd);
}