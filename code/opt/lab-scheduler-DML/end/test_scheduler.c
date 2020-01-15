#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(int argc, char *argv[]) {

  #ifndef DML

  int pid;
  int k, nprocess;
  int z, steps;
  int priority;
  char buffer_src[1024], buffer_dst[1024];

  if (argc != 4) {
    printf(2, "usage: %s nsubprocess nsteps priority\n", argv[0]);
    exit();
  }

  nprocess = atoi(argv[1]);
  steps = atoi(argv[2]);
  priority = atoi(argv[3]);

  for (k = 0; k < nprocess; k++) {
    // ensure different creation times (proc->ctime)
    // needed for properly testing FCFS scheduling
    sleep(2);
    
    pid = fork();
    if (pid < 0) {
      printf(1, "%d failed in fork!\n", getpid());
      exit();
    } else if (pid == 0) {  
      // child
      printf(1, "[pid=%d] created\n", getpid());
      setpriority(getpid(), priority);
      for (z = 0; z < steps; z += 1) {
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

  #else

  printf(2, "error: test not for DML scheduler policy\n");

  #endif

  exit();
}
