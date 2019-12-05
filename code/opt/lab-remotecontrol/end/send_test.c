#include "types.h"
#include "user.h"

int main(int argc, char** argv){
    char message_2_send[1500];
    if(argc == 3 && strcmp("-s", argv[1]) == 0){
        memmove(message_2_send, argv[2], strlen(argv[2]));
    }
    else{
        memmove(message_2_send,"l'acqua è un brodo di pollo!!!",32);
        printf(1,"Usage is: [send_test -s <some text>]\nSending default string...\n");
    }
    if(send(message_2_send, strlen(message_2_send) + 1) < 0){
        printf(2, "Error whilst sending data\n");
        exit();
    }
    exit();
}