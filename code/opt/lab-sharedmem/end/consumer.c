#include "types.h"
#include "user.h"
#include "stat.h"
#include "fcntl.h"
#define  PGSIZE 4096

int main(void){
	sleep(20);				//For synchronization (Waiting producer to write)
	int key = 0, num_pages=4;		//Key selected, number of pages to be allocated
	void* mem = shmgetat(key,num_pages);	//Obtaining memory access
	int communication;			//Store value read
        int* pointer = (int*) mem;              //Cast to the type of data you want to communicate
	int i;

	if(mem == 0) {
		printf(1,"Error in shmgetat, exit..\n");
		exit();
	}

	for(i = 0; i<num_pages; i++){
		communication=*pointer;
		printf(1,"%d: Read on mem value %d (Key %d)\n",getpid(),communication,key);
		mem += PGSIZE;
		pointer = (int*) mem;		
	}

	key = 1; num_pages=3;
	mem = shmgetat(key,num_pages);
	pointer = (int*) mem;
	for(; i<num_pages+4; i++){
		communication=*pointer;
		printf(1,"%d: Read on mem value %d (Key %d)\n",getpid(),communication,key);
		mem += PGSIZE;
		pointer = (int*) mem;		
	}

	exit();
}

