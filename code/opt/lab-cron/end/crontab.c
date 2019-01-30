#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char *argv_c[] = {"cron", "0"};
char *argv_e[] = {"editor", "cronfile"};

void kill_cron(){
    //code for killing old cron process
    int pid = 0;
    pid = pidcron();

    if (pid > 0){
	printf(2, "killing cron process\n");
	kill(pid);
    }
};

int main(int argc, char *argv[]){
    if (argc != 2 || strlen(argv[1]) != 2){
        printf(2, "error in parameters\n");
	exit();
    }

    int count = 0;
    char option = argv[1][1];
    char buf;
    short n;
    int fd;
    int pid, pid2;

    //list
    if (option == 'l'){
	fd = open("cronfile", O_RDONLY);
	if (fd < 0){
	    printf(2, "Errore nell'apertura del file cronfile");
	    exit();
	}

	while((n = read(fd, &buf, sizeof(buf))) > 0){
	    if (buf == '\n')
		count++;
	    if (count == 3)
		break;
	}

	while((n = read(fd, &buf, sizeof(buf)) > 0)){
	  printf(2, "%c", buf);
	}

	exit();
    }

    //editor
    if (option == 'e'){

	//devo eliminare il processo cron attivo
	kill_cron();

	pid = fork();
	if (pid == 0 ) {
	    //create a new process that opens the editor
	    //editor start in order to modify cronfile
	    exec("editor", argv_e);
	    exit();
	} else if (pid > 0){
	    //father process wait editor closure and create a new process that execute cron
	    wait();
	    pid2 = fork();
	    if (pid2 == 0) {
		exec("cron", argv_c);
		exit();
	    } else {
	        exit();
	    }
	} else {
	    printf(2, "Error");
	}
	
    }

    //reset
    if (option == 'r'){

	//devo eliminare il processo cron attivo
	kill_cron();

	//delete file and create a new one
    	if(unlink("cronfile") < 0){
            printf(2, "file already deleted\n");
        }
	fd = open("cronfile", O_CREATE|O_WRONLY);
	if (fd < 0){
	    printf(2, "Error in creating cronfile\n");
	    exit();
	}

	printf(fd, "This file is used from cron process.\n\nhour\tminute\tdom\tmonth\tdow\tcommand\n");
	close(fd);
	exit();
    }

    printf(2, "Error: the inserted command is not supported\n");
    exit();
}
