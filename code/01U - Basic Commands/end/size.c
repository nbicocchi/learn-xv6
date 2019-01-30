#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define SIZE 1024

int main(int argc, char *argv[])
{
    char buffer[SIZE];
    int r, fd, count = 0;

    if (argc != 2) {
	printf(2, "usage: %s filename\n", argv[0]);
	exit();
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
	printf(2, "%s: cannot open file %s\n", argv[0], argv[1]);
	exit();
    }

    while ((r = read(fd, buffer, SIZE)) > 0) {
	count += r;
    }
    close(fd);

    printf(1, "size=%d\n", count);
    exit();
}
