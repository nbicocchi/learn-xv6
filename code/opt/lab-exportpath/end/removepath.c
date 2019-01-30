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
        printf(1, "Usage: removepath /path/to/dir/\nor removepath -a to remove all\n");
        exit();
    }

    int fd, i, flag = 0;
    long unsigned int n;
    char buf[BUFSIZE];
    int del, end;


    if(argv[1][0] == '-' && argv[1][1] == 'a') {
        //delete the file to delete its content
        unlink("/.sh_profile"); 
	exit();  
    }

    //check path lenght
    for(n = 0; argv[1][n] != 0; n++);

    fd = open("/.sh_profile", O_RDWR);
    if(fd < 0) {
        printf(1, "open file failed\n");
        exit();
    }

    //put file content in a buffer
    buf[read(fd, buf, BUFSIZE)] = 0;
    close(fd);

    //delete the file to delete its content
    unlink("/.sh_profile");
   
    //check whether our argument is in the file
    for(i = 0; buf[i] != 0; i++) {
        flag = isequal(buf + i, argv[1]);
        if(flag == 1)
            del = i;
        for(; buf[i] != '\n'; i++);
        end = i + 1;
        if(flag == 1)
            break;
    }

    if(flag == 1) {
        //rewrite buffer without it
        for(; buf[end] != 0; del++, end++) {
            buf[del] = buf[end];
        }
        for(; buf[del] != 0; del++)
            buf[del] = 0;
        for(i = 0; buf[i] != 0; i++);

        //create new file
        fd = open("/.sh_profile", O_CREATE | O_RDWR);
        if(fd < 0) {
            printf(1, "error: create file failed\n");
            exit();
        }
        
        //write buffer on file
        if(write(fd, buf, i) != i){
        printf(1, "error: write to file failed\n");
        exit();
    }
        close(fd);
    }
    else {
        exit();
    }
    exit();
}
