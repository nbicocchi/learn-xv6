#include "types.h"
#include "defs.h"

int sys_send(void) {
  char *json_payload;
  int size;

  if(argstr(0, &json_payload) < 0 || argint(1, &size) < 0) {
    cprintf("SYS_SEND ERROR WHILST FETCHING ARGS");
    return -1;
  }

  if(send_eth_frame(json_payload, size) < 0) {
    cprintf("ERROR WHILST EXECUTING SYS_SEND");
    return -1;
  }

  return 0;
}