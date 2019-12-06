#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

int main(int argc, char *argv[]){  
  struct proc_us ptable[NPROC];
  struct proc_us *p;
  
  if (getptable(NPROC, NPROC * sizeof(struct proc_us), &ptable) < 0) {
    printf(1, "Error getting ptable");
    exit();
  }
  
  printf(1, "PID\tPPID\tTICKETS\tMEM\tSTATE\t\tCMD\n");
  for(p = ptable; p != &ptable[NPROC-1]; p++) {      
        if (p->state == UNUSED) continue;

	if (p->pid == 1)
	    printf(1, "%d\tN/A",p->pid);
	else
	    printf(1, "%d\t%d", p->pid, p->ppid);

        printf(1, "\t%d", p->ntickets);
        printf(1, "\t%d", p->sz);

  	switch(p->state){
  	case UNUSED:
  		printf(1, "\t%s", "UNUSED  ");
  		break;
  	case EMBRYO:
  		printf(1, "\t%s", "EMBRYO  ");
  		break;
  	case SLEEPING:
  		printf(1, "\t%s", "SLEEPING");
  		break;
  	case RUNNABLE:
  		printf(1, "\t%s", "RUNNABLE");
  		break;
  	case RUNNING:
  		printf(1, "\t%s", "RUNNING ");
  		break;
  	case ZOMBIE:
  		printf(1, "\t%s", "ZOMBIE  ");
  		break;
  	} 
  	printf(1, "\t%s\n", p->name);
  }

  printf(1, "\n");

  printf(1, "PID\tPPID\tCTIME\tSTIME\tRETIME\tRUTIME\tCMD\n");
  for(p = ptable; p != &ptable[NPROC-1]; p++) {      
        if (p->state == UNUSED) continue;

	if (p->pid == 1)
	    printf(1, "%d\tN/A",p->pid);
	else
	    printf(1, "%d\t%d", p->pid, p->ppid);

        printf(1, "\t%d", p->ctime);
        printf(1, "\t%d", p->stime);
  	printf(1, "\t%d", p->retime);
        printf(1, "\t%d", p->rutime);
        printf(1, "\t%s\n", p->name);
  }

  printf(1, "\n");
  exit();
}