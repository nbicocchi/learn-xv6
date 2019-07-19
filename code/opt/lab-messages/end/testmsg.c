#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"


void proc_sender() {

    char* msg;
    char rmsg[255];
    msg = "HELLO THERE!";
    sendmsg(1,msg,0);
    printf(1,"S: sent message\n");
    recvmsg(2,rmsg);
    printf(1,"S: received message: %s\n",rmsg);
}

void proc_receiver() {
    char rmsg[255];
    recvmsg(1,rmsg);
    printf(1,"R: received message: %s\n",rmsg);
    char* msg;
    msg = "GENERAL KENOBI.";
    sendmsg(2,msg,0);
    printf(1,"R: sent answer\n");
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