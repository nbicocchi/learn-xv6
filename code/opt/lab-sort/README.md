## Progettazione di Sistemi Operativi - DIEF UNIMORE ##
## Implementing "sort" commands in xv6, project by Eleonora Macchioni ##

The project consist in the implementation of the "sort" commands in the XV6 operating system.
The commands are written using the C language, with the system calls and function available on the basic version of XV6.

The commands both work with multiple options and multiple files, both in number of rows, two or multiple files can be specified. It is also possible to not specify the file, thus work with the standard input.

The command --help show what actions are possible:
Usage: sort [OPTION]... [FILE]...
Sort the lines of the FILE passed as parameter or from standard input in lexicografical order
More than one FILE can be specified as input, and as options:
Possible arguments are:
	-c		print the first line out of order;
	-n		print in numerical order the strings;
	-u		print only the unique elements;
	-o		write to the specified file;
	-f		case insensitive;
	-r		reverse;

2 text files have been added in order to test the function, when removed, the MAKEFILE must be modified too.


