#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MAXPROC 7

// posso inserire 7 processi massimo con una diversa durata
// 7 poichè il numero di argomenti massimo accettati dal sistema è 8
int main(int argc, char *argv[]) {
  int pid;
  int k, nprocess;
  int z, i;
  int steps[MAXPROC];
  char buffer_src[1024], buffer_dst[1024];

  if (argc < 3) {
    printf(2, "usage: %s nsubprocess nsteps_Proc1 nsteps_Proc2 ... nsteps_ProcN\n", argv[0]);
    exit();
  }

  nprocess = atoi(argv[1]);
  if(nprocess > MAXPROC){
    printf(2, "too many processes: insert max 7 processes\n", argv[0]);
    exit(); 
  }

  for(i = 0; i < nprocess; i++){
	  steps[i] = atoi(argv[2+i]);
	} 

  for (k = 0; k < nprocess; k++) {
    pid = fork ();
    if (pid < 0) {
      printf(1, "%d failed in fork!\n", getpid());
      exit();
    } else if (pid == 0) {  
      // child
      printf(1, "[pid=%d] created\n", getpid());
      for (z = 0; z < steps[k]; z += 1) {
         // copy buffers one inside the other and back
         // used for wasting cpu time
         memmove(buffer_dst, buffer_src, 1024);
         memmove(buffer_src, buffer_dst, 1024);
      }
      exit();
    }
  }

  for (k = 0; k < nprocess; k++) {
    pid = wait();
    printf(1, "[pid=%d] terminated\n", pid);
  }

  exit();
}
