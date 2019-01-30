// init: The initial user-level program

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char *argv[] = { "sh", 0 };
char *argv_c[] = { "cron", "&" };

int
main(void)
{
  int pid, pid_cron, wpid;

  if(open("console", O_RDWR) < 0){
    mknod("console", 1, 1);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  pid_cron = fork();
  if (pid_cron == 0){
    exec("cron", argv_c);
    printf(1, "init: cron failed\n");
    exit();
  } else if (pid_cron > 0){
    for(;;){
      printf(1, "init: starting sh\n");
      pid = fork();
      if(pid < 0){
        printf(1, "init: fork failed\n");
        exit();
      }
      if(pid == 0){
        exec("sh", argv);
        printf(1, "init: exec sh failed\n");
        exit();
      }
      while((wpid=wait()) >= 0 && wpid != pid)
        printf(1, "zombie!\n");
    }
  } else {
    printf(1, "init: cron fork failed");
    exit();
  } 
}
