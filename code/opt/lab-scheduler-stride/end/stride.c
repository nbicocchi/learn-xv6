#include "types.h"
#include "stat.h"
#include "user.h"

int main (int argc, char *argv[]){
  int i, z, pid, ticket = 0;
  char buffer_src[1024], buffer_dst[1024];

  if(argc != 3)
  {
    printf(2, "usage: nprocess exectime(at least 100000) \n");
    exit();
  }

  int nprocess = atoi(argv[1]);
  int steps = atoi(argv[2]);
  steps = steps*10000;

  printf(1, "\nXv6 Stride Scheduling\n");
  printf(1, "\nPress Ctrl+P to see details about processes\n");

  for(i = 0; i < nprocess; i++){
    ticket += 50;
    pid = fork(ticket);
    if(pid < 0)
    {
      printf(1, "%d failed in fork!\n", getpid());
      exit();
    }
    else if(pid == 0)
    {
      printf(1, "Child %d with %d tickets!\n", getpid(), ticket);
      for (z = 0; z < steps*4; z += 1) {
         // copy buffers one inside the other and back (*4)
         // used for wasting cpu time
         memmove(buffer_dst, buffer_src, 1024);
         memmove(buffer_src, buffer_dst, 1024);
      }
      printf(1, "Child %d with %d tickets is over\n", getpid(), ticket);
      exit();
    }
  }
  for(i = 0; i < nprocess; i++) wait(); //Wait for Children to exit

  exit();
}
