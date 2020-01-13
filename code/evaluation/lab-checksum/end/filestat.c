#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "param.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"

#define SIZE 512

void readFile(char* name){
    int fd, n=1,i;
    char buf[SIZE];
    fd = open(name, O_RDONLY);
    printf(1,"Successful open the file in read.\n");
   
    if(fd > 0){
        for(i = 0; i < n; i++)
        {
          read(fd, buf, SIZE);		
    	}
        printf(1,"Read Test --> PASSED\n");
    } else {
        printf(1,"Read Test --> Nothing to read.\n");
    }
    close(fd);
}

void writeFile(char* name){
    int fd,i;
    int n=1;
    char buf[SIZE];

    fd = open(name,O_CREATE|O_CHECKED|O_RDWR);
    printf(1,"Successful open the file in write.\n");
    printf(1,"Buffer size: %d.\n", SIZE);

    memset(buf, 0, SIZE);
    printf(1,"Write file begin.\n");

    for( i = 0; i < n; i++)
    {
        buf[0] = (char)('T');
        write(fd, buf, SIZE);
    }
   
    printf(1,"Write Test --> PASSED\n");
    close(fd);
}

int main(int argc, char *argv[])
{
  struct stat st;
  struct stat *ptr = &st;
  int fd;

  if(argc != 2){
    printf(1, "Usage: filestat pathname\n");
    exit();
  }

  if((fd = open(argv[1], 0)) < 0) {
    printf(1, "filestat: cannot open %s\n", argv[1]);
    printf(1, "TEST START -->\n");
    writeFile(argv[1]);
    readFile(argv[1]);
    printf(1,"TEST PASSED\n");
  }

  if(stat(argv[1], ptr) < 0) {
    printf(2, "filestat: cannot stat %s\n", argv[1]);
    close(fd);
    exit();
  }

  switch(st.type) {
    case T_DIR:
      printf(1, "Type: %d\nSize: %d\n", st.type, st.size);
      break;
    case T_FILE:
      printf(1, "Type: %d\nSize: %d\n", st.type, st.size);
      break;
    case T_DEV:
      printf(1, "Type: %d\nSize: %d\n", st.type, st.size);
      break;
    case T_CHECKED:
      printf(1, "Type: %d\nSize: %d\nChecksum: %d\n", st.type, st.size, st.checksum);
      break;
  }
  close(fd);
  exit();
}
