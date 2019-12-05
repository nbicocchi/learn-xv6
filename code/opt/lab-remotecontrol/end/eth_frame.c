#include "types.h"
#include "defs.h"
#include "eth_frame.h"


void format_eframe(struct eframe *frame, uint8_t *payload, uint16_t p_siz){
    frame->eframe_delimiter=(uint8_t)0xae;
    memmove(frame->payload, payload, p_siz);
}