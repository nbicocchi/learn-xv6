#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

int main(int argc, char *argv[]){  
  printf(1, "pid=%d, ppid=%d\n", getpid(), getppid());
  exit();
}
