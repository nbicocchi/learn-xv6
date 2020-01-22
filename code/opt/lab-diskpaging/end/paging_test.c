#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"

int main() {
  
  int fd= 0;
  int numberwrite = 100;
  char buf;
  int i=0;
  struct proc_us ptable[NPROC];
  struct proc_us *p;

  if (getptable(NPROC, NPROC * sizeof(struct proc_us), &ptable) < 0) {
    printf(1, "Error getting ptable");
    exit();
  }

  fd = open("testFile", O_CREATE | O_WRONLY);
  if(fd < 0){
    printf(2, "error: cannot open testFile for writing\n");
    exit();
  }


 
  printf(1, "Writing");
  for(int j = 0; j<numberwrite;j++){
    int cc = write(fd, "hello", 4);
	    if(cc <= 0){
		printf(2, "error\n");
	       break;
	      }
   if(j%20 == 0){
		 printf(1, ".");
 				}
 
  }
  
  close(fd);
  printf(1,"\n");
  

  fd = open("testFile", O_RDONLY);
  if(fd < 0){
    printf(2, "error: cannot re-open testFile for reading\n");
    exit();
  }

   printf(1, "Reading");
   while (read(fd, &buf, sizeof(buf)>0)){
	i=i+1;
    	if(i%60==0){
			printf(1, ".");
				}
  }
 
  close(fd);
  printf(1,"\n");


  
  for(p = ptable; p != &ptable[NPROC-1]; p++) {  
	if(strcmp(p->name,"paging_test")==0){    
       		 printf(1, "Execution time: %d ms", p->rutime*1000);    
  		}
	}

  printf(1, "\n");
  exit();
}
