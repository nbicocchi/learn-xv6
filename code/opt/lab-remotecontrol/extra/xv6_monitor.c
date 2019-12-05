#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_PAYLOAD 1500
#define BUF_SIZ 1510

bool is_delimiter(char c){
    return (unsigned char)c == (unsigned char)0xae ? true : false;
}
bool is_terminator(char c){
    return (unsigned char)c == (unsigned char)0 ? true : false;
}

int main(int argc, char** argv){
    int sock_fd;
    system("tput civis"); //hide cursor because it's useless, to unhide use <tput cvvis> 
    printf("waiting for xv6 to boot up...\n");
    do {
        sleep(2);
        sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    } while(sock_fd<0);
    printf("connection established with xv6 via socket...\n");

    const char *iface = "tap0";
    const int len = strlen(iface);
    setsockopt(sock_fd, SOL_SOCKET, SO_BINDTODEVICE, iface, len);
    unsigned char buf [BUF_SIZ];
    char payload[MAX_PAYLOAD];

    while(true) {
        memset(buf, 0, BUF_SIZ);
        memset(payload, 0, MAX_PAYLOAD);
        struct sockaddr saddr;
        int sa_len = sizeof(saddr);
        int buf_len = recvfrom(sock_fd, buf, BUF_SIZ, 0, &saddr, (socklen_t*)&sa_len);
        if(buf_len < 0){
            printf("error in reading from socket\n");
            break;
        }
        unsigned char delimiter = buf[0];
        if(!is_delimiter(delimiter)){
            //frame not valid
            continue;
        }
        char c;
        int i = 1;
        do{
            c = buf[i];
            if(is_terminator(c))
                break;
            payload[i-1]=c;
            i++;
        }while(!is_terminator(c) && i < MAX_PAYLOAD);
        payload[i-1]=0;

        system("clear"); //clear the terminal befor each refresh
        printf("%s",payload);
    }
}
