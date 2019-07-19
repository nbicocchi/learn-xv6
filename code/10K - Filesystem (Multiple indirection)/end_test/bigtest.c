#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

int main() {
  char buf[512];
  int fd, i, sectors = 0;

  fd = open("big.file", O_CREATE | O_WRONLY);
  if(fd < 0){
    printf(2, "error: cannot open big.file for writing\n");
    exit();
  }

  memset(buf, 0, sizeof(buf));

  /* writing section */
  printf(1, "Writing");
  while(1) {
    *(int*)buf = sectors;
    int cc = write(fd, buf, sizeof(buf));
    if(cc <= 0) break;

    if (sectors++ % 10 == 0) printf(1, ".");
  }
  printf(1, " %d sectors [write]\n", sectors);
  close(fd);

  /* reading section */
  fd = open("big.file", O_RDONLY);
  if(fd < 0){
    printf(2, "error: cannot re-open big.file for reading\n");
    exit();
  }

  printf(1, "Reading");
  for(i = 0; i < sectors; i++){
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
    if (i % 10 == 0) printf(1, ".");
  }
  printf(1, " %d sectors [read]\n", i);
  close(fd);

  exit();
}
