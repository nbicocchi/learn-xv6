#include "types.h"
#include "defs.h"
#include "nic.h"
#include "eth_frame.h"

int send_eth_frame(char *payload, uint16_t p_siz){
    struct nic_device *nd;
    if(get_device("mynet0", &nd) < 0) {
        cprintf("ERROR [ send_eth_frame ] --> Device not loaded\n");
        return -1;
    }
    /*
    *   TODO: se size > 1500 allora inviare più frame: terminatore = fe -> continua, terminatore = ff -> fine dei frames 
    * */
    struct eframe frame;
    format_eframe(&frame, (uint8_t *)payload, p_siz);
    nd->send_packet(nd->driver, (uint8_t*)&frame, EFSIZ(p_siz));
    return 0;
}

void log_system_infos(){
    log_proc_infos();
}