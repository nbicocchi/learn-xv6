#include "types.h"
#include "user.h"
#include "sdh.h"
#include "fcntl.h"


int
main(int argc, char *argv[])
{
	if (argc != 2){
				printf(1, "Usage: test_schedulers nprocess\n");
				exit();
 		}
	int i;
	int n;
	int j = 0;
	int k;
	int retime;
	int rutime;
	int stime;
	int sums[3][3];
	int isched;
        n = atoi(argv[1]);
	int pid;
	int scheduler_running; //parameter for saving scheduler which is runnning at the moment
	int fd;
        fd = open("data.txt", O_RDWR | O_CREATE);
	write(fd,"S!\n",strlen("S!"));
	scheduler_running = getscheduler();
        for(isched = 0; isched < 5; isched++){
	setscheduler(isched);
	printf(1,"Testing SCHEDULER : %d %s \n", getscheduler(), schedulerName[getscheduler()]);
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			sums[i][j] = 0;
	
	for (i = 0; i < n; i++) {
		j = i % 3;
		sleep(2); //FCFS works properly
		pid = fork();
		if (pid == 0) {//child
			j = (getpid() - 4) % 3; // ensures independence from the first son's pid when gathering the results in the second part of the program
			if (getscheduler() == 4){ //Inside SML scheduler
			// Need to differentiate priorities
			switch(j) {
				case 0:
          				setpriority(getpid(), 1); // 1st row --> highest priority
					break;
				case 1:
          				setpriority(getpid(), 8); // 2nd row --> intermediate priority
					break;
				case 2:
         				 setpriority(getpid(), 17); // 3rd row --> lowest priority
					break;
			}
				
							
			}
			switch(j) {
				case 0: //CPU‐bound process (CPU):
						for (double z = 0; z < 100000.0; z+= 0.1){
				         double x =  x + 3.14 * 89.64;   // useless calculations to consume CPU time
					}
					break;
				case 1: //short tasks based CPU‐bound process (S‐CPU):
					for (k = 0; k < 1000; k++){
						for (j = 0; j < 1000; j++){}
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
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			sums[i][j] /= n;
	printf(1, "\n\nCPU bound:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n\n", sums[0][0], sums[0][1], sums[0][2], sums[0][0] + sums[0][1] + sums[0][2]);
	printf(1, "CPU-S bound:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n\n", sums[1][0], sums[1][1], sums[1][2], sums[1][0] + sums[1][1] + sums[1][2]);
	printf(1, "I/O bound:\nAverage ready time: %d\nAverage running time: %d\nAverage sleeping time: %d\nAverage turnaround time: %d\n\n\n", sums[2][0], sums[2][1], sums[2][2], sums[2][0] + sums[2][1] + sums[2][2]);
	setscheduler(isched);
	// saving data //double for
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			char value[3];
			// 3 cases
			//int ret;
			int resto;
			int ris;

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
			int one, two, three; // 3 single digits
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
			// add other cases for more digits if necessary

			int sum;
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
				
			}
			
		}	
		
	}
	printf(1,"Test saved\n");
	}

	setscheduler(scheduler_running); //restore current scheduler
	write(fd,"F!\n",strlen("F!\n"));
        /*
        close(fd);
	fd = open("data.txt", O_CREATE);
        int t;
	read(fd, &t, sizeof(int));
	printf(1,"last read %d\n", t);
	read(fd, &t, sizeof(int));
	printf(1,"last read %d\n", t);
	*/
        close(fd);
	exit();
}
