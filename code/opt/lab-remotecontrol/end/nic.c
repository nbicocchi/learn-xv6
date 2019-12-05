#include "nic.h"
#include "defs.h"

int get_device(char* interface, struct nic_device** nd) {
   if(nic_devices[0].send_packet == 0 || nic_devices[0].recv_packet == 0) {
     return -1;
   }
   *nd = &nic_devices[0];

   return 0;
}

void register_device(struct nic_device nd) {
  nic_devices[0] = nd;
}
