## Operating Systems Course - DIEF UNIMORE ##

In this exercise we will implement the id, date, and ps commands. 
All of them require a system call not implemented in stock xv6.

# id #
Command line tool printing its pid and its parent process pid.
While the system call getpid() is implemented, getppid() is missing.

# date #
Command line tool printing the current date. In order to obtain the value
of the hardware clock a system call date() have to be implemented.

# ps #
Command line tool printing the list of active processes. In order to make it work,
we have to implement a new system call getptable() for returning the list of
processes to a command running in user-space (ps). 

# Adding a system call #
In order to add a system call please see syscall.h, syscall.c, user.h, usys.s. 
While sys_getppid(), sys_date(), and sys_getptable() have to be placed in sysproc.c, 
the actual implementation of getptable() have to be placed in proc.c 
(the system call fork() is implemented in the same way).

Good work!
