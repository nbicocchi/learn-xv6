#include "types.h"
#include "user.h"
#include "stat.h"
#include "fcntl.h"


int main(void){
  int pid;
  int k;
  char *argv[] = {"", 0};

  //Ensure that the refcount of a key doesn't fall to 0 if the producer exit before consumer starts
  int key=0; shmgetat(key,4);
  key=1; shmgetat(key,4);

  for (k = 0; k < 2; k++) {
    pid = fork ();
    if (pid < 0) {
      printf(1, "%d failed in fork!\n", getpid());
      exit();
    } else if (pid == 0) {  
      if(k==0) argv[0]="producer";
      else argv[0]="consumer";

      exec(argv[0], argv);
      printf(1, "Exec failed");
      exit();
    }
  }

  for (k = 0; k < 2; k++) {
    pid = wait();
    printf(1, "[pid=%d] terminated\n", pid);
  }

  exit();
}

