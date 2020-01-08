#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

#include "sdh.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

//changes begin
struct semaphore {
  int value;
  int active;
  struct spinlock lock;
};

struct semaphore sema[32];

//changes end

static struct proc *initproc;

int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

struct spinlock schedulerlock;

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
  initlock(&schedulerlock, "schedulerlock");

  ////////////////////////////////////
  // Default scheduler policy Init  //
  ////////////////////////////////////
  // if (schedSelected == n) {
  //    -> init
  // } 
  ////////////////////////////////////
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
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
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  p->priority = 10;
  p->ctime = ticks;
  p->stime = 0;
  p->retime = 0;
  p->rutime = 0;

  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;
  p->isthread = 0;   //changes
  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
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

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
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

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;

  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
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
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
		/* changes begin */
      if (p->isthread == 1) {
        // p->state = ZOMBIE;
         kfree(p->kstack);
         p->kstack = 0;
         p->state = UNUSED;
      }
      else {
      /* changes end */
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }
  }
  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
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
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
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
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;

  for(;;) {
      // Enable interrupts on this processor.
      sti();

      // Loop over process table looking for process to run.
      acquire(&ptable.lock);
      //p = ready_process();

      acquire(&schedulerlock);
      p = (*ready_process)();
      release(&schedulerlock);
     
      if (p != 0) {
          // Switch to chosen process.  It is the process's job
          // to release ptable.lock and then reacquire it
          // before jumping back to us.
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

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  myproc()->state = RUNNABLE;
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
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
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
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
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;
  struct proc *pc; //changes

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
	  /* changes begin */
      for(pc = ptable.proc; pc < &ptable.proc[NPROC]; pc++){
         if ((pc->parent == p) && (pc->isthread == 1)){
            pc->killed = 1;
            if (pc->state == SLEEPING)
               pc->state = RUNNABLE;
         }
      }
      /* changes end */ 
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}
// changes begin

int sem_init(int sem, int value)
{
  acquire(&sema[sem].lock);

  if (sema[sem].active == 0)
  {
     sema[sem].active = 1;
     sema[sem].value = value;
  }
  else
  {
     return -1;
  }  

  release(&sema[sem].lock);

  return 0;
}

int
sem_destroy(int sem)
{
  acquire(&sema[sem].lock);
  sema[sem].active = 0;
  release(&sema[sem].lock);

  return 0; 
}

int sem_wait(int sem, int count)
{
  acquire(&sema[sem].lock);

  if (sema[sem].value >= count)
  {
     sema[sem].value = sema[sem].value - count;
  }
  else
  {
     while (sema[sem].value < count)
     {  
        sleep(&sema[sem],&sema[sem].lock);
     }
     sema[sem].value = sema[sem].value - count;
  }

  release(&sema[sem].lock);

  return 0;
}


int sem_signal(int sem, int count)
{
  acquire(&sema[sem].lock);

  sema[sem].value = sema[sem].value + count;
  //cprintf("thread:sem value is %d \n",sema[sem].value);
  wakeup(&sema[sem]); 
  release(&sema[sem].lock);

  return 0;
}


int clone(void (*func)(void *), void *arg, void *stack)
{

   int i, pid;
   struct proc *np;
   int *myarg;
   int *myret;
   struct proc *curproc = myproc();

   if((np = allocproc()) == 0)
     return -1;

   np->pgdir = curproc->pgdir; 
   np->sz = curproc->sz;
   np->parent = curproc;
   *np->tf = *curproc->tf;
   np->stack = stack;

   np->tf->eax = 0; 

   /*
   *myarg = (int)arg;

   *myret = np->tf->eip;
   */
   
   np->tf->eip = (int)func;

   myret = stack + 4096 - 2 * sizeof(int *);
   *myret = 0xFFFFFFFF;
   
   myarg = stack + 4096 - sizeof(int *);
   *myarg = (int)arg;

   np->tf->esp = (int)stack +  PGSIZE - 2 * sizeof(int *);
   np->tf->ebp = np->tf->esp;

   np->isthread = 1;
  
   for(i = 0; i < NOFILE; i++)
     if(curproc->ofile[i])
       np->ofile[i] = filedup(curproc->ofile[i]);
   np->cwd = idup(curproc->cwd);

   safestrcpy(np->name, curproc->name, sizeof(curproc->name));

   pid = np->pid;

   acquire(&ptable.lock);
   np->state = RUNNABLE;
   release(&ptable.lock);

   return pid;  
}

int join(void **stack)
{

  struct proc *p;
  int haveKids, pid;
  struct proc *curproc = myproc();

  acquire(&ptable.lock);
  for(;;) {
    haveKids = 0;

    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
      if (p->parent != curproc || p->isthread != 1 )
        continue;
      haveKids = 1;

      if (p->state == ZOMBIE) {
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        p->state = UNUSED;
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        *stack = p->stack;
        release(&ptable.lock);
        return pid;
      }
    }
    
    if (!haveKids || curproc->killed) {
      release(&ptable.lock);
      return -1;
    }

    sleep(curproc, &ptable.lock);

  }
  return 0;
}

//changes end

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

// Return a simplied version of the process table
// for user-space programs
int 
getptable(int nproc, int size, char *buffer){
  struct proc *p_src;
  struct proc_us *p_dst;
  int n;
  
  acquire(&ptable.lock);
  p_src = ptable.proc;
  p_dst = (struct proc_us*)buffer;

  for (n = 0; n < nproc; n++) {
    // avoid copying unsed process slots
    if (p_src->state == UNUSED) continue;
    p_dst->sz = p_src->sz; 
    p_dst->state = p_src->state;
    p_dst->pid = p_src->pid;
    p_dst->ppid = p_src->parent->pid;
    memmove(p_dst->name, p_src->name, 16);
    p_dst->priority = p_src->priority;
    p_dst->ctime = p_src->ctime;
    p_dst->stime = p_src->stime;
    p_dst->retime = p_src->retime;
    p_dst->rutime = p_src->rutime;
    p_src++;
    p_dst++;
  }
  release(&ptable.lock);
  return 0;
}

