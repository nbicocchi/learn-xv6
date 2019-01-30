#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

struct {
	struct spinlock lock;
	struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

/* Linked List containing an array of RUNNABLE process
 * of a certain priority.
 */
struct priority_queue {
	struct priority_queue * next;
	int last;
	int pos;
	struct proc * procs[NPROC];
	int priority;
};

/* Contains: a priority_queue for each priority allowed,
 * a validity array used to know if a priority_queue isempty or not
 * and actual_queue that reference to the priority_queue which
 * contains the next proc to be executed.
 */
struct queue_container {
	char validity[MAX_PRIORITY];
	struct priority_queue queue[MAX_PRIORITY][1];
	struct priority_queue *actual_queue;
};

struct queue_container *active, *expired;
struct queue_container active_d, expired_d;

int nextpid = 1;

extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void pinit(void) {
	initlock(&ptable.lock, "ptable");
	// Here is to initialize things

	active = &active_d;
	initQueueContainer(active);
	expired = &expired_d;
	initQueueContainer(expired);
}

// Must be called with interrupts disabled
int cpuid() {
	return mycpu() - cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void) {
	int apicid, i;

	if (readeflags() & FL_IF)
		panic("mycpu called with interrupts enabled\n");

	apicid = lapicid();
	// APIC IDs are not guaranteed to be contiguous. Maybe we should have
	// a reverse map, or reserve a register to store &cpus[i].
	for (i = 0; i < ncpu; ++i) {
		if (cpus[i].apicid == apicid)
			return &cpus[i];
	}
	panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
	struct cpu *c;
	struct proc *p;
	pushcli();
	c = mycpu();
	p = c->proc;
	popcli();
	return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void) {
	struct proc *p;
	char *sp;

	acquire(&ptable.lock);

	for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
		if (p->state == UNUSED)
			goto found;

	release(&ptable.lock);
	return 0;

	found: p->state = EMBRYO;
	p->priority = 10;
	p->ncicle = 0;
	p->pid = nextpid++;

	release(&ptable.lock);

	// Allocate kernel stack.
	if ((p->kstack = kalloc()) == 0) {
		p->state = UNUSED;
		return 0;
	}
	sp = p->kstack + KSTACKSIZE;

	// Leave room for trap frame.
	sp -= sizeof *p->tf;
	p->tf = (struct trapframe*) sp;

	// Set up new context to start executing at forkret,
	// which returns to trapret.
	sp -= 4;
	*(uint*) sp = (uint) trapret;

	sp -= sizeof *p->context;
	p->context = (struct context*) sp;
	memset(p->context, 0, sizeof *p->context);
	p->context->eip = (uint) forkret;

	return p;
}

//PAGEBREAK: 32
// Set up first user process.
void userinit(void) {
	struct proc *p;
	extern char _binary_initcode_start[], _binary_initcode_size[];

	p = allocproc();

	initproc = p;
	if ((p->pgdir = setupkvm()) == 0)
		panic("userinit: out of memory?");
	inituvm(p->pgdir, _binary_initcode_start, (int) _binary_initcode_size);
	p->sz = PGSIZE;
	memset(p->tf, 0, sizeof(*p->tf));
	p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
	p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
	p->tf->es = p->tf->ds;
	p->tf->ss = p->tf->ds;
	p->tf->eflags = FL_IF;
	p->tf->esp = PGSIZE;
	p->tf->eip = 0;  // beginning of initcode.S

	safestrcpy(p->name, "initcode", sizeof(p->name));
	p->cwd = namei("/");

	// this assignment to p->state lets other cores
	// run this process. the acquire forces the above
	// writes to be visible, and the lock is also needed
	// because the assignment might not be atomic.
	acquire(&ptable.lock);

	p->state = RUNNABLE;
	// ***
	insertActive(p);

	release(&ptable.lock);

}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int growproc(int n) {
	uint sz;
	struct proc *curproc = myproc();

	sz = curproc->sz;
	if (n > 0) {
		if ((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
			return -1;
	} else if (n < 0) {
		if ((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
			return -1;
	}
	curproc->sz = sz;
	switchuvm(curproc);
	return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int fork(void) {
	int i, pid;
	struct proc *np;
	struct proc *curproc = myproc();

	// Allocate process.
	if ((np = allocproc()) == 0) {
		return -1;
	}

	// Copy process state from proc.
	if ((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0) {
		kfree(np->kstack);
		np->kstack = 0;
		np->state = UNUSED;
		return -1;
	}
	np->sz = curproc->sz;
	np->parent = curproc;
	*np->tf = *curproc->tf;

	// Clear %eax so that fork returns 0 in the child.
	np->tf->eax = 0;

	for (i = 0; i < NOFILE; i++)
		if (curproc->ofile[i])
			np->ofile[i] = filedup(curproc->ofile[i]);
	np->cwd = idup(curproc->cwd);

	safestrcpy(np->name, curproc->name, sizeof(curproc->name));

	pid = np->pid;

	acquire(&ptable.lock);

	np->state = RUNNABLE;
	// ***
	insertActive(np);

	release(&ptable.lock);

	return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void exit(void) {
	struct proc *curproc = myproc();
	struct proc *p;
	int fd;

	if (curproc == initproc)
		panic("init exiting");

	// Close all open files.
	for (fd = 0; fd < NOFILE; fd++) {
		if (curproc->ofile[fd]) {
			fileclose(curproc->ofile[fd]);
			curproc->ofile[fd] = 0;
		}
	}

	begin_op();
	iput(curproc->cwd);
	end_op();
	curproc->cwd = 0;

	acquire(&ptable.lock);

	// Parent might be sleeping in wait().
	wakeup1(curproc->parent);

	// Pass abandoned children to init.
	for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
		if (p->parent == curproc) {
			p->parent = initproc;
			if (p->state == ZOMBIE)
				wakeup1(initproc);
		}
	}

	// Jump into the scheduler, never to return.
	curproc->state = ZOMBIE;
	sched();
	panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int wait(void) {
	struct proc *p;
	int havekids, pid;
	struct proc *curproc = myproc();

	acquire(&ptable.lock);
	for (;;) {
		// Scan through table looking for exited children.
		havekids = 0;
		for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
			if (p->parent != curproc)
				continue;
			havekids = 1;
			if (p->state == ZOMBIE) {
				// Found one.
				pid = p->pid;
				kfree(p->kstack);
				p->kstack = 0;
				freevm(p->pgdir);
				p->pid = 0;
				p->parent = 0;
				p->name[0] = 0;
				p->killed = 0;
				p->state = UNUSED;
				release(&ptable.lock);
				return pid;
			}
		}

		// No point waiting if we don't have any children.
		if (!havekids || curproc->killed) {
			release(&ptable.lock);
			return -1;
		}

		// Wait for children to exit.  (See wakeup1 call in proc_exit.)
		sleep(curproc, &ptable.lock);  //DOC: wait-sleep
	}
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void sched(void) {
	int intena;
	struct proc *p = myproc();

	if (!holding(&ptable.lock))
		panic("sched ptable.lock");
	if (mycpu()->ncli != 1)
		panic("sched locks");
	if (p->state == RUNNING)
		panic("sched running");
	if (readeflags() & FL_IF)
		panic("sched interruptible");
	intena = mycpu()->intena;
	swtch(&p->context, mycpu()->scheduler);
	mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void yield(void) {
	acquire(&ptable.lock);  //DOC: yieldlock
	myproc()->state = RUNNABLE;
	// ***
	insertExpired(myproc());

	sched();
	release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void forkret(void) {
	static int first = 1;
	// Still holding ptable.lock from scheduler.
	release(&ptable.lock);

	if (first) {
		// Some initialization functions must be run in the context
		// of a regular process (e.g., they call sleep), and thus cannot
		// be run from main().
		first = 0;
		iinit(ROOTDEV);
		initlog(ROOTDEV);
	}

	// Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void sleep(void *chan, struct spinlock *lk) {
	struct proc *p = myproc();

	if (p == 0)
		panic("sleep");

	if (lk == 0)
		panic("sleep without lk");

	// Must acquire ptable.lock in order to
	// change p->state and then call sched.
	// Once we hold ptable.lock, we can be
	// guaranteed that we won't miss any wakeup
	// (wakeup runs with ptable.lock locked),
	// so it's okay to release lk.
	if (lk != &ptable.lock) {  //DOC: sleeplock0
		acquire(&ptable.lock);  //DOC: sleeplock1
		release(lk);
	}
	// Go to sleep.
	p->chan = chan;
	p->state = SLEEPING;

	sched();

	// Tidy up.
	p->chan = 0;

	// Reacquire original lock.
	if (lk != &ptable.lock) {  //DOC: sleeplock2
		release(&ptable.lock);
		acquire(lk);
	}
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void wakeup1(void *chan) {
	struct proc *p;

	for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
		if (p->state == SLEEPING && p->chan == chan) {
			p->state = RUNNABLE;
			// ***
			insertExpired(p);
		}
}

// Wake up all processes sleeping on chan.
void wakeup(void *chan) {
	acquire(&ptable.lock);
	wakeup1(chan);
	release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int kill(int pid) {
	struct proc *p;

	acquire(&ptable.lock);
	for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
		if (p->pid == pid) {
			p->killed = 1;
			// Wake process from sleep if necessary.
			if (p->state == SLEEPING) {
				p->state = RUNNABLE;
				// ***
				insertExpired(p);
			}
			release(&ptable.lock);
			return 0;
		}
	}
	release(&ptable.lock);
	return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void procdump(void) {
	static char *states[] = { [UNUSED] "unused", [EMBRYO] "embryo", [SLEEPING
			] "sleep ", [RUNNABLE] "runble", [RUNNING] "run   ", [ZOMBIE
			] "zombie" };
	int i;
	struct proc *p;
	char *state;
	uint pc[10];

	for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
		if (p->state == UNUSED)
			continue;
		if (p->state >= 0 && p->state < NELEM(states) && states[p->state])
			state = states[p->state];
		else
			state = "???";
		cprintf("%d %s %s", p->pid, state, p->name);
		if (p->state == SLEEPING) {
			getcallerpcs((uint*) p->context->ebp + 2, pc);
			for (i = 0; i < 10 && pc[i] != 0; i++)
				cprintf(" %p", pc[i]);
		}
		cprintf("\n");
	}
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void scheduler(void) {
	struct proc *p;
	struct cpu *c = mycpu();
	c->proc = 0;

	for (;;) {
		// Enable interrupts on this processor.
		sti();
		acquire(&ptable.lock);

		p = getNextProc();
		if (p != NULL) {
			// Switch to chosen process.  It is the process's job
			// to release ptable.lock and then reacquire it
			// before jumping back to us.

//			print_proc(p);
//			cprintf("last: %d\texecuting: %d\n", &ptable.proc[NPROC], p);
			c->proc = p;
			switchuvm(p);
			p->state = RUNNING;

			swtch(&(c->scheduler), p->context);
			switchkvm();

			// Process is done running for now.
			// It should have changed its p->state before coming back.
			c->proc = 0;
		}
		release(&ptable.lock);
	}
}

// Return a simplied version of the process table
// for user-space programs
int getptable(int nproc, int size, char *buffer) {
	struct proc *p_src;
	struct proc_us *p_dst;
	int n;

	acquire(&ptable.lock);
	p_src = ptable.proc;
	p_dst = (struct proc_us*) buffer;

	for (n = 0; n < nproc; n++) {
		// avoid copying unused process slots
		if (p_src->state == UNUSED)
			continue;
		p_dst->sz = p_src->sz;
		p_dst->state = p_src->state;
		p_dst->pid = p_src->pid;
		p_dst->ppid = p_src->parent->pid;
		p_dst->priority = p_src->priority;
		memmove(p_dst->name, p_src->name, 16);
		p_src++;
		p_dst++;
	}
	release(&ptable.lock);
	return 0;
}

// Change Process Priority
int setpriority(int pid, int priority) {
	struct proc *p;

	acquire(&ptable.lock);
	for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
		if (p->pid == pid) {
			p->priority = priority;
			break;
		}
	}
	release(&ptable.lock);

	return pid;
}


struct queue_container initQueueContainer(struct queue_container * queue) {
	for (int i = 0; i < MAX_PRIORITY; i++) {
		queue->validity[i] = 0;
	}
	queue->actual_queue = NULL;
	return *queue;
}

// Initialize and clean a queue
int initQueue(struct priority_queue* queue, int priority) {
	queue->next = NULL;
	queue->last = -1;
	queue->pos = 0;
	// No malloc
	// If malloc was be usable I'd have done reallocation like this
	//queue->size = 8;
	//queue->procs = malloc(sizeof(*(queue->procs)) * queue->size);
	queue->priority = priority;
	return 1;
}

/* Get the priority_queue of a specific priority in the
 * specified queue_container.
 * If it does not exists then it will be created.
 */
struct priority_queue* getQueue(struct queue_container* queue_container,
		int priority) {
	if (queue_container->validity[priority]) {
		return queue_container->queue[priority];
	}

	struct priority_queue* new = queue_container->queue[priority];
	initQueue(new, priority);

	queue_container->validity[priority] = 1;

	if (queue_container->actual_queue == NULL
			|| queue_container->actual_queue->priority > priority) {
		new->next = queue_container->actual_queue;
		queue_container->actual_queue = new;

		return new;
	}
	// else
	struct priority_queue* before = queue_container->actual_queue;
	for (int i = priority - 1; i >= 0; i--) {
		if (queue_container->validity[i]) {
			before = queue_container->queue[i];
			break;
		}
	}
	new->next = before->next;
	before->next = new;

	return new;
}

/* Insert the proc in the specified priority_queue
 * and update info of the priority_queue
 */
int addProcToQueue(struct proc* proc, struct priority_queue * queue) {
	queue->last++;
	// Round robin insert with '%' operator in procs (proc[64]) to handle overflow
	queue->procs[queue->last % NPROC] = proc;
	return 0;
}

/* Inserts proc in the specified queue_container
*  in the right position.
*/
int insertRunnable(struct proc* proc, struct queue_container* queue_container) {
	if (proc->state != RUNNABLE) {
		return -1;
	}
	int priority = proc->priority;
	struct priority_queue * queue = getQueue(queue_container, priority);

	addProcToQueue(proc, queue);
	return 1;
}

/* If proc is RUNNABLE the funcion inserts it in the ACTIVE queue of processes
 * proc: process to be inserted in the 'queue' of execution
 *
 * Returns: 1 if succesfully inserted, otherwise -1
 * ***: This function should be called only at the creation of a proces
 */
int insertActive(struct proc* proc) {
	return insertRunnable(proc, active);
}

/*  As insertActive, but it inserts proc in ACTIVE or EXPIRED queue.
 *
 * ***: In this function it could be implemented a policy
 * of re-insertion of proc   in ACTIVE queue in spite of EXPIRED one.
 * Inserting in ACTIVE means to give more exec time to a process.
 */
int insertExpired(struct proc* proc) {

	/* My default policy is to reinstert a proces in ACTIVE
	 * 1 time over the priority(+1) of the process
	 * priority == 1  --> reinsert 1 time in 2
 	 * priority == 10 -->  reinsert 1 time in 11
	 */
	int cicle = (proc->ncicle)++;
	if ((cicle % (proc->priority + 1)) == 0)
		return insertRunnable(proc, active);
	return insertRunnable(proc, expired);
}

// Exchange active container with expired
int exchange() {
	struct queue_container *tmp = expired;
	expired = active;
	active = tmp;
	return 0;
}

/*Move actual_queue of ACTIVE container to the next priority_queue
* setting all the necessary
*/
int setNextQueue() {
	struct priority_queue* queue = active->actual_queue;
	struct priority_queue* next = queue->next;
	int priority = active->actual_queue->priority;
	active->validity[priority] = 0;

	active->actual_queue = next;
	if (next != NULL) {
		return 0;
	} else { // next == NULL
		return exchange();
	}
	return -1;
}


/* Find & returns the process that belongs to the queue
 * with the lowest priority among the valid ones
 * and that has been inserted before all processes
 * not yet extracted from its priority_queue.
 * If needed it exchange queue_container
*/
struct proc * getNextProc() {
	struct priority_queue* queue = active->actual_queue;
	if (queue == NULL) {
		exchange();
		queue = active->actual_queue;
		if (queue == NULL) {
			return NULL;
		}
	}
	if (queue->pos > queue->last) {
		setNextQueue();
		return getNextProc();
	}
	struct proc * ret = queue->procs[queue->pos % NPROC];
	(queue->pos)++;
	return ret;
}

/* NOT USED (Debug)
* Temporary debug function used to spot where procs
* are created and then insert them.
*/
int fill_procs() {
	acquire(&ptable.lock);
	struct proc *p;
	cprintf("start fill\n");
	for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
		print_proc(p);
		if (p->state == RUNNABLE) {
			cprintf("inserisco da fill\n");
			insertActive(p);
		}
	}
	release(&ptable.lock);
	return 0;
}

/* NOT USED (Debug)
 * Like ps
 * Usefull funcion to show proc info
 */
void print_proc(struct proc *p) {
	cprintf("%d\tN/A", p->pid);

	cprintf("\t%d", p->priority);
	cprintf("\t%d", p->sz);

	switch (p->state) {
	case UNUSED:
		cprintf("\t%s", "UNUSED  ");
		break;
	case EMBRYO:
		cprintf("\t%s", "EMBRYO  ");
		break;
	case SLEEPING:
		cprintf("\t%s", "SLEEPING");
		break;
	case RUNNABLE:
		cprintf("\t%s", "RUNNABLE");
		break;
	case RUNNING:
		cprintf("\t%s", "RUNNING ");
		break;
	case ZOMBIE:
		cprintf("\t%s", "ZOMBIE  ");
		break;
	}
	cprintf("\t%s\n", p->name);
}

