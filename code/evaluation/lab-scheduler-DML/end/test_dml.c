#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {

  #ifdef DML

  int pid;
  int i, j, k, z, nprocess;
  char buffer_src[1024], buffer_dst[1024];
  char *echoargv[] = { "" };

  if (argc != 2) {
    printf(2, "usage: %s nsubprocess\n", argv[0]);
    exit();
  }

  nprocess = atoi(argv[1]);

  for (i = 0; i < nprocess; i++) {
    sleep(2);
    pid = fork();
    if (pid < 0) {
      printf(1, "%d failed in fork!\n", getpid());
      exit();
    } else if (pid == 0) {  
      // child
      j = getpid() % 4; 
      switch(j) {
	case 0: // processo che esegue per tutto il quanto di tempo:
		printf(1, "[pid=%d CPU] created\n", getpid());
		for (k = 0; k < 1000000; k++) {
        	    memmove(buffer_dst, buffer_src, 1024);
       	  	    memmove(buffer_src, buffer_dst, 1024);
     	 	}
		break;
	case 1: // processo che simula un'operazione di I/O
		printf(1, "[pid=%d I/O] created\n", getpid());
		for (k = 0; k < 100; k++){
		    sleep(1);
		}
		break;
	case 2: // processo che rilascia volontariamente la CPU
	        printf(1, "[pid=%d YIE] created\n", getpid());
                for (k = 0; k < 100; k++){
		    for (z = 0; z < 1000000; z++){}
		    yield();
		}
		break;
	case 3: // processo che richiama la system call exec
		printf(1, "[pid=%d EXE] created\n", getpid());
		for (k = 0; k < 100; k++){
		    for (z = 0; z < 1000000; z++){}
		    exec("echo", echoargv);
		}
		break;
      }
      exit();
    }
  }

  for (i = 0; i < nprocess; i++) {
      pid = wait();
      j = pid % 4; 
      switch(j) {
	case 0: // processo che esegue per tutto il quanto di tempo:
		printf(1, "[pid=%d CPU] terminated\n", pid);
		break;
	case 1: // processo che simula un'operazione di I/O
		printf(1, "[pid=%d I/O] terminated\n", pid);
		break;
	case 2: // processo che rilascia volontariamente la CPU
	        printf(1, "[pid=%d YIE] terminated\n", pid);
		break;
	case 3: // processo che richiama la system call exec
		printf(1, "[pid=%d EXE] terminated\n", pid);
		break;
      }
  }

  #else

  printf(2, "error: test for DML scheduler policy\n");

  #endif
  exit();
}
