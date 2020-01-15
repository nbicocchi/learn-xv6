#include "types.h"
#include "user.h"
#include "sdh.h"
#include "fcntl.h"


int
main(int argc, char *argv[])
{
	int i,k,j;	// indeces
	int n; 		// number of process passed by argument --> they will be created
	int retime;	// ready time
	int rutime;	// running time
	int stime;	// sleeping time
	int sums[3][3]; // matrix for saving test results
	int isched; 	// index to point a scheduler
	int pid;
	int scheduler_running; //parameter for saving scheduler which is runnning in the system before the test
	int fd;		// file descriptor

	// data.txt is used to put markers inside the file system
	fd = open("data.txt", O_RDWR | O_CREATE);

	// checking argument passed by user
	if (argc != 2){
		printf(1, "Usage: test_schedulers nprocess\n");
		exit();
 	}

	n = atoi(argv[1]); 	
	
	// writing a charachter in the filesystem to identify data inside the file system	
	write(fd,"S!\n",strlen("S!"));

	// saving the current scheduler
	scheduler_running = getscheduler();
        
	
	for(isched = 0; isched < 5; isched++){
		// changing scheduler at every cycle
		setscheduler(isched);
		printf(1,"Testing SCHEDULER : %d %s \n", getscheduler(), schedulerName[getscheduler()]);
	
		// matrix sum is reset
		for (i = 0; i < 3; i++)
			for (j = 0; j < 3; j++)
				sums[i][j] = 0;
	
		for (i = 0; i < n; i++) {
			j = i % 3;
			//sleep for FCFS to work properly
			sleep(2); 
			pid = fork();
			if (pid == 0) {//child

				// ensures independence from the first son's pid when gathering the results in the second part of the program
				j = (getpid() - 4) % 3; 

				if (getscheduler() == 4){ 
					//Inside SML scheduler	
					// Need to differentiate priorities for every process (we have different rows)
					switch(j) {
						case 0:
							// 1st row --> highest priority
          						setpriority(getpid(), 1); 
							break;
						case 1:
							 // 2nd row --> intermediate priority
          						setpriority(getpid(), 8);
							break;
						case 2:
							// 3rd row --> lowest priority
         						setpriority(getpid(), 17); 
							break;
					}
					
				}
			
				// every  process created is a different kind of task (CPU Bound, S-CPU and I/O bound)
				switch(j) {
					case 0: //CPU‐bound process (CPU):
						// useless calculations to consume CPU time
						for (double z = 0; z < 1000000.0; z+= 0.1){
					        	double x =  x + 3.14 * 89.64;   
						}
						break;

					case 1: //short tasks based CPU‐bound process (S‐CPU):
						for (k = 0; k < 100; k++){
							for (j = 0; j < 1000; j++)
								yield();
						}
						break;

					case 2:// simulate I/O bound process (IO)
						for(k = 0; k < 1000; k++){
							sleep(1);
						}
						break;
				}
				exit(); // children exit here
			}
			continue; // father continues to spawn the next child
	}

	// father waits for his children
	for (i = 0; i < n; i++) {
		pid = wait2(&retime, &rutime, &stime);
		int res = (pid - 4) % 3; // correlates to j in the dispatching loop
		switch(res) {
			case 0: // CPU bound processes
				printf(1, "CPU-bound, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n", pid, retime, rutime, stime, retime + rutime + stime);
				sums[0][0] += retime;
				sums[0][1] += rutime;
				sums[0][2] += stime;
				break;
			case 1: // CPU bound processes, short tasks
				printf(1, "CPU-S bound, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n", pid, retime, rutime, stime, retime + rutime + stime);
				sums[1][0] += retime;
				sums[1][1] += rutime;
				sums[1][2] += stime;
				break;
			case 2: // simulating I/O bound processes
				printf(1, "I/O bound, pid: %d, ready: %d, running: %d, sleeping: %d, turnaround: %d\n", pid, retime, rutime, stime, retime + rutime + stime);
				sums[2][0] += retime;
				sums[2][1] += rutime;
				sums[2][2] += stime;
				break;
		}
	}

	//printing final test result of a scheduler
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			sums[i][j] /= n;
			// Printf for user in Xv6
	printf(1, "\n\nCPU bound:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n", sums[0][0], sums[0][1], sums[0][2], sums[0][0] + sums[0][1] + sums[0][2]);
	printf(1, "CPU-S bound:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n", sums[1][0], sums[1][1], sums[1][2], sums[1][0] + sums[1][1] + sums[1][2]);
	printf(1, "I/O bound:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n", sums[2][0], sums[2][1], sums[2][2], sums[2][0] + sums[2][1] + sums[2][2]);
	setscheduler(isched);

	// saving data inside the file system (fs.img) //double for
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){

			// variables used for writing data in file system
			// we need to convert int variables to string variables in order to retrieve them outside Xv6
			char value[4];
			int one, two, three, four; 
			int resto;
			int ris;
		        int sum;

			//1st case --> one digit
			if (sums[i][j] >= 0 && sums[i][j] <= 9){
				value[0] = sums[i][j] + '0';
				write(fd,value,sizeof(char));
				write(fd,",",strlen(",")); //to separate values
	 		}

			//2nd case --> two digits
			if (sums[i][j] >= 10 && sums[i][j] <= 99){
				resto = sums[i][j] % 10;
				ris = sums[i][j] / 10;
				value[0] = ris + '0';
				value[1] = resto + '0';
				write(fd, value, 2*sizeof(char));
				write(fd,",",strlen(",")); //to separate values
			}

			//3rd case --> three digits
			if (sums[i][j] >= 100 && sums[i][j] <= 999){
				one = sums[i][j] / 100;
				two = (sums[i][j] - one*100)/10;
				three = (sums[i][j] - one*100) - two*10;
				value[0] = one + '0';
				value[1] = two + '0';
				value[2] = three + '0';
				write(fd, value, 3*sizeof(char));
				write(fd,",",strlen(",")); //to separate values
			}

			if (sums[i][j] > 1000 && sums[i][j] <= 9999){
				one = sums[i][j] / 1000;
				two = (sums[i][j] - one*1000)/100;
				three = (sums[i][j] - one*1000 - two*100)/10;
				four = (sums[i][j] - one*1000-two*100)%10;
				value[0] = one + '0';
				value[1] = two + '0';
				value[2] = three + '0';
				value[3] = four + '0';
				write(fd, value, 4*sizeof(char));
				write(fd,",", strlen(","));			
			}
	
			if (j == 2){ //last case--> it has to be written the turnaround time
				sum = sums[i][0] + sums[i][1] + sums[i][2];
				if (sum >= 0 && sum <= 9){
					value[0] = sum + '0';
					write(fd,value,sizeof(char));
					write(fd,",",strlen(",")); //to separate values
	 			}	
				if (sum >= 10 && sum <= 99){
					resto = sum % 10;
					ris = sum / 10;
					value[0] = ris + '0';
					value[1] = resto + '0';
					write(fd, value, 2*sizeof(char));
					write(fd,",",strlen(",")); //to separate values
				}
				if (sum>= 100 && sum<= 999){
					one = sum / 100;
					two = (sum - one*100)/10;
					three = (sum - one*100) - two*10;
					value[0] = one + '0';
					value[1] = two + '0';
					value[2] = three + '0';
					write(fd, value, 3*sizeof(char));
					write(fd,",",strlen(",")); //to separate values
				}
				if (sum > 1000 && sum <= 9999){
					one = sum / 1000;
					two = (sum - one*1000)/100;
					three = (sum - one*1000-two*100)/10;
					four = (sum - one*1000-two*100)%10;
					value[0] = one + '0';
					value[1] = two + '0';
					value[2] = three + '0';
					value[3] = four + '0';
					write(fd, value, 4*sizeof(char));
					write(fd,",", strlen(","));			
				}
			}
			
		}	
		
	}

	}

	//restoring current scheduler
	setscheduler(scheduler_running); 

	// closing file data.txt used for markers
	write(fd,"F!\n",strlen("F!\n"));
       
	// closing file data.txt used for markers
        close(fd);

	exit();
}
