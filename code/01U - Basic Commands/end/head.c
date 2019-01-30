#include "types.h"
#include "stat.h"
#include "user.h"
#define MAX_BUF 512
#define STDIN "_stdin"

char buf[MAX_BUF];

/* Function: print_help
 * --------------
 * Function to print a series of guidelines to understand the command
*/
void print_help()
{
    printf(1,
	   "Usage: head [OPTION]... [FILE]...\nPrint the first 10 lines of each FILE to standard output.\nWith more than one FILE, precede each with a header giving the filename.\nPossible arguments are:\n\t-c NUM\t\tprint the first NUM bytes of each file;\n\t-n NUM\t\tprint the first NUM lines of each file;\n\t-q\t\tquiet, never print headers giving file names;\n\t-v\t\tverbose, print headers giving file names;\n");
    exit();
}

/* Function: print_version
 * --------------
 * Function to print the current version of the command
*/
void print_version()
{
    printf(1, "head for xv6 1.0\nWritten by Nicolo' Gasparini\n");
    exit();
}

/* Function: error_head
 * --------------
 * Error function to print a specified error
 * err: error code
 * s:   string to print
*/
void error_head(uint err, char *s)
{
    switch (err) {
    case (0):
	printf(2, "head: syntax error\n");
	break;
    case (1):
	printf(2,
	       "head: invalid option %s\nTry 'head --help' for more information\n",
	       s);
	break;
    case (2):
	printf(2, "head: syntax error, expected at least one argument\n");
	break;
    case (3):
	printf(2, "head: invalid number of lines: %s\n", s);
	break;
    case (4):
	printf(2, "head: invalid number of bytes: %s\n", s);
	break;
    case (5):
	printf(2, "head: read error\n");
	break;
    case (6):
	printf(2,
	       "head: cannot open '%s' for reading: No such file or directory\n",
	       s);
	break;
    default:
	printf(2, "head: error\n");
    }
    exit();
}

/* Function: head
 * --------------
 * Effective function of "head" command, opens the file, reads it and print the result according to modes specified.
 * fn:    filename
 * mode:  represent whether to print or not filenames, 0 is default, 1 is quiet, 2 is verbose
 * moden: represent if we are working with lines or bytes, 0 is default, 1 is lines, 2 is bytes
 * n:     number of lines or bytes to print
*/
void head(char *fn, int n, int mode, int moden)
{
    int i, r, fd, l = 0;

    if (strcmp(fn, STDIN) == 0) {
	fd = 0;
    } else {
	fd = open(fn, 0);
	if (fd < 0) {
	    error_head(6, fn);
	}
    }

    //print the filename when required
    if (mode == 2) {
	printf(1, "==> %s <==\n", fn);
    }
    //read a buffer of 512 character from file, then check if it is composed by 1 or more lines
    while ((r = read(fd, buf, sizeof(buf))) > 0 && l < n) {
	for (i = 0; i < r && l < n; i++) {
	    printf(1, "%c", buf[i]);
	    if (buf[i] == '\n' && moden != 2) {
		l++;
	    } else if (moden == 2) {
		l++;
	    }
	}
    }
    if (r < 0) {
	close(fd);
	error_head(5, 0);
    }
    close(fd);
}

/* Function: main
 * --------------
 * Main function for "head" command usage, see print_help for details.
 * mode:  represent whether to print or not filenames, 0 is default, 1 is quiet, 2 is verbose
 * moden: represent if we are working with lines or bytes, 0 is default, 1 is lines, 2 is bytes
 * n:     number of lines or bytes to print
*/
int main(int argc, char *argv[])
{
    int i, mode = 0, moden = 0, n = 10;
    char *c;

    if (argc <= 1) {
	head(STDIN, n, mode, moden);
	exit();
    }
    //start checking the arguments
    //skip the first since it is the name of the command
    for (i = 1; i < argc; i++) {
	strcpy(buf, argv[i]);
	if (buf[0] == '-') {	//option, not filename
	    switch (buf[1]) {
	    case 'n':
		if (atoi(argv[++i]) == 0) {
		    error_head(3, argv[i]);
		}
		n = atoi(argv[i]);
		moden = 1;
		break;
	    case 'c':
		if (atoi(argv[++i]) == 0) {
		    error_head(4, argv[i]);
		}
		n = atoi(argv[i]);
		moden = 2;
		break;
	    case 'v':
		mode = 2;
		break;
	    case 'q':
		mode = 1;
		break;
	    case '-':
		c = buf;
		c += 2;
		if (strcmp(c, "version") == 0) {
		    print_version();
		} else if (strcmp(c, "help") == 0) {
		    print_help();
		} else {
		    error_head(1, argv[i]);
		}
		break;
	    default:
		error_head(1, argv[i]);
		break;
	    }
	} else {
	    //filenames
	    if (i + 1 < argc && mode == 0) {
		//-q not specified and multiple files
		mode = 2;
	    }
	    head(argv[i], n, mode, moden);
	    if (i + 1 < argc) {
		printf(1, "\n");
	    }
	}
    }				//end cicle arguments
    exit();
}
