#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#ifndef MAX_PRIORITY
#define MAX_PRIORITY 20
#endif

int main(int argc, char *argv[])
{
  int priority, pid;

  if (argc < 3) {
      printf(2, "Usage: nice pid priority\n" );
      exit();
  }
  pid = atoi(argv[1]);
  priority = atoi(argv[2]);
  if (priority < 0 || priority > MAX_PRIORITY) {
      printf(2, "Invalid priority (0-%d)!\n",MAX_PRIORITY);
      exit();
  }

  setpriority(pid, priority);

  exit();
}
