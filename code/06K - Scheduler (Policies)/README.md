## Operating Systems Course - DIEF UNIMORE ##

# Scheduling policies in xv6

This patch is an enhancement of the "*06K - Scheduler (Policies)*" patch and it aims to present 6 different scheduling policies which can be used in xv6.
The 6 policies implemented: DEFAULT, PRIORITY, FCFS, CFS, SML, LOTTERY. 
In order to enable a specific policy, when you launch qemu you have to specify the command above, which will set a flag that will enable the scheduling policy specified in it.

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
* [LOTTERY](#lottery) - Lottery Scheduling algorithm

### DEFAULT - Round Robin

The default algorithm implemented in xv6 it's one of the simplest (with FCFS) and relies on the Round-Robin policy, basically it loops through all the process which are available to run (market with the ```RUNNABLE```) state and give access to CPU at each one of them one at a time.
To schedule processes fairly, a round-robin scheduler generally employs time-sharing, giving each job a time slot or quantum (its allowance of CPU time), and interrupting the job if it is not completed by then. 
The job is resumed next time a time slot is assigned to that process. 
If the process terminates or changes its state to waiting during its attributed time quantum, the scheduler selects the first process in the ready queue to execute. 
In the absence of time-sharing, or if the quanta were large relative to the sizes of the jobs, a process that produced large jobs would be favoured over other processes.
Round-robin scheduling is simple, easy to implement, and starvation-free.

### PRIORITY - Priority based

The priority scheduling algorithm (SML) represents a preemptive policy that executes processes based on their priority. The scheduling policy first select the runnable process with the lowest value of priority and executes it, after that it finds the one with the seconds lowest value of priority and excutes it and so on, until we have finished all the processes. This scheduling policy allows the user to mark some processes which we wants to be completed first in a simple but fast way.
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
This algorithm is very similar to PRIORITY, but in this case we have only three queues (low, mid, high) and the user must select for each process which queue the process belongs to (default priority = 10: mid)

The following system call will change the priority queue of the process with a specific pid process:

```
int setpriority(int pid, int priority)
```

In this case ```priority``` is a number between 1 and 20 which represents the new process priority.

### LOTTERY - Lottery Scheduler

#### Author: Lorenzo Del Rossi

The lottery scheduling represents a randomized resource allocation mechanism which avoids the problem of starvation.

#### How Lottery Scheduler works

Each process has a number of *lottery tickets* and then the scheduler draws a random ticket number and the process which has the winner ticket will run until the next extraction.

To check if a process wins the lottery we have to see if its number of tickets is higher than the number extracted (the number is randomly extracted in the range [0 , sum of all tickets)), if it is not we will sum its number of tickets to the number of tickets of the next process, check again etc. until the sum of the tickets becomes higher than the chosen number, then that process will be the lottery winner. For example:



![Lottery Example](\images\lotteryexample.png)



 ```
Number of tickets of Process 1 = 10
Number of tickets of Process 2 = 2
Number of tickets of Process 3 = 5
Number of tickets of Process 4 = 1
Number of tickets of Process 5 = 2
Total tickets = 10 + 2 + 5 + 1 + 2 = 20
Random number extracted from [0..19] = 15

Check Process 1: 10 > 15? No, so we check Process 2
Check Process 2: (10 + 2) > 15? No, so we check Process 3
Check Process 3: (10 + 2 + 5) > 15? Yes, then Process 3 wins the lottery!

 ```

For further informations about how the lottery scheduling works there is an useful [paper]( https://www.usenix.org/legacy/publications/library/proceedings/osdi/full_papers/waldspurger.pdf ) (from which the previous example has been taken).
The probability for a process to win the lottery grows with its number of tickets, and that number can change only via the ```ticket``` command.
Since the algorithm is full of random features the arrival time of each process is not predictable as it could be done for other scheduling algorithms.

The following system call will change the number of tickets given a process PID:

```
int settickets(int pid, int tickets)
```

In order to run the ```ticket``` command an example of command is:

```
ticket 2 12
```

In this example the number of tickets of the process P with PID(P) = 2 will be set to 12.

**ATTENTION** Choosing the number of tickets equal to zero would cause the process starvation, for this reason the number of tickets must be set greater than zero and less than the maximum number of tickets defined as ```MAX_TICKETS``` in the ```param.h``` file.

Currently the maximum number of tickets is set to 20 in order to take the total number of tickets low without losing the non-uniform ticket distribution, allowing the scheduler to be faster when finding the *lottery winner*.

The more the maximum number of tickets per process is set, the more the scheduler will have a random behavior when finding the process to run thanks to the higher variance of the tickets distribution, but the scheduling process will be slower.

#### Testing the lottery scheduler

To test the scheduler's behavior it has been written the ```test_lottery``` command, it is similar to the ```test_scheduler``` command , but adapted to the lottery-based scheduling.

In order to run properly the lottery scheduling test only 2 parameters (```nsubprocess``` and ```nsteps ```) are needed instead of the 3 parameters needed in ```test_scheduler``` (```nsubprocess```, ```nsteps``` and ```priority```) since it is preferable to set all the subprocesses with a different number of tickets, and that random setting is implicit in the ```allocproc``` function. So to run a test with 5 subprocesses composed by 400000 steps the command is:

```
test_lottery 5 400000
```

Furthermore it has been written the ```pstic``` command, that is similar to the ```ps``` command, but it shows the number of tickets of the processes instead of their priority.

In order to run the above mentioned test holding at the same time the shell the command is:
```
test_lottery 5 400000 &
```


