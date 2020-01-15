#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"


#define NUM_PROD 1
#define NUM_CONS 2
#define INCREMENT 50
#define DECREMENT 25
#define NUM_PREL_FOR_CONS 2
#define NUM_DEP_FOR_PROD 2
#define TARGET_COUNT_PER_CHILD 50
#define BANK_FILE "bank"
#define BANK_INITIAL_FOUND 0
#define SEMAPHORE_NUM 0

void starter(int seconds){
	for(int i = 0; i < seconds; i++){
		sleep(100);
		printf(1,".");
	}
	printf(1,"\n\n\n");
}

int bank_init(char *filename, int value)
{
	int fd;

	if ((fd = open(filename, O_CREATE | O_RDWR)) < 0) {
		printf(1, "counter_init: error initializing file: %s\n", filename);
		exit();
	}

	printf(fd, "%d\n", value);
	close(fd);

	return 0;
}

int counter_get(char *filename)
{
	int fd, n, value;
	char buffer[32];

	if ((fd = open(filename, O_CREATE | O_RDWR)) < 0) {
		printf(1, "counter_get: error opening file: %s\n", filename);
		exit();
	}

	n = read(fd, buffer, 31);
	buffer[n] = '\0';
	value = atoi(buffer);
	close(fd);

	return value;
}

int counter_set_prod(char *filename, int value)
{
	int fd;

	if ((fd = open(filename, O_CREATE | O_RDWR)) < 0) {
		printf(1, "counter_set: error opening file: %s\n", filename);
		exit();
	}

	printf(fd, "%d\n", value);
	close(fd);

	return value;
}

int counter_set_cons(char *filename, int value)
{
	int fd;

	if ((fd = open(filename, O_CREATE | O_RDWR)) < 0) {
		printf(1, "counter_set: error opening file: %s\n", filename);
		exit();
	}

	printf(fd, "%d\n", value);
	close(fd);

	return value;
}


void prod(void)
{
	int i;
	int counter;
	printf(1, "Prod started...\n");
	sleep(10);

	for (i=0; i<NUM_DEP_FOR_PROD; i++) {
		sem_wait(SEMAPHORE_NUM, 1);
		
		counter = counter_get(BANK_FILE);
		counter=counter + INCREMENT;
		printf(1,"PROD: counter now is : %d \n", counter);
		counter_set_prod(BANK_FILE, counter);

		sem_signal(SEMAPHORE_NUM, 1);
	}

	exit();
}

void cons(void){
	int i;
	int counter;
	printf(1, "Cons started...\n");
	sleep(10);
	for (i=0; i<NUM_PREL_FOR_CONS; i++) {
		sem_wait(SEMAPHORE_NUM, 1);
		
		counter = counter_get(BANK_FILE);
		while((counter - DECREMENT) < 0){
			printf(1,"ATTENTION:the bank account is momentarily empty, the cons must wait for a prod to insert the money \n");
			sem_signal(SEMAPHORE_NUM,1);
			sleep(1000);
			sem_wait(SEMAPHORE_NUM,1);
			counter = counter_get(BANK_FILE);
			
		}
		counter=counter - DECREMENT;
		printf(1,"CONS: counter now is : %d \n", counter);
		counter_set_prod(BANK_FILE, counter);

		sem_signal(SEMAPHORE_NUM, 1);
	}

	exit();
}

int main(int argc, char **argv)
{
	int i;
	int sem_size;
	int final_counter;
	int final_target = BANK_INITIAL_FOUND + (NUM_PROD * NUM_DEP_FOR_PROD * INCREMENT) - (NUM_CONS * NUM_PREL_FOR_CONS * DECREMENT);
	
	char param[6]="-guide";
	
	if(final_target<0){
		printf(1,"You cannot have this configuration of %d producer and %d consumer because the result of their withdrawal and payment in the same account is %d. IMPOSSIBLE\n",NUM_PROD, NUM_CONS, final_target);
		exit();
	}
	if (argc > 2){
		printf(1,"ERROR:The maximum number of parameters is 1.");
		exit();
	}
	
	else if(argc==2 && strcmp(param,argv[1])!=0){
		printf(1,"ERROR:the only second acceptable parameter is -guide");
	}
	else if(argc==2 && strcmp(param,argv[1])==0){
		printf(1,"\n\n\n Author:Federico Campo\n\n in this test the problems of competition\n ,between processes that simulate users who\n withdraw and deposit money on the same\n bank account, are managed with the help\n of semaphore\n\n\n\n\n");
		printf(1,"starting in 10 seconds");
		starter(10);	
	}
	else
		sem_size = 1;

	// Initialize semaphore to 1
	if (sem_init(SEMAPHORE_NUM, sem_size) < 0)
	{
		printf(1, "main: error initializing semaphore %d\n", SEMAPHORE_NUM);
		exit();
	}


	printf(1, "main: initialized semaphore %d to %d\n", SEMAPHORE_NUM, sem_size);

	// Initialize bank
	bank_init(BANK_FILE, BANK_INITIAL_FOUND);

	printf(1, "Running with %d processes; %d PROD and %d CONS...\n", (NUM_PROD + NUM_CONS), NUM_PROD, NUM_CONS);

	// Start all prod
	for (i=0; i<NUM_PROD; i++) {
		int pid = fork();
		if (pid == 0)
			prod();
	}
	

	// Start all cons
	for (i=0; i<NUM_CONS; i++) {
		int pid = fork();
		if (pid == 0)
			cons();
	}

	// Wait for all PROD,CONS
	for (i=0; i<(NUM_PROD + NUM_CONS); i++) {
		wait();
	}

	// Check the result
	final_counter = counter_get(BANK_FILE);
	printf(1, "Final counter is %d, target is %d\n", final_counter, final_target);
	if (final_counter == final_target)
		printf(1, "TEST PASSED!\n");
	else
		printf(1, "TEST FAILED!\n");
	
	// Clean up semaphore
	sem_destroy(SEMAPHORE_NUM);
	
	// Exit
	exit();
}
