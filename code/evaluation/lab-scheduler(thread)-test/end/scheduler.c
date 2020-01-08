#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#include "sdh.h"

void help(void);

int main(int argc, char *argv[])
{
  char* op; // utility operation
  int sid;  // id of scheduler
  int max;
  int csid; // current id of scheduler
  
  if (argc < 2 || argc > 3) {
      help();
      exit();
  } 

  op = argv[1];
  max = sizeof(schedulerName) / sizeof(char *);
  
  if(strcmp(op, "list") == 0) {
    // get list and status of all pre-installed scheduler
    if (argc != 2) {
        help();
        exit();
    } 
    csid = getscheduler();
    printf(1, "SID\tENABLED\tNAME\n");
    for(int i = 0; i < max; i++) {
      printf(1, "%d\t",i);
      if(csid == i) {
        printf(1, "Y\t");
      } else {
        printf(1, "N\t");
      }
      printf(1, "%s\n",schedulerName[i]);
    }
    printf(1, "\n");
    exit();
  } else if(strcmp(op, "set") == 0) {
    // set pre-installed scheduler
    if(argc == 2) {
      // From compiler
      sid = schedSelected;
    } else if (argc != 3) {
      help();
      exit();
    } else {
      sid = atoi(argv[2]);
    }
    if(sid < 0 || sid > max) {
      help();
      exit();
    }
    csid = setscheduler(sid);
    if(csid == -1) {
      printf(2, "Error from system call\n");
      exit();
    }
    printf(1, "Scheduler policy: %s - SID %d\n", schedulerName[csid], csid);
    if(sid != csid) {
      printf(2, "Error in set operation");
      exit();
    }
    exit();
  } else if(strcmp(op, "get") == 0) {
    // get current scheduler
    if (argc != 2) {
        help();
        exit();
    } 
    csid = getscheduler();
    printf(1, "Scheduler policy: %s - SID %d\n", schedulerName[csid], csid);
    exit();
  } else if (strcmp(op, "default") == 0){
    // get default scheduler from compiler
    if (argc != 2) {
        help();
        exit();
    } 
    sid = schedSelected;
    printf(1, "Scheduler default policy: %s - SID %d\n", schedulerName[sid], sid);
    exit();
  } else {
    help();
    exit();
  }
}

void help(void){
  printf(1, "\nUsage:\n\n" );
  printf(1, "scheduler get\n\tGet current policy (name) and id (sid).\n\n" );
  printf(1, "scheduler default\n\tGet default policy (name) and id (sid).\n\n" );
  printf(1, "scheduler list\n\tGet list of all pre-installed policies.\n\n" );
  printf(1, "scheduler set <sid>\n\tSet policy in runtime with id (sid).\n\n" );
  printf(1, "scheduler set\n\tSet default policy in runtime.\n\n" );
  return;
}
