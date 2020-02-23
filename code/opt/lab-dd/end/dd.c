#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "stddef.h"
#include "stdbool.h"

//if the input block size of bs is not specified the default number of bytes = 512
#define DEFAULT_BYTES 512

/* ------------------------------------------------------------VARIABLES-----------------------------------------------------------------*/
//Variables for file
static char *input_file = NULL;
static char *output_file = NULL;
int r = 0;
int w = 0;
int r_temp = 0;
int count_file_bytes = 0;
int x = 0;
int inizio,fine;
int tempo;
char *buffer[1024];
char buffer_temp[1024];

//Variables for dd command
int input_output_bytes = 0;		
int count = 0;
int total_count = 0;
int conv = 0;				//1 if a conversion is required, 0 otherwise
int conv_case = 0;			//specify the type o conversione required (lcase/ucase)
int input_partial_block = 0;
int output_partial_block = 0;
/* -----------------------------------------------------FUNCTION CHECK PARAMETERS--------------------------------------------------------*/

//Parameter is in the form X=...
//Boolean function that return true(1) if X is = name, otherwise false(0)
bool check_operand(char *parameter, char *name)
{
	while(*name != '\0'){
		if(*parameter++ != *name++) return false;
	}
	return (!*parameter || *parameter == '=');
}

//Boolean function that return true(1) if lcase/ucase is contained in value
bool check_conv(char *value, char *name)
{
	while(*name != '\0'){
		if(*value++ != *name++)	return false;	
	}
	return true;
}

/* -----------------------------------------------------------bs only-----------------------------------------------------------------*/
void bs_only(int src_fd, int dest_fd)
{
					//calculate the initial time of the function
					inizio = uptime();
				
					//Calculate the total bytes of the file
					while((r_temp = read(src_fd, buffer_temp, 1)) > 0)
					{	
						//count_file_bytes = number of bytes of the input_file
						count_file_bytes++;

					}

					printf(1,"File total lenght: %d bytes\n", count_file_bytes);
					close(src_fd);

					//Check and open input file
					if((src_fd = open(input_file, O_RDONLY)) < 0)
					{
						printf(2, "dd: cannot open input file %s\n", input_file);
						exit();
					}
				
					//read from the source file the bytes that must be copy
					while((r = read(src_fd, buffer, input_output_bytes)) > 0)
					{
						//Check that the bytes that i have to copy is < of the lenght of the file
						if(input_output_bytes > count_file_bytes)	
						{
							printf(1,"The number of bytes entered by input is bigger than the number of bytes of the file -> dd will copy all the file content\n");	
						}
				
						//write to the destination file the number of bytes entered by input with bs
						w = write(dest_fd, buffer, r);

						x = w + x;	

						if(w != r || w < 0)	break;

					}/*close while*/

					//calculate the final time of the function
					fine=uptime();
					
					//calculate the total time of the function	
					tempo = (fine - inizio);

					//input partial block
					if((count_file_bytes % input_output_bytes)!=0)	input_partial_block = 1; 
					else input_partial_block = 0;

					//output partial block
					if((x % input_output_bytes) != 0)  output_partial_block = 1;
					else output_partial_block = 0;

					printf(1,"record in: %d+%d\nrecord out: %d+%d\n(%d bytes / %d bytes) copied in %d,%d s\n",(count_file_bytes/input_output_bytes),input_partial_block,(x / input_output_bytes),output_partial_block ,count_file_bytes, count_file_bytes, tempo/100,tempo%100);

}/*close bs only */

