#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"
#include "param.h"

int main() {
  int i, used_blocks=0, free_blocks=0;
  struct diskstats ds;

  /* show stats about fs using getfsstats() syscall */
  getfsstats(&ds);

  for (i = 0; i < ds.size; i++) {
    if (i != 0 && i % 50 == 0) 
      printf(1, "\n");
    
    if (ds.diskmap[i] == 1) {
      printf(1, "*");
      used_blocks++;
    } else {
      printf(1, "_");
      free_blocks++;
    }
  } 
  printf(1, "\n\n");

  printf(1, "total bytes:\t%dK\n", ds.size * BSIZE / 1024);
  printf(1, "data bytes:\t%dK\n", ds.nblock * BSIZE / 1024);
  printf(1, "used bytes:\t%dK\n", used_blocks * BSIZE / 1024);
  printf(1, "free bytes:\t%dK\n", free_blocks * BSIZE / 1024);

  printf(1, "master block:\t%d\n", 0);
  printf(1, "super block:\t%d\n", 1);
  printf(1, "log blocks:\t%d - %d\n", ds.logstart, ds.logstart + ds.nlog - 1);
  printf(1, "inode blocks:\t%d - %d\n", 
    ds.inodestart, 
    ds.inodestart + ds.ninode / (BSIZE / sizeof(struct dinode)));
  printf(1, "bmap blocks:\t%d - %d\n", ds.bmapstart, ds.bmapstart + ds.nbmap - 1);
  printf(1, "data blocks:\t%d - %d\n\n", ds.blockstart, ds.blockstart + ds.nblock - 1); 

  /* end stats */  
  char buf[512];
  int fd, blocks = 0;   

  fd = open("big.file", O_CREATE | O_WRONLY);
  if(fd < 0){
    printf(2, "error: cannot open big.file for writing\n");
    exit();
  }

  memset(buf, 0, sizeof(buf));

  /* writing section */
  printf(1, "Write |");
  while(1) {
    *(int*)buf = blocks;
    int cc = write(fd, buf, sizeof(buf));
    if(cc <= 0) break;

    if (blocks++ % 10 == 0) printf(1, "w");
  }
  printf(1, "| blocks=%d, bytes=%dK\n", blocks, blocks*BSIZE/1024);
  close(fd);

  /* reading section */
  fd = open("big.file", O_RDONLY);
  if(fd < 0){
    printf(2, "error: cannot re-open big.file for reading\n");
    exit();
  }

  printf(1, "Verify|");
  for(i = 0; i < blocks; i++){
    int cc = read(fd, buf, sizeof(buf));
    if(cc <= 0){
      printf(2, "error: read error at sector %d\n", i);
      exit();
    }
    /* check content */
    if(*(int*)buf != i){
      printf(2, "error: read the wrong data (%d) for sector %d\n", *(int*)buf, i);
      exit();
    }
    if (i % 10 == 0) printf(1, "v");
  }
  printf(1, "| blocks=%d, bytes=%dK\n", blocks, blocks*BSIZE/1024);
  close(fd);

  exit();
}
