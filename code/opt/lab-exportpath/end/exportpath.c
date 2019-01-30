#include "types.h"
#include "user.h"
#include "fcntl.h"

#define BUFSIZE 512

int
isequal(char* str1, char* str2)
{
    int flag = 1;
    int i = 0;

    for(; str1[i] != '\n'; i++) {
        if(str1[i] != str2[i])
            flag = 0;
    }
    if(str2[i] != 0)
        flag = 0;
    
    return flag;
}

int
main(int argc, char *argv[])
{   
    if(argc == 1 || argc > 2) {
        printf(1, "Usage: exportpath /path/to/dir/\n");
        exit();
    }

    int fd, i, flag = 1;
    int n, m;
    char buf[BUFSIZE];

    //check path lenght
    for(n = 0; argv[1][n] != 0; n++);

    fd = open("/.sh_profile", O_CREATE | O_RDWR);
    if(fd < 0) {
        printf(1, "error: open file failed\n");
        exit();
    }

    //put file content on a buffer
    m = read(fd, buf, BUFSIZE);
    buf[m] = 0;
   
    //check whether path is inserted already
    for(i = 0; buf[i] != 0; i++) {
        flag = isequal(buf + i, argv[1]);
        if(flag == 1)
            break;
        for(; buf[i] != '\n'; i++);
    }

    if(flag == 1 && buf[0] != 0) {
        close(fd);
        printf(1, "path already exported\n");
        exit();
    }

    //check if file dimension doesn't exceed BUFSIZE
    if(m + n >= BUFSIZE) {
        printf(1, "too many paths exported, remove some\n");
        exit();
    }

    //write path
    if(write(fd, argv[1], n) != n){
        printf(1, "error: write to file failed\n");
        exit();
    }

    //write \n
    if(write(fd, "\n", 1) != 1){
        printf(1, "error: write to file failed\n");
        exit();
    }

    close(fd);
    exit();
}