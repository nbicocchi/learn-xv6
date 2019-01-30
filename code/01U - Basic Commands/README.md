## Operating Systems Course - DIEF UNIMORE ##

In this exercise, we will add some basic shell commands to xv6.
In particular basic versions of the commands cp, mv, touch, uptime have to 
be implemented.

They have to understand the syntax reported below and work on files only (no dirs). 
System calls to be used are reported in brackets. Please see user.h (on xv6) for 
a complete list of syscalls and library functions available.

To refresh how the commands work, you can test them on linux as they are available 
on most distributions.

* cp src dst (open, read, write)
* mv oldname newname (link, unlink)
* touch filename (open)
* uptime (uptime)

Good work!
