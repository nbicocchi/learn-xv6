## Operating Systems Course - DIEF UNIMORE ##

In this exercise, we will add a priority to processes. 

Firsly, struct proc (proc.h) has to be modified to accomodate a 
priority field for processes. Please check the allocproc function (proc.c)
for initializing the added field. 

We also have to slightly modify the ps command and the system call getptable() 
to show the priority when ps is invoked.

We also implement a user space program (nice) to modify process priority
at runtime. The command nice has to be invoked as follows:

```
$ nice pid priority ([1, 20] = low number, high priority)
```

To actually change the priority associated with the process within the
process table, a new system call (i.e., setpriority(pid, priority)) has to be implemented.

Finally, we will modify the stock xv6 scheduler (implementing a 
round robin policy) to take advantage of priorities. The new scheduler (proc.c)
always executes the RUNNABLE process with the highest priority (i.e., low priority 
number). 

A user command named test_scheduler can also be implemented to observe the new scheduler 
working. In particular test_scheduler generates n children and waits for them.

```
$ test_scheduler n_children task_lenght priority
```

As an example, try as shown below to use the nice command to change the priority
of 2 of the 5 children processes. What happens?

Example:

```
$ test_scheduler 5 1000000 15 &
$ ps
$ nice child1 5
$ nice child2 5
```

Good work!
