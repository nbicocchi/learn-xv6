/*
 * Author: antipatico (https://github.com/antipatico)
 * License: Apache 2
 * Year: 2020
 */
#include "types.h"
#include "user.h"
#include "stat.h"
#include "fcntl.h"
#include "elf.h"

#define NULL 0
#define ELF_SIZE 64

struct secthdr {
  uint name;
  uint type;
  uint flags;
  uint addr;
  uint offset;
  uint size;
  uint link;
  uint info;
  uint addraling;
  uint entsize;
};


char* read_file(char *name, uint *num);
char* extract_section(char* exe, uint size, char* name, uint *n);
void invalid_elf();

int main(int argc, char*argv[]) {
  char *name, *exe, *section;
  uint num, size;

  if (argc < 2) {
    printf(2,"USAGE: %s BINARY_NAME [SECTION_NAME]\n",argv[0]);
    exit();
  }
  if (argc > 2) {
    name = argv[2];
  } else {
    name = ".text";
  }
  exe = read_file(argv[1], &num);
  section = extract_section(exe, num, name, &size);
  free(exe);
  if(section == NULL) {
    printf(2,"ERROR: section not found in file.\n");
    exit();
  }
  for(int i = 0; i < size; i++)
    printf(1, "%c", section[i]);

  free(section);
  exit();
}

void invalid_elf() {
  printf(2, "ERROR: input file is not a valid ELF.\n");
  exit();
}

char* extract_section(char* exe, uint size, char* sname, uint* n) {
  struct elfhdr *elf;
  struct secthdr *sect;
  char *buf, *strtable;

  buf = NULL;
  if (size < ELF_SIZE)
    invalid_elf();
  elf = (struct elfhdr*)exe;
  if (elf->magic != ELF_MAGIC)
    invalid_elf();
  sect = (struct secthdr*)(exe+(elf->shstrndx * elf->shentsize)+elf->shoff);
  strtable = exe+(sect->offset);
  for(int i = 0; i < elf->shnum; i++) {
    sect = (struct secthdr*)(exe+elf->shoff+(elf->shentsize*i));
    if(strcmp(sname,strtable+sect->name) == 0) {
        buf = (char*)malloc(sizeof(char)*sect->size);
        memcpy(buf, exe+sect->offset, sect->size);
        *n = sect->size;
    }
  }
  return buf;
}

char* read_file(char *name, uint *num)
{
	int fd ;
	char *buffer;
  struct stat st;
  

	fd = open(name, O_RDONLY) ;
  if(fd < 0) {
    printf(2, "Open file error!\n");
    exit();
  }
  fstat(fd, &st);
	*num = st.size;
	buffer = (char*) malloc(sizeof(char) * (*num)) ;
	if (buffer == NULL)
	{
		printf(2, "Memory error!\n") ;
		exit() ;
	}
	long result = read(fd, buffer, *num) ;
	if (result != *num)
	{
		printf(2, "File read error!\n") ;
		exit() ;
	}
  close(fd);
	return buffer; 
}