/* -----------------------------------------------------------bs count-----------------------------------------------------------------*/
void bs_count(int src_fd, int dest_fd)
{
					//calculate the initial time of the function
			inizio = uptime();

			//copy only the number of bs bytes * number of block from the source file to the destination file
			//(count = blocks) * (input_output_bytes = bytes) -> count = total number of bytes 
			total_count = count * input_output_bytes;		

			//Support print
			printf(1,"Number of the block to copy: %d\n", count);
			printf(1,"Size of each block that must be copy: %d bytes\n", input_output_bytes);
			printf(1,"Total size that must be copy: %d bytes\n", total_count);
			
			//Calculate the total bytes of the file
			while((r_temp = read(src_fd, buffer_temp, 1)) > 0)
			{	
				//count_file_bytes = number of bytes of the input_file
				count_file_bytes++;
			}	
			printf(1,"File total lenght: %d bytes\n", count_file_bytes);

			close(src_fd);

			//Check and open input file
			if((src_fd = open(input_file, O_RDONLY)) < 0)	
			{
				printf(2, "dd: cannot open input file %s\n", input_file);
				exit();
			}
			
			//read from the source file the number of bytes entered by input with bs and count
			r = read(src_fd, buffer, total_count);
			
			//write to the destination file the number of bytes entered by input with bs and count
			w= write(dest_fd, buffer, r);		
				
			//Check that the bytes that i have to copy is < of the lenght of the file
			if(total_count > count_file_bytes)	
			{
				total_count = count_file_bytes;	
				printf(1,"The number of bytes entered by input is bigger than the number of bytes of the file -> dd will copy all the file content\n");					
			}
			
			//calculate the final time of the function
			fine=uptime();
				
			//calculate the total time of the function	
			tempo = (fine - inizio);
				
			//input partial block
			if((total_count % input_output_bytes)!=0)	input_partial_block = 1; 
			else input_partial_block = 0;

			//output partial block
			if((w % input_output_bytes) != 0)  output_partial_block = 1;
			else output_partial_block = 0;

			printf(1,"record in: %d+%d\nrecord out: %d+%d\n(%d bytes / %d bytes) copied in %d,%d s\n",(total_count/input_output_bytes),input_partial_block,(w / input_output_bytes),output_partial_block ,total_count, count_file_bytes, tempo/100,tempo%100);	

}/*close bs count*/

