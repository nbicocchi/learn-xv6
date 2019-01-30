#include "types.h"
#include "user.h"
#include "stat.h"
#include "fs.h"

int main(int argc, char *argv[])
{
    int fd1, fd2;
    int r1, r2;
    char c1, c2;
    int byte_counter = 0;
    int line_counter = 0;

    if (argc != 3) {
	printf(2, "usage: %s file1 file2\n", argv[0]);
	exit();
    }

    fd1 = open(argv[1], 0);
    if (fd1 < 0) {
	printf(2, "%s: open() error\n", argv[0]);
        exit();
    }
    fd2 = open(argv[2], 0);
    if (fd2 < 0) {
	printf(2, "%s: open() error\n", argv[0]);
        exit();
    }

    while ((r1 = read(fd1, &c1, 1)) > 0 && (r2 = read(fd2, &c2, 1)) > 0) {
        byte_counter++;
	if (c1 != c2) {		
            //different, compute byte and line and exit
	    printf(1, "difference @byte %d, @line %d\n", byte_counter, line_counter);
	    close(fd1);
	    close(fd2);
	    exit();
	} else if (c1 == '\n') {	
	    line_counter++;
	} 
    }

    if (r1 < 0 && r2 > 0) {	
	printf(1, " %s longer than %s [%d bytes ugual]\n",
	       argv[2], argv[1], byte_counter);
    } else if (r1 > 0 && r2 < 0) {	
	printf(1, " %s longer than %s [%d bytes ugual]\n",
	       argv[1], argv[2], byte_counter);
    } else {	
	printf(1, "%s equals %s\n", argv[1], argv[2]);
    }

    close(fd1);
    close(fd2);
    exit();
}
