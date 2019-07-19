#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"


void proc_sender() {

    char* msg;
    msg = "HELLO THERE!";
    printf(1,"S: Sending a message\n");
    sendmsg(1,msg,1);
    printf(1,"S: Received ack\n");
}

void proc_receiver() {
    char rmsg[255];
    int i = 0;
    //to waste some time count to 50millions
    while(i<1000000000) {i++;}
    printf(1,"R: finished wasting my time\n");
    recvmsg(1,rmsg);
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