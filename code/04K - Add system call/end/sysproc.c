#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Return a simplified process table (struct proc_us, proc.h) to user space
int 
sys_getptable(void){
  int nproc;
  int size;
  char *buffer;
  
  if (argint(0, &nproc) < 0) {
    return -1;
  }
  
  if (argint(1, &size) < 0) {
    return -1;
  }

  if (argptr(2, &buffer, size) < 0) {
    return -1;
  }

  if (size != nproc * sizeof(struct proc_us)) {
    return -1;
  }
  
  return getptable(nproc, size, buffer);
}

// Return the parent pid of the calling process
int
sys_getppid(void)
{
  return myproc()->parent->pid;
}

// Fill a struct rtcdate with cmos data
int 
sys_date(void) 
{
  struct rtcdate* date;

  argptr(0, (void*)(&date), sizeof(*date));
  cmostime(date);
  
  return 0;
}
