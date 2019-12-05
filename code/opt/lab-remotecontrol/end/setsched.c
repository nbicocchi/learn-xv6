#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "user.h"


void print_usage(){
    char *usage = "CHANGE THE CURRENT SCHEDULING POLICY, ARGUMENT = NEW SCHEDULING POLICY\n\t- 0 = DEFAULT POLICY\
\n\t- 1 = PRIORITY\n\t- 2 = FCFS\n\t- 3 = CFS\n\t- 4 = SML\n";
    printf(2, "%s",usage);
    exit();
}
int main(int argc, char** argv){
    if(argc != 2){
        print_usage();
    }
    int new_policy = atoi(argv[1]);
    if(new_policy < 0 || new_policy > 4){
        print_usage();
    }
    if(setscheduler(new_policy) < 0){
        printf(2, "Error whilst changing scheduling policy\n");
        exit();
    }
    exit();
}