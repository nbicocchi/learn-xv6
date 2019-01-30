#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#define MAX_BUF 512




enum bool {false,true};

/* Function: print_version
 * --------------
 * Function to print the helper of the command
 */
void print_help(){
	printf(1, "Usage: sort [OPTION]... [FILE]...\nSort the lines of the FILE passed as parameter or from standard input in lexicografical order\nMore than one FILE can be specified as input, and as options\nPossible arguments are:\n\t-c\t\tprint the first line out of order;\n\t-n\t\tprint in numerical order the strings;\n\t-u\t\tprint only the unique elements;\n\t-o\t\twrite to the specified file;\n\t-f\t\tcase insensitive;\n\t-r\t\treverse;\n");
	exit();
}

/* Function: print_version
 * --------------
 * Function to print the current version of the command
 */
void print_version(){
	printf(1, "sort for xv6 1.0\nWritten by Eleonora Macchioni\n");
	exit();
}
/* Function: bubbleSort
 * --------------
 * Function to sort the passed lines using bubblesort algorithm.
 * Accepts numeric or not numeric values.
 * **v: the text to be organized
 * dim: how many lines in the text
 * numeric: specify if the strings should be converted to numeric values (int)
 */
void bubbleSort(char **v,int dim,enum bool numeric){
	int i=0;

	enum bool ordinato=false;
	while(dim>1 && !ordinato){
		ordinato=true;
		for(i=0;i<dim-1;i++)
			if((!numeric && strcmp(v[i],v[i+1])>0) || (numeric && atoi(v[i])>atoi(v[i+1]))){
				char tmp[255];
				strcpy(tmp,v[i]); 
				v[i]=malloc(sizeof(char)*(strlen(v[i+1])));

				strcpy(v[i],v[i+1]);
				v[i+1]=malloc(sizeof(char)*strlen(tmp));
				strcpy(v[i+1],tmp);

				ordinato=false;}
		dim--;
	}


}
/* Function: toLower
 * --------------
 * converts the string passed to lowercase
 * s: the string to convert
 */
void toLower(char *s){
	int i;
	for(i=0;i<strlen(s);i++)
		if(s[i]>='A' && s[i]<='Z')
			s[i]+=32;
}
/* Function: unique
 * --------------
 * Removes the duplicate from the file and returns the new dimension (in line) of the text
 * **v: the text
 * dim: the input dimension of the text
 * return dim: the returned dimension of the text
 */
int unique(char **v,int dim){
	dim--;
	for(int i=0;i<dim;i++){
		for(int j=i+1;j<dim;){
			if(strcmp(v[j],v[i])==0){

				for(int k=j;k<dim;k++){
					v[k]=malloc(sizeof(char)*strlen(v[k+1]));
					strcpy(v[k],v[k+1]);
				}
				dim--;
			}
			else
				j++;
		}
	}
	return dim++;


}
/* Function: reverse
 * --------------
 * Reverse the lines of the text
 * **v: the text
 * dim: the input dimension of the text
 */
void reverse(char **v,int dim){

	/* Function to reverse arr[] from start to end*/
	int start=0;
	dim--;
	int end=dim; 
	char temp[MAX_BUF];
	while (start < end) 
	{       
		strcpy(temp,v[start]); 
		v[start] = malloc(sizeof(char) * strlen(v[end])); 
		strcpy(v[start],v[end]); 
		v[end] = malloc(sizeof(char) * strlen(temp)); 
		strcpy( v[end],temp); 
		start++; 
		end--; 

	}  


}
/* Function: main
 * --------------
 * Main function for "sort" command usage, see print_help for details.
 */

int main(int argc, char *argv[]){




	int input=0;
	int first_file=0;
	int fd=0;
	int dim=0;

	enum bool numeric=true;
	enum bool c=false, u=false,f=false, o=false,r=false,n=false;
	
	char dest[MAX_BUF];
	char tmp[255];
	char *text[255];



	//input no option first
	if(argc<=1){


		while((read(0, tmp, sizeof(tmp)) > 0)){
			text[dim]=malloc(sizeof(char)*strlen(tmp));
			strcpy(text[dim],tmp);
			dim++;
		}

		numeric=false;

		bubbleSort(text,dim,numeric);

		for(int i=0;i<dim;i++){

			printf(1,"%s",text[i],strlen(text[i]));}
		free(text);

		exit();

	}






	//other cases


	for(int i=1;i<argc;i++){
		fd=0;

		if(argv[i][0]=='-'){
			//we have options 

			if(argv[i][1]=='c'){
				c=true;


			}
			else if(argv[i][1]=='u'){
				u=true;

			}
			else if(argv[i][1]=='f'){
				f=true;
			}
			else if(argv[i][1]=='o'){
				o=true;
			}
			else if(argv[i][1]=='r'){
				r=true;

			}
			else if(argv[i][1]=='n'){
				n=true;

			}
			else if(strcmp(argv[i],"--help") == 0){
				print_help();

			}
			else if(strcmp(argv[i],"--version") == 0){
				print_version();

			}
			else{
				printf(2, "sort: invalid option %s\nTry 'head --help' for more information\n", argv[i]);
				exit();
			}

			// input case
			if(i==argc-1 || (o==true && i+1==argc-1 && argv[i+1][0]!='-')){
				input=1;
				fd=0;
			}

		}

		else{

			//identify destination filename
			if(o==true && first_file==0){
				strcpy(dest,argv[i]);
				first_file=1;
			}
			else{
				//create buffer

				fd=open(argv[i],0);
				if(fd<0){
					printf(2,"sort: No such file or directory");close(fd);exit();
				}

				int readed = read(fd, tmp, sizeof(tmp));
				close(fd);
				int n1=0;
				char row[255];

				for(int i=0;i<readed;i++){

					if(tmp[i]!='\n'){
						row[n1]=tmp[i];
						n1++;
					}
					else{   
						row[n1]=0;
						text[dim]=malloc(sizeof(char)*n1);
						strcpy(text[dim],row);
						dim++;
						n1=0;

					}
				}
			}
		}
	}

	if(input==1){

		while((read(0, tmp, sizeof(tmp)) > 0)){
			text[dim]=malloc(sizeof(char)*strlen(tmp));
			strcpy(text[dim],tmp);
			dim++;
		}

	}
	close(fd);



	if(c==true){

		for(int i=0;i<dim-1;i++){

			if(strcmp(text[i],text[i+1])<0){
				printf(1,"Sort: fuori ordine %s\n",text[i]);
				free(text);
				exit();
			}
			free(text);		
		}
		exit();
	}


	if(f==true){

		for(int i=0;i<dim-1;i++)
			toLower(text[i]);
	}
	if(u==true){
		dim=unique(text,dim);
	}

	if(n==true){
		numeric=true;
		bubbleSort(text,dim,numeric);
	}
	else{

		numeric=false;
		bubbleSort(text,dim,numeric);


	}

	if(r==true){

		reverse(text,dim);
	}

	if(o==true){

		unlink(dest);
		int outfile=open(dest, O_RDWR | O_CREATE);
		for(int i=0;i<dim;i++){
			write(outfile,text[i],strlen(text[i]));
			write(outfile,"\n",1);}
		free(text);
		exit();
	}
	if(input==0){
		for(int i=0;i<dim;i++)
			printf(1,"%s\n",text[i]);
	}
	else {
		for(int i=0;i<dim;i++){

			printf(1,"%s",text[i]);
		}
	}


	close(fd);
	free(text);
	exit();

}

