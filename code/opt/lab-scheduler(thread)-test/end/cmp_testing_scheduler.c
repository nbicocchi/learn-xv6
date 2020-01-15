#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sdh.h"

#define NUM_THREADS 10
#define TARGET_COUNT_FOR_THREAD 50000
#define SEMAPHORE_NUM1 0
#define SEMAPHORE_NUM2 1
#define NUM_SCHEDULER 5

uint g_counter1;
uint g_counter2;
int makespan[NUM_SCHEDULER]; 

void thread1(void *arg)
{
	int i;
	int counter;

	sleep(10);

	for (i=0; i<TARGET_COUNT_FOR_THREAD; i++) {
		/*Start critical section*/
		sem_wait(SEMAPHORE_NUM1, 1);
		/*Algorithm to stress Scheduler  --> change with what do you want*/ 
		counter = g_counter1;
		sleep(0);
		counter++;
		counter=counter*2;
		counter=counter/2;
		counter=counter*3;
		counter=counter/3;
		sleep(0);
		if(counter%700==0){
			printf(1,"%d",*(int*)arg);
		}
		g_counter1 = counter;
		/*End algorithm*/
		sem_signal(SEMAPHORE_NUM1, 1);
		/*End critical section*/
	}
	exit();
}
void thread2(void *arg)
{
	int i;
	int counter;

	sleep(10);

	for (i=0; i<TARGET_COUNT_FOR_THREAD; i++) {
		/*Start critical section*/
		sem_wait(SEMAPHORE_NUM2, 1);
		/*Algorithm to stress Scheduler  --> change with what do you want*/ 
		counter = g_counter2;
		sleep(0);
		counter++;
		counter=counter*2;
		counter=counter/2;
		counter=counter*3;
		counter=counter/3;
		sleep(0);
		if(counter%700==0){
			printf(1,"%d",*(int*)arg);
		}
		g_counter2 = counter;
		/*End algorithm*/
		sem_signal(SEMAPHORE_NUM2, 1);
		/*End critical section*/
	}
	exit();
}

int get_makespan(int pid){
  struct proc_us ptable[NPROC];
  struct proc_us *p;
  if (getptable(NPROC, NPROC * sizeof(struct proc_us), &ptable) < 0) {
    printf(1, "Error getting ptable");
    exit();
	return -1;
  }
  for(p = ptable; p != &ptable[NPROC-1]; p++) {      
        if (p->state == UNUSED) continue;

		if (p->pid == pid){
			int makespan = p->stime + p->retime + p-> rutime;
			return makespan;
		}
  }
  return -1;
}

int main(int argc, char **argv)
{
	int max = sizeof(schedulerName) / sizeof(char *);
	printf(1,"Number of scheduler to test %d\n",max);
	for (int sid=0; sid<max;sid++){
		int csid = setscheduler(sid);
		if(csid == -1) {
			printf(2, "Error from system call\n");
			exit();
		}
		printf(1, "\nTest with Scheduler policy: %s - SID %d\n", schedulerName[csid], csid);
		int i,j;
		int sem_size = 1;
		int final_counter;
		int final_target = NUM_THREADS * TARGET_COUNT_FOR_THREAD;
		// Initialize semaphore to 1
		if (sem_init(SEMAPHORE_NUM1, sem_size) < 0)
		{
			printf(1, "main: error initializing semaphore 1 %d\n", SEMAPHORE_NUM1);
			exit();
		}
		if (sem_init(SEMAPHORE_NUM2, sem_size) < 0)
		{
			printf(1, "main: error initializing semaphore 2 %d\n", SEMAPHORE_NUM2);
			exit();
		}

		// Initialize counter
		g_counter1 = 0;
		g_counter2 = 0;
		// Set up thread stuff

		// Stacks
		void *stacks[NUM_THREADS];
		// Args
		int *args[NUM_THREADS];

		// Allocate stacks and args and make sure we have them all
		// Bail if something fails
		for (i=0; i<NUM_THREADS; i++) {
			stacks[i] = (void*) malloc(4096);
			if (!stacks[i]) {
				printf(1, "main: could not get stack for thread %d, exiting...\n");
				exit();
			}

			args[i] = (int*) malloc(4);
			if (!args[i]) {
				printf(1, "main: could not get memory (for arg) for thread %d, exiting...\n");
				exit();
			}

			*args[i] = i;
		}

		printf(1, "main: running with %d threads and 2 semaphore...\n", NUM_THREADS);

		// Start all children
		for (i=0; i<NUM_THREADS; i++) {
			//int pid=0;
			if(i%2==0){
				clone(thread1, args[i], stacks[i]);
			}
			else{
				clone(thread2, args[i], stacks[i]);
			}
			//printf(1, "main: created thread with pid %d\n", pid);
			printf(1, "M");
		}
	
		// Wait for all children
		for (i=0; i<NUM_THREADS; i++) {
			void *joinstack;
			join(&joinstack);
			for (j=0; j<NUM_THREADS; j++) {
				if (joinstack == stacks[i]) {
					break;
				}
			}
		} 

		// Check the result
		final_counter = g_counter1 + g_counter2;
		printf(1, "\nNumber of operations done is %d, the total is %d\n", final_counter, final_target);
		if (final_counter == final_target)
			printf(1, "TEST SUCCESSFULLY!\n");
		else
			printf(1, "TEST FAILED!\n");
	
		// Clean up semaphore
		sem_destroy(SEMAPHORE_NUM1);
		sem_destroy(SEMAPHORE_NUM2);
		int mainpid = getpid();
		makespan[sid]=get_makespan(mainpid);
		for (int n = sid -1;n>=0;n--){
			makespan[sid] = makespan[sid] - makespan[n];			
		}
		printf(1,"Scheduler %s | Makespan is: %d\n",schedulerName[sid],makespan[sid]);
		
		}
	printf(1,"\n*************************************FINAL STATISTICS*************************************\n");
	printf(1, "\nTest running with %d threads, 2 semaphore and %d operations for thread in critical section\n\n", NUM_THREADS, TARGET_COUNT_FOR_THREAD);
	for (int sid=0;sid<max;sid++){
		float speedup = (float)makespan[2]/(float)makespan[sid];
		float efficiency = speedup / (float)NUM_THREADS*100;
		/*Print float number*/
		int intspeedup = (int)speedup;
		speedup=speedup-intspeedup;
		int decspeedup=speedup*100000;
		char zero[6]="";
		if(decspeedup<10000){
			zero[0] = '0';
			zero[1] = 0;
		}
		if (decspeedup<1000){
			zero[1] = '0';
			zero[2] = 0;
		}
		if (decspeedup<100){
			zero[2] = '0';
			zero[3] = 0;
		}
		if (decspeedup<10){
			zero[3] = '0';
			zero[4] = 0;
		}
		if (decspeedup<1){
			zero[4] = '0';
			zero[5] = 0;
		}
		int intefficiency = (int)efficiency;
		efficiency=efficiency-intefficiency;
		int decefficiency=efficiency*100000;
		char zeroe[6]="";	
		if(decefficiency<10000){
			zeroe[0] = '0';
			zeroe[1] = 0;
		}
		if (decefficiency<1000){
			zeroe[1] = '0';
			zeroe[2] = 0;
		}
		if (decefficiency<100){
			zeroe[2] = '0';
			zeroe[3] = 0;
		}
		if (decefficiency<10){
			zeroe[3] = '0';
			zeroe[4] = 0;
		}
		if (decefficiency<1){
			zeroe[4] = '0';
			zeroe[5] = 0;
		}
		printf(1,"Scheduler %s | Makespan is: %d | Speedup is: %d.%s%d | Efficiency is: %d.%s%d% \n",schedulerName[sid],makespan[sid],intspeedup,zero,decspeedup,intefficiency,zeroe,decefficiency);		
	}
	// Exit		
	exit();
}
