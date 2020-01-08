# MLFQ Scheduler
With this atch, the xv6 operating system will schedule processes with the Multi-Level Feedback Queue algorithm.
There are three queues, each associated with a priority value. Before start running for the first time, a process will be assigned to the highest priority queue and once dispatched the process can run for a established quantum of time. If it doesn't finish in the quantum assigned, the process will be downgraded to the queue below and another process from the highest-priority queue can start running.
The quantum assigned is inversely proportional to the priority queue.
There is a mechanism based on ready time which limits the starvation of low-priority processes.

Generally, processes with low computation-time will finish before the others.

# Launch it
$./generate.sh -l 00U - MLFQ
in the folder containig the "generate.sh" file (previous folder)

# Test
To test it, there are two ways:
- "test_scheduler" command (usage: Nprocesses Msteps priority)
- "test" command (usage: Nprocesses nsteps-Proc1 nsteps-Proc2 ... nsteps_ProcN)

It's possible to launch both commands concurrently and see processes behaviour with "ps" command.


# Credits
The code of "test_scheduler" was taken from the previous labs
