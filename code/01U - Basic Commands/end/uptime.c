#include "types.h"
#include "user.h"

int main(int argc, char *argv[])
{
    int ut;

    // returns uptime in 1/100 second
    ut = uptime();

    printf(1, "xv6 up since %ds.\n", ut / 100);
    exit();
}
