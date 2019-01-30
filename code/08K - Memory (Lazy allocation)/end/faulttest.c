#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MALLOC_NUMBER 4
#define MALLOC_SIZE 4096

int main(int argc, char **argv) {
  int i;
  char *buffer;

  for (i = 0; i < MALLOC_NUMBER; i++) {
    /* allocate memory */
    if ((buffer = (char *)malloc(MALLOC_SIZE)) < 0) {
      printf(2, "error: malloc()\n");
      exit();
    }
    
    /* access memory (writing) */
    memset(buffer, 0, MALLOC_SIZE);
  }
    
  exit();
}
