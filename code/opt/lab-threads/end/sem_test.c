#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

#define NUM_CHILDREN 2
#define TARGET_COUNT_PER_CHILD 50
#define COUNTER_FILE "counter"
#define SEMAPHORE_NUM 0

int counter_init(char *filename, int value)
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

int counter_set(char *filename, int value)
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

void child(void)
{
	int i;
	int counter;

	printf(1, "Process started...\n");
	sleep(10);

	for (i=0; i<TARGET_COUNT_PER_CHILD; i++) {
		sem_wait(SEMAPHORE_NUM, 1);
		
		counter = counter_get("counter");
		counter++;
		counter_set("counter", counter);

		sem_signal(SEMAPHORE_NUM, 1);
	}

	exit();
}

int main(int argc, char **argv)
{
	int i;
	int sem_size;
	int final_counter;
	int final_target = NUM_CHILDREN*TARGET_COUNT_PER_CHILD;

	if (argc >= 2)
		sem_size = NUM_CHILDREN;
	else
		sem_size = 1;

	// Initialize semaphore to 1
	if (sem_init(SEMAPHORE_NUM, sem_size) < 0)
	{
		printf(1, "main: error initializing semaphore %d\n", SEMAPHORE_NUM);
		exit();
	}

	printf(1, "main: initialized semaphore %d to %d\n", SEMAPHORE_NUM, sem_size);

	// Initialize counter
	counter_init(COUNTER_FILE, 0);

	printf(1, "Running with %d processes...\n", NUM_CHILDREN);

	// Start all children
	for (i=0; i<NUM_CHILDREN; i++) {
		int pid = fork();
		if (pid == 0)
			child();
	}
	
	// Wait for all children
	for (i=0; i<NUM_CHILDREN; i++) {
		wait();
	}

	// Check the result
	final_counter = counter_get(COUNTER_FILE);
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