/* -----------------------------------------------------------bs conv-----------------------------------------------------------------*/
void bs_conv(int src_fd, int dest_fd)
{
				//calculate the initial time of the function
			inizio = uptime();
			
			char tt[input_output_bytes];

			//Calculate the total bytes of the file
			while((r_temp = read(src_fd, buffer_temp, 1)) > 0)
			{	
				//count_file_bytes = number of bytes of the input_file
				count_file_bytes++;
			}	

			printf(1,"File total lenght: %d bytes\n", count_file_bytes);
			close(src_fd);

			//Check and open input file
			if((src_fd = open(input_file, O_RDONLY)) < 0)	
			{
				printf(2, "dd: cannot open input file %s\n", input_file);
				exit();
			}

			//Check that the bytes that i have to copy is < of the lenght of the file
			if(input_output_bytes > count_file_bytes)	
			{
				input_output_bytes = count_file_bytes;	
				printf(1,"The number of bytes entered by input is bigger than the number of bytes of the file -> dd will copy all the file content\n");					
			}

			switch(conv_case)
			{
				case(0):	//conversion from Uppercase to Lowercase -> lcase
				{	
					while(( r= read(src_fd, buffer_temp, input_output_bytes)) > 0)
					{	
						for(int i=0;i< input_output_bytes;i++)
						{
							if((buffer_temp[i] >='A') && (buffer_temp[i]<= 'Z'))
							{
								buffer_temp[i] = buffer_temp[i] + 32;
							}	
							tt[i] = buffer_temp[i];
						}

						w =write(dest_fd, tt, r);

						x = w + x; 

						if(w!=r || w<0)	break;
					}	

				}
				case(1):	//Conversion from Lowercase to Uppercase -> ucase
				{
					while(( r= read(src_fd, buffer_temp, input_output_bytes)) > 0)
					{	
						for(int i=0;i< input_output_bytes;i++)
						{
							if((buffer_temp[i] >='a') && (buffer_temp[i]<= 'z'))
							{
								buffer_temp[i] = buffer_temp[i] - 32;
							}	
							tt[i] = buffer_temp[i];
						}

						w =write(dest_fd, tt, r);

						x = w + x;

						if(w!=r || w<0)	break;
					}

				}
			
			}/*close switch*/

				//calculate the final time of the function
				fine=uptime();
				
				//calculate the total time of the function	
				tempo = (fine - inizio);

				//input partial block
				if((count_file_bytes % input_output_bytes)!=0)	input_partial_block = 1; 
				else input_partial_block = 0;

				//output partial block
				if((x % input_output_bytes) != 0)  output_partial_block = 1;
				else output_partial_block = 0;

			
			printf(1,"record in: %d+%d\nrecord out: %d+%d\n(%d bytes / %d bytes) copied in %d,%d s \n",(count_file_bytes/input_output_bytes),input_partial_block,(x / input_output_bytes),output_partial_block ,count_file_bytes, count_file_bytes, tempo/100, tempo%100);	


}
/* ---------------------------------------------------------bs count conv---------------------------------------------------------------*/
void bs_count_conv(int src_fd, int dest_fd)
{
				//calculate the initial time of the function
			inizio = uptime();

			//copy only the number of bs bytes * number of block from the source file to the destination file
			//(count = blocks) * (input_output_bytes = bytes) -> count = total number of bytes 
			total_count = count * input_output_bytes;		

			//Support print
			printf(1,"Number of the block to copy: %d\n", count);
			printf(1,"Size of each block that must be copy: %d bytes\n", input_output_bytes);
			printf(1,"Total size that must be copy: %d bytes\n", total_count);
			

			char tt[total_count];

			//Calculate the total bytes of the file
			while((r_temp = read(src_fd, buffer_temp, 1)) > 0)
			{	
				//count_file_bytes = number of bytes of the input_file
				count_file_bytes++;	
			}	

			printf(1,"File total lenght: %d bytes\n", count_file_bytes);
			close(src_fd);

			//Check and open input file
			if((src_fd = open(input_file, O_RDONLY)) < 0)	
			{
				printf(2, "dd: cannot open input file %s\n", input_file);
				exit();
			}
			
			//Check that the bytes that i have to copy is < of the lenght of the file
			if(total_count > count_file_bytes)	
			{
				total_count = count_file_bytes;	
				printf(1,"The number of bytes entered by input is bigger than the number of bytes of the file -> dd will copy all the file content\n");					
			}

			switch(conv_case)
			{
				case(0):	//conversion from Uppercase to Lowercase -> lcase
				{	
					r= read(src_fd, buffer_temp, total_count);

						for(int i=0;i< total_count;i++)
						{
							if((buffer_temp[i] >='A') && (buffer_temp[i]<= 'Z'))
							{
								buffer_temp[i] = buffer_temp[i] + 32;
							}	
							tt[i] = buffer_temp[i];
							
						}

						w = write(dest_fd, tt, r);
						break;
				}
			
				case(1):	//Conversion from Lowercase to Uppercase -> ucase
				{
					r= read(src_fd, buffer_temp, total_count);
	
						for(int i=0;i< total_count;i++)
						{
							if((buffer_temp[i] >='a') && (buffer_temp[i]<= 'z'))
							{
								buffer_temp[i] = buffer_temp[i] - 32;
							}	
							tt[i] = buffer_temp[i];
						}

						w =write(dest_fd, tt, r);
						break;
				}
			}/*close switch*/ 
				
				//calculate the final time of the function
				fine=uptime();
				
				//calculate the total time of the function	
				tempo = (fine - inizio);

				//input partial block
				if((total_count % input_output_bytes)!=0)	input_partial_block = 1; 
				else input_partial_block = 0;

				//output partial block
				if((w % input_output_bytes) != 0)  output_partial_block = 1;
				else output_partial_block = 0;

			printf(1,"record in: %d+%d\nrecord out: %d+%d\n(%d bytes / %d bytes) copied in %d,%d s\n",(total_count/input_output_bytes),input_partial_block,(w / input_output_bytes),output_partial_block ,total_count, count_file_bytes, tempo/100, tempo%100);	
}
/* -----------------------------------------------------------FUNCTION DECODE------------------------------------------------------------*/
//Function that decode arguments
void decode_arguments(int argc, char *argv[])
{	
	
	for(int i = 1;i < argc; i++)
	{

	char *name = argv[i];
	char *value = strchr(name, '=');

		//check the case in which there is not '='
		if (value == NULL)	{
		printf(2,"It does not follow the sintax name=value\n");
		exit();
		}

		value++;
		
		//name = if -> input file
		if(check_operand(name,"if") == 1)	input_file = value;
	
		//name = of -> output file
		else if(check_operand(name,"of") == 1)	output_file = value;
		
		//name = conv
		//conv converts the input file according to the option entered by the user
		else if(check_operand(name,"conv") == 1) 
		{	
			conv = 1;

			if(check_conv(value,"lcase") == 1)
			{
				printf(1,"Uppercase to Lowercase conversion\n");
				conv_case = 0;
			}

			if(check_conv(value,"ucase") == 1)	
			{
				printf(1, "Lowercase to Uppercase conversion\n"); 
				conv_case = 1;
			}
			
			if((check_conv(value,"lcase") == 0) && (check_conv(value,"ucase") == 0))  
			{
				printf(1,"Unrecognized type of conversion\n");
				exit();
			}
		}

		else {
			
			int n_block_bytes_input = atoi(value);	//conversion from char to int
			
			if(atoi(value) <= 0)	{
				printf(2,"Invalid block size\n");
				exit();
			}

			//name = bs 
			//bs reads and writes BYTES bytes at a time -> bs = BYTES
			if(check_operand(name,"bs") == 1)	{
			input_output_bytes = n_block_bytes_input;
			//printf(2, "%d\n", input_output_bytes);
			}
						
			//name = count
			//count copies only BLOCKS input blocks -> count = BLOCKS (used with bs)
			else if(check_operand(name,"count") == 1){ count = n_block_bytes_input;}
			
			else printf(2, "Unrecognized operand\n");
		}
		
	}/*close for*/

		//if bs in not entered default = 512 bytes-> if of conv || if of count || if of
		if(((input_output_bytes == 0) && (count != 0)) || 
		   ((input_output_bytes == 0) && (conv != 0))  ||
		   ((input_output_bytes == 0) && (conv == 0) && (count == 0)))
		{	
			if((input_file != NULL) && (output_file != NULL))
			{
				input_output_bytes = DEFAULT_BYTES;
				printf(1, "The option bs is missing, dd considers the default value bs = 512 bytes\n"); 
			}
		}
}/*close decode_arguments*/

/*--------------------------------------------------------------- MAIN ------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	
	int src_fd,dest_fd;

	//Check the minimum number of parameters
	if(argc <= 1)	{
		printf(2,"Invalid number of parameters, use 'man dd' to see the syntax\n");
		exit();
	}
	
	decode_arguments(argc,argv);
	
	//Check if the input file or output file is NULL
	if((input_file == NULL || output_file == NULL))
	{
		printf(2, "The input file or output file is NULL\n");
		exit();
	}	

	//Check and open input file
	if((src_fd = open(input_file, O_RDONLY)) < 0)	
	{
		printf(2, "dd: cannot open input file %s\n", input_file);
		exit();
	}
	
	//Check and open output file
	if((dest_fd = open(output_file, O_CREATE | O_WRONLY)) < 0)
	{
		printf(2, "dd: cannot open output file %s\n", output_file);
		exit();
	}


/*------------------------------------------------------------------bs-------------------------------------------------------------------*/
	if(input_output_bytes != 0)
	{
/*---------------------------------------------------------------bs only-----------------------------------------------------------------*/
		if((count == 0) && (conv == 0))	
		{	
			bs_only(src_fd, dest_fd);
		}
/*---------------------------------------------------------------bs count----------------------------------------------------------------*/
		if((count != 0) && (conv == 0))	
		{	
			bs_count(src_fd,dest_fd);	
		}

/*---------------------------------------------------------------bs conv ----------------------------------------------------------------*/
		if((conv == 1) && (count == 0))
		{	
			bs_conv(src_fd,dest_fd);
		}
/*---------------------------------------------------------------bs count conv ----------------------------------------------------------*/
		if((conv == 1) && (count != 0))
		{	
			bs_count_conv(src_fd,dest_fd);
		}

	}/*close bs*/

	close(src_fd);
	close(dest_fd);
	exit();

}/*close main*/
