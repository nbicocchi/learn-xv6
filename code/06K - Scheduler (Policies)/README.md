# Scheduling policies in xv6

This patch of xv6 aims to present 4 different scheduling policies which can be used in xv6.
The 5 policies implemented: DEFAULT, PRIORITY, FCFS, SML. 
In order to enable a specific policy, when you launch qemu you have to specify the command above, which will set a flag that wil enable the scheduling policity specified in it.

```
$ make qemu-nox SCHEDPOLICY=POLICY
```

or

```
$ generate.sh --lab labn --flags SCHEDPOLICY=POLICY
```

If the flag isn't defined at launch, then DEFAULT (Round-Robin policy) is used.

## Polices

* [DEFAULT](#default) - Round-Robin algorithm (stock xv6)
* [PRIORITY](#priority) - Priority based algorithm
* [FCFS](#fcfs) - First Come First Served algorithm
* [CFS](#cfs) - Completely Fair Scheduling algorithm 
* [SML](#sml) - Static Multi Level Queue algorithm

### DEFAULT - Round Robin

The default algorithm implemented in xv6 it's one of the simplest (with FCFS) and relies on the Round-Robin policy, basically it loops through all the process which are available to run (market with the ```RUNNABLE```) state and give access to
CPU at each one of them one at a time.
To schedule processes fairly, a round-robin scheduler generally employs time-sharing, giving each job a time slot or quantum (its allowance of CPU time), and interrupting the job if it is not completed by then. 
The job is resumed next time a time slot is assigned to that process. 
If the process terminates or changes its state to waiting during its attributed time quantum, the scheduler selects the first process in the ready queue to execute. 
In the absence of time-sharing, or if the quanta were large relative to the sizes of the jobs, a process that produced large jobs would be favoured over other processes.
Round-robin scheduling is simple, easy to implement, and starvation-free.

### PRIORITY - Priority based

The priority scheduling algorithm (SML) represents a preemptive policy that executes processes based on their priority.The scheduling policy first select the runnable process with the lowest value of priority and executes it, after that it finds the one with the seconds lowest value of priority and excutes it
and so on, until we have finished all the processes.This scheduling policy allows the user to mark some processes which we wants to be completed first in a simple but fast way.
Priority range in this algorithm is 1-20 (default is 10) where we give priority equals to 1 for the processes which we want to be completed first.

The following system call will change the priority queue of the process with a specific pid process:

```
int setpriority(int pid, int priority)
```

In this case ```priority``` is a number between 1 and 20 which represents the new process priority.

### FCFS - First Come First Served

First come first served (FCFS), is the simplest scheduling algorithm. FCFS simply queues processes in the order that they arrive in the ready queue. 
The scheduling overhead due to using this policy is minimal since context switches only occur upon process termination, and no reorganization of the process queue is required.
Throughput can be low, because long processes can be holding CPU, waiting the short processes for a long time, so short processes which are in a queue are penalized over the longer ones (known as convoy effect).
By using this policy we have no starvation, because each process gets chance to be executed after a definite time.
There isn't prioritization, so using this policy we cannot force certain processes to be completed first which means that this system has trouble meeting process deadlines. The lack of prioritization means that as long as every process eventually completes, there is no starvation. 
In an environment where some processes might not complete, there can be starvation since the processes that come next the one which might not complete are never executed.

### CFS - Completely Fair Scheduling

Always run the runnable process with the minimum running time. 
...

### SML - Static Multi Level Queue

The static multilevel queue scheduling (SML) represents a preemptive policy that includes a three priority queues (high queue: priority < 7, mid queue: 7 < priority < 14, low queue: priority > 14).
In this scheduling policy the scheduler will select a process from a lower queue only if no process is ready to run at a higher queue.
The algorithm first runs all the process with highest priority and then, when they finish, it will consider all the process with a lower priority.
Moving between priority queues is only available via a system call (i.e., setpriority()).
This algorithm is very similar to PRIORITY, but in this case we have only three queues (low, mid, high) and the user must select foreach process which queue the process belongs to (default priority = 10: mid)

The following system call will change the priority queue of the process with a specific pid process:

```
int setpriority(int pid, int priority)
```

In this case ```priority``` is a number between 1 and 20 which represents the new process priority.

