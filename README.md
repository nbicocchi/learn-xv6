# Operating Systems MSc (DIEF, UNIMORE)
This course deals with the design and implementation of operating systems. 
The main topics include: boot-up sequence, memory management, processes, file systems and inter process communication in modern operating systems. The course is based on the Xv6 operating system.

## Software
Xv6 is a teaching operating system developed in the summer of 2006 for MIT's operating systems course, 6.828: Operating System Engineering. We use it because of its beauty and simplicity. To successfully run it, you need to install some packages (tested on Ubuntu 18.04):

* $ sudo apt-get install git qemu build-essential 

For running xv6:

* $ cd xv6-public
* $ make
* $ make qemu-nox
* (for quitting the emulator) ctrl-a x

## Web
* https://pdos.csail.mit.edu/6.828/2019/xv6.html

## Books
* xv6 a simple, Unix-like teaching operating system; Russ Cox, Frans Kaashoek, Robert Morris
* Modern Operating Systems (4th Edition); Andrew S. Tanenbaum

## Video
* [YouTube](https://www.youtube.com/watch?v=k51934LHz3k&t=25s&index=1&list=PLEJxKK7AcSEGPOCFtQTJhOElU44J_JAun)

## Slides, code examples
Slides, books, and code examples can be found here and downloaded using git:

> $ git clone https://github.com/nbicocchi/operatingsystemsmsc.git

Both slides and code examples might be updated or bug-fixed during the course. At any time, for downloading the latest version, go the course material directory (on your pc) and use:

> $ git pull (in case of errors: git reset --hard; git pull)

## Modules
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

* Implementing scheduling policies (DEFAULT, PRIORITY, FCFS, SML, CFS, LOTTERY)
* Implementing a unique scheduler() function supporting different policies
* Implementing additional kernel utilities (statitics, random)
* Implementing an advanced version of ps showing more process information

[07K] Memory (Exec shebang)

* Implementing support for executing shell scripts with shebang
* Implementing shebang support with the exec() system call
* Implementing script support in sh

[08K] Memory (Lazy allocation)

* Implementing "lazy allocation" within the sbrk() system call
* Intercepting a PAGE FAULT trap for allocating missing memory pages

[09K] File System (Stats)

* Implementing a system call for acquiring fs-related statistics (superblock/bitmap)
* Implementing a user command for display statistics

[10K] File System (Single/Double/Triple indirection)

* Increasing the maximum file length using a modified single indirection
* Increasing the maximum file length using double indirection
* Increasing the maximum file length using triple indirection

## Labs
The course is organized around a number of patches for xv6. They are placed within the /code/ and /code/opt/ folders. Each patch has its own README.md for documentation and contains only the files that have been modified from xv6 stock. For running patches the script generate.sh can be used as described below.

```
usage: ./generate.sh -l lab [-s subdir] [-f flags] [-n] [-c]
This tool creates a copy of xv6-public/, patches it with a selected lab folder, and excutes the qemu emulator
-l The lab to be built and executed
-s The subdir (withitn each lab) to be built. Default=end
-f Flags to be passed to the compiler. Default=none
-n Skip the creation of a copy of xv6-public/ and patching. Useful for development. Default=false
-c Clean all patched copies of xv6
```

## Exam
For their project students can choose any topic about xv6 they like. As an inspiration, it is possible to implement an improved shell, missing user commands, or new/better functionalities within the kernel.

The project has to be delivered in the same format of labs (see code/). The command below must generate a modified (and working!) version of xv6.

> $ ./generate.sh -l your_project_dir 




