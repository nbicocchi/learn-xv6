#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[])
{
    int i, fd;

    if (argc < 2) {
	printf(2, "Usage: touch files...\n");
	exit();
    }

    for (i = 1; i < argc; i++) {
	fd = open(argv[i], O_CREATE | O_WRONLY);
	if (fd < 0) {
	    printf(2, "touch: %s failed to create\n", argv[i]);
	    break;
	}
	close(fd);
    }

    exit();
}
