## Operating Systems Course - DIEF UNIMORE ##

In this exercise we will see how implement the sempahore utility in xv6.
In order to make it work, we have to implement a new struct semaphore, some
system calls relative to this struct and three tests for see the behavior of the system.
In order to add a system call please see syscall.h, syscall.c, user.h, usys.s,
defs.h, sysproc.c, proc.c.
The actual implementations of the new system calls is placed in proc.c.

##NEW SYSTEM CALLS##

int sem_init(int sem, int value):
	This system call is needed to initialize the semaphore struct values.
	int sem: this parameter is the number of semaphore;
	int value: this parameter is the value of semaphore.
	The return value is zero if the initialization has been successful otherwise -1.

int sem_destroy(int sem):
	This system call is needed to destroy the struct semaphore.
	int sem: this parameter is the number of semaphore.
	The return value is zero.

int sem_wait(int sem, int count):
	This system call allows process/thread to take exclusive possession of the common
	resources through the use of the semaphore or wait for it to be freed from other
	processes.
	int sem: this parameter is the number of semaphore;
	int count: this parameter is a number with which the value of the semaphore is compared.
	The return value is zero.

int sem_signal(int sem, int count):
	This system call allows a process to release a resource that is being possessed
	exclusively though the use of the semaphore.
	int sem: this parameter is the number of semaphore;
	int count: this parameter is a number with which the value of the semaphore is compared.
	The return value is zero.

int clone(void (*func)(void *), void *arg, void *stack):
	This system call allows to clone a thread and execute it.
	void(*func)(void *):this parameter is a pointer to the function to be cloned.
	void *arg:this parameter is a pointer to the pointer of the cloned function.
	void *stack:this parameter is a pointer to the memory allocated for the cloned function.
	The return is the new pid of the cloned function.

int join(void **stack):
	this system call creates the children of a parent process and executes them.
	void **stack:this parameter is a pointer to the stack pointer that will be dedicated
	to the function's child.

Some changes have also been made to the original system calls: allocproc, exit, kill.

!!!!!!!!!!!!!!!!!!!!!!PAY ATTENTION!!!!!!!!!!!!!!!!!!!!!!

ALL CHANGES IN THE FILES : 
	-proc.c
	-sysproc.c
	-syscall.c
	-syscall.h
	-defs.h
	-user.h
	-usys.s
	-Makefile

ARE INDICATED WITH THE FOLLOWING COMMENT STRINGS:
	//changes begin
	//changes end
	//changes

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

To test the new features of xv6, three different tests were created:

	-thread_test
	-sem_test
	-prodcons_test

thread_test: 
	This test creates a series of threads with the system call clone and manages the parallelism
	of their operations on a common variable called "counter", allowing on it an exclusive excess 
	by each thread to avoid errors.

sem_test:
	This test creates a series of children(fork) of a parent process that operate in parallel on a
	file called "counter". The test manages them with the help of semaphore that allow exclusive access
	to this	resource by only one child at a time so as not to generate errors.

bank_test:
	This test simulates a series of users who have the same bank account and decide to make withdrawals(cons)
	and deposit(prod) simultaneously. The test will manage with the use of semaphore the possible
	collisions that would occur if there was no management mode of the problem of parallelism.
	With the addition of the '-guide' parameter to the launch of the command, a brief description of the test
	is obtained.




Author: Federico Campo

				--Good Work!!--
	
	





