## Operating Systems Course - DIEF UNIMORE ##

# Paging policies in xv6

This patch aims to present 3 different paging policies which can be used in xv6.
The 3 policies implemented: DEFAULT, FIFO, RANDOM.
In order to enable a specific policy, when you launch qemu you have to specify the command above, which will set a flag that will enable the paging policy specified in it.

```
$ make qemu-nox PAGINGPOLICY=POLICY
```

or

```
$ generate.sh -l patchname --flags PAGINGPOLICY=POLICY
```

If the flag isn't defined at launch, then DEFAULT (LRU) is used.

## Polices

* [DEFAULT](#default) - LRU (Least Recently Used)
* [FIFO](#fifo) - First In First Out algorithm
* [RANDOM](#random) - Random algorithm

### DEFAULT - LRU

This algorithm replaces the pages not recently used. Each page is characterized by two parameters witch are:
1)Reference bit = its value is 1 each time the page is referenced (read or written);
2)Dirty bit = its value is 1 when the page is written.

The page that has both the parameters equal to zero is replaced. If there are no pages with these features an error message is generated.


### FIFO - First In First Out

This algorithm replaces the page that has long been in memory. The operating system keeps a circular list of all the pages that are in memory, where the page at the top of it is the oldest.
When a page fault occurs, the top page is removed and the new page is inserted at the end of the list.


### RANDOM 

This algorithm replaces a random page among those occupied in the memory. Through a function that generates a random number between 0 and NBUF, the corresponding page is replaced.
This algorithm is not very efficient because it could be replaced a page that has been used or modified recently could be replaced and therefore could be used also in the near future.

##TESTING

To test the different paging policies there is a C program "paging_test" that writes and reads a file many times, stressing the system.
It is noted that the execution time of the process varies according to the paging policy used.

By running the "paging_test" program, it is possible to observe how the Random and FIFO algorithms lead to longer execution times than the LRU algorithm which also analyzes the reference bit and the dirty bit.
In fact in most cases the execution time of FIFO and LRU is 3/4 seconds longer than that obtained with LRU.
It does not always happen because paging test writes sequentially on the file and therefore LRU is not excellent because it replaces the page used less recently and not the page used recently which probably will no longer be accessed.

#### Authors: Claudia De Michele and Rosa d'Iorio