// Change Process Priority
int
setpriority(int pid, int priority)
{
  struct proc *p;
  
  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid) {
        p->priority = priority;
        break;
    }
  }
  release(&ptable.lock);

  return pid;
}

// Run every clock tick and update the statistic fields of each process
void 
update_statistics() {
  struct proc *p;
  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    switch(p->state) {
      case SLEEPING:
        p->stime++;
        break;
      case RUNNABLE:
        p->retime++;
        break;
      case RUNNING:
        p->rutime++;
        break;
      default:
        ;
    }
  }
  release(&ptable.lock);
}

// Generate a random number, between 0 and M
// This is a modified version of the LFSR alogrithm
// found here: http://goo.gl/At4AIC */
int
random(int max) {

  if(max <= 0) {
    return 1;
  }

  static int z1 = 12345; // 12345 for rest of zx
  static int z2 = 12345; // 12345 for rest of zx
  static int z3 = 12345; // 12345 for rest of zx
  static int z4 = 12345; // 12345 for rest of zx

  int b;
  b = (((z1 << 6) ^ z1) >> 13);
  z1 = (((z1 & 4294967294) << 18) ^ b);
  b = (((z2 << 2) ^ z2) >> 27);
  z2 = (((z2 & 4294967288) << 2) ^ b);
  b = (((z3 << 13) ^ z3) >> 21);
  z3 = (((z3 & 4294967280) << 7) ^ b);
  b = (((z4 << 3) ^ z4) >> 12);
  z4 = (((z4 & 4294967168) << 13) ^ b);

  // if we have an argument, then we can use it
  int rand = ((z1 ^ z2 ^ z3 ^ z4)) % max;

  if(rand < 0) {
    rand = rand * -1;
  }

  return rand;
}

// system call - get scheduler policy id
int
getscheduler()
{
  int sid; 
  acquire(&schedulerlock);
  sid = schedSelected;
  release(&schedulerlock);
  return sid;
}

// system call - set scheduler policy
int
setscheduler(int sid) 
{
  int max = sizeof(schedulerName) / sizeof(char *);
  if(sid < 0 || sid > max) {
    return -1;
  }

  acquire(&schedulerlock);

  ///////////////////////////////////////////////
  // Init / remove scheduler policy in runtime //
  ///////////////////////////////////////////////
  // if (sid == n) {
  //    -> init
  // } else {
  //    -> remove
  // }
  ///////////////////////////////////////////////

  ready_process = schedulerFunction[sid];
  schedSelected = sid;
  release(&schedulerlock);

  return sid;
}


// The following funtions are used by scheduler() for selecting 
// the next process to be executed.
// All of them select a process (using a different policy) among runnables.

/////////////////////////////////////////////
// Scheduler policies - Pre-installed code //
/////////////////////////////////////////////

// Default scheduler -------------------
struct proc *defaultScheduler(void) {
  struct proc *p;
  int i, rnd;
  
  rnd = random(NPROC);
  for (i = 0; i < NPROC; i++) {
    p = ptable.proc + ((rnd + i) % NPROC); 
    if (p->state == RUNNABLE) return p;
  }
  return 0;
}

// Priority Scheduler -------------------
struct proc *priorityScheduler() {
  struct proc *p, *pcandidate = 0;
  
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(p->state == RUNNABLE) {
      if (pcandidate == 0) {
        pcandidate = p;
      } else if (p->priority < pcandidate->priority) {
        pcandidate = p;
      }
    }
  }
  return pcandidate; 
}

// FCFS Scheduler -----------------------
struct proc *fcfsScheduler() {
  struct proc *p, *pcandidate = 0;
  
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(p->state == RUNNABLE) {
      if (pcandidate == 0) {
        pcandidate = p;
      } else if (p->ctime < pcandidate->ctime) {
        pcandidate = p;
      }
    }
  }
  return pcandidate; 
}

// CFS Scheduler -------------------------
struct proc *cfsScheduler() {
  struct proc *p, *pcandidate = 0;
  
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
    if(p->state == RUNNABLE) {
      if (pcandidate == 0) {
        pcandidate = p;
      } else if (p->rutime < pcandidate->rutime) {
        pcandidate = p;
      }
    }
  }
  return pcandidate; 
}

// SML Scheduler ---------------------------
struct proc *smlScheduler() {
  struct proc *p_i, **pp_i; 
  struct proc *high_queue[NPROC], *mid_queue[NPROC], *low_queue[NPROC];
  int high_index = 0, mid_index = 0, low_index = 0;
  
  for(p_i = ptable.proc; p_i < &ptable.proc[NPROC]; p_i++) {
    if (p_i->state != RUNNABLE) continue;

    if (p_i->priority <= 7) {
      high_queue[high_index++] = p_i;
    } else if (p_i->priority > 7 && p_i->priority <= 14) {
      mid_queue[mid_index++] = p_i;
    } else {
      low_queue[low_index++] = p_i;
    }
  }

  if (high_index > 0) {
    return high_queue[random(high_index)];
  } else if (mid_index > 0) {
    return mid_queue[random(mid_index)];
  } else if (low_index > 0) {
    return low_queue[random(low_index)];
  }

  return 0;
}