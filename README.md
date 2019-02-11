# Operating Systems MSc (DIEF, UNIMORE) #
This course deals with the design and implementation of operating systems. 
The topics include the bootup sequence, memory management, processes, interrupt handling, file systems, 
synchronization, inter process communication in modern operating systems. 
The course is built on the xv6 operating system.


## How to use it ##
The whole material, comprising slides and code examples, can be downloaded using git:

* git clone https://github.com/nbicocchi/operatingsystemsmsc.git

Both slides and code examples might be updated or bug-fixed during the course. At any time, for downloading the
latest version, go the course material directory (on your pc) and use:

* $ git pull


## xv6 ##
Xv6 is a teaching operating system developed in the summer of 2006 for MIT's operating systems course, 6.828: Operating System Engineering. 
We use it because of its beauty and simplicity. To successfully run it, you need to install some packages (tested on Ubuntu 16):

* $ sudo apt-get install git qemu build-essential 

Despite books and code have already been included into this repository, you can access the full xv6 documentation at the following url:

* https://pdos.csail.mit.edu/6.828/2017/xv6.html

For running xv6:

* $ cd xv6-public
* $ make
* $ make qemu-nox


## Modules ##
[01U] Basic commands

* xv6 functionalities, concepts, internal organization
* Building and running xv6
* Implementing user commands

[02U] Login

* Implementing the login command (asking for user and password)
* Boot sequence change, automatic launch of login at boot

[03U] Shell

* Porting lsh (Linux version) to xv6
* Implementing I/O redirection (lsh)
* Implementing command piping (lsh)
* Boot sequence change, automatic launch of lsh at boot

[04K] Adding a System Call

* Implementing the ps command
* Implementing related system calls (getptable(), getppid())

[05K] Scheduler (Process Priority)

* Adding a priority to processes
* Implementing the nice command for changing priorities
* Implementing related system calls (setpriority())
* Implementing a priority-based scheduler

[06K] Scheduler (Policies)

* Implementing scheduling policies (DEFAULT, PRIORITY, FCFS, SML)
* Implementing a unique scheduler() function supporting different policies
* Implementing additional kernel utilities (statitics, random)
* Implementing an advanced version of ps showing process statistics

[07K] Memory (Exec shebang)

* Implementing support for executing shell scripts with shebang
* Implementing shebang support with the exec() system call
* Implementing script support in sh

[08K] Memory (Lazy allocation)

* Implementing "lazy allocation" within the sbrk() system call
* Intercepting a PAGE FAULT trap for allocating missing memory pages

[09K] File System (Single indirect)

* Increasing the maximum file lenght using single indirection (70K -> 200K)
* Implementing a user command for testing maximum allowed size

[10K] File System (Double indirect)

* Increasing the maximum file lenght using double indirection (70K -> ...)

## Course Rules ##
For their project, students can choose any topic about xv6 they like.
As an inspiration it is possible to implement an improved shell, missing user commands, or new/better functionalities
within the kernel.

The project has to be delivered in the same format of labs (see code/). The command: 

$ ./generate.sh --lab your_project_dir 

must generate a modified (and working!) version of xv6. The discussion will start from there.


## Course Books ##
* xv6: a simple, Unix-like teaching operating system; Russ Cox, Frans Kaashoek, Robert Morris 

* Modern Operating Systems; Andrew S. Tanenbaum 
