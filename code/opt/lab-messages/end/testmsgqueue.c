#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"


void proc_sender() {
    char *msg;
    char *rmsg[MSG_SIZE];
    msg = "HELLO ";
    sendmsg(0,msg,0);
    msg = "THERE";
    sendmsg(0,msg,0);
    msg = "!";
    sendmsg(0,msg,0);
    recvmsg(1,rmsg);
    printf(1,"S: received message: %s\n",rmsg);
}
void proc_receiver() {
    char *msg;
    char *rmsg[MSG_SIZE];
    recvmsg(0,rmsg);
    printf(1,"R: %s",rmsg);
    recvmsg(0,rmsg);
    printf(1,"%s",rmsg);
    recvmsg(0,rmsg);
    printf(1,"%s",rmsg);
    printf(1,"\nR: Received all the messages!\n");
    msg = "GENERAL KENOBI!";
    sendmsg(1,msg,0);
}


int main(int argc, char **argv) {
    int pid=fork();
    if(pid==0) {
        proc_sender();
        exit();
    }

    pid=fork();
    if(pid==0) {
        proc_receiver();
        exit();
    }

    wait();
    wait();
    printf(1,"Test Finished\n");
    exit();
}