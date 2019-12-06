#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "param.h"

int main(int argc, char *argv[])
{
  int pid, tickets;

  if (argc < 3) {
      printf(2, "Usage: ticket pid numtickets\n" );
      exit();
  }

  pid = atoi(argv[1]);
  tickets = atoi(argv[2]);
  
  if (tickets <= 0 || tickets > MAX_TICKETS) {
      printf(2, "Tickets must be greater than zero and less than %d!\n", MAX_TICKETS +1);
      exit();
  }

  settickets(pid, tickets);

  exit();
}
