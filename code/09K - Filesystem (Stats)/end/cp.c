#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
    int src_fd, dst_fd;
    int r = 0, w = 0;
    char *src;
    char *dst;
    char *buffer[512];

    if (argc != 3) {
	printf(2, "Usage: cp src dst\n");
	exit();
    }

    src = argv[1];
    dst = argv[2];

    if ((src_fd = open(src, O_RDONLY)) < 0) {
	printf(2, "cp: cannot open source %s\n", src);
	exit();
    }

    if ((dst_fd = open(dst, O_CREATE | O_WRONLY)) < 0) {
	printf(2, "cp: cannot open destination %s\n", dst);
	exit();
    }

    while ((r = read(src_fd, buffer, sizeof(buffer))) > 0) {
	w = write(dst_fd, buffer, r);
	if (w != r || w < 0)
	    break;
    }

    if (r < 0 || w < 0) {
	printf(2, "cp: error copying %s to %s\n", src, dst);
    }

    close(src_fd);
    close(dst_fd);

    exit();
}
