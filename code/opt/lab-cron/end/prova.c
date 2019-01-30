#include "types.h"
#include "stat.h"
#include "user.h"

int main (int argc, char *argv[]){
    int a=0;
    for (a = 0; a < 1000000; a++);
    printf(2, "%d\n", a);
    exit();
}
