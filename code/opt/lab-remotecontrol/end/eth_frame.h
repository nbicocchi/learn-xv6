#ifndef ETH_FRAME
#define ETH_FRAME

#define EFSIZ(payload_siz) payload_siz + 1  

struct eframe{
    uint8_t eframe_delimiter; //10101011 define whether or not the frame is valid
    uint8_t payload[MAX_FRAME_SIZ]; //payload must be at least 12 bytes oth. the frame will not be sent, size > 1500 may cause problemss
};

void format_eframe(struct eframe *frame, uint8_t *payload, uint16_t p_siz);

#endif