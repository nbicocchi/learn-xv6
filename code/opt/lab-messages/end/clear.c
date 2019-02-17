#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#include "mmu.h"


int main(int argc, char **argv) {
    if(argc > 1) {
        int val = atoi(argv[1]);
        if(val > 0) {
            for(int i=0; i<val; i++) {
                printf(1,"\n");
            }
            exit();
        }
        //if val == 0 restore to default, because atoi failed (or the string was '0')
    }
    for(int i=0; i<100; i++) {
        printf(1,"\n");
    }
    exit();
}