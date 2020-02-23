#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "stddef.h"
#include "stdbool.h"

/* -------------------------------------------------------------VARIABLES--------------------------------------------------------------------*/
char buffer[2048];
int src_fd = 0;
int r = 0;
static char *input_file = "man_file";

/* ----------------------------------------------------------------MAIN----------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	//check parameter -> man dd
	if(argc < 2) 
	{
		printf(2, "Error command man. Use 'man dd'\n");
		exit();
	}

	/* INPUT FILE */
	//Check input_file not NULL
	if(input_file == NULL)	
	{
		printf(2,"Input file NULL\n");
		exit();
	}
	
	//Check and open input file
	if((src_fd = open(input_file, O_RDONLY)) < 0)	
	{
		printf(2, "dd: cannot open input file %s\n", input_file);
		exit();
	}
	
	printf(1,"\n");
	
	while((r = read(src_fd, buffer, sizeof(buffer))) > 0)
	{
		if(r == 0)
		{
		printf(1,"Error: impossible read man\n");
		}
	
		printf(1,"%s",buffer);

	}
	
	printf(1,"\n");
	close(src_fd);
	exit();
}
