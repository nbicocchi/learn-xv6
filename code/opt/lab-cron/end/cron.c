#include "types.h"
#include "stat.h"
#include "user.h"
#include "date.h"
#include "fcntl.h"
#include "param.h"

int dayofweek(int y, int m, int d){
	d = d + m;
	if (d < 3)
		y--;
	else
		y = y-2;
	return ((23*m/9+d+4+y/4-y/100+y/400)%7);
	//return (d+=m<3?y−−:y−2, 23∗m/9+d+4+y/4−y/100+y/400)%7;
};

void kill_cron(){
    //code for killing old cron process
    int pid = 0;
    pid = pidcron();

    if (pid>0){
	printf(2, "killing cron process\n");
	kill(pid);
    }
};

struct command{
    int hour;
    int minute;
    int dom;
    int month;
    int dow;
    char command[20];
    char **arg_c;
};

int main(int argc, char** argv){
    int i = 0;
    int count = 0;
    int n;
    int fd;
    char ch;
    char buf[2];
    char command[20];
    struct command r;
    struct command *commands = malloc(10*sizeof(struct command));
    int pid;
    int dow;
    struct rtcdate rtc;
    date(&rtc);

    //kill_cron();

    fd = open("cronfile", O_RDONLY);
    //lettura cronfile

    //scarto le prime tre linee del file
    while((n = read(fd, &ch, sizeof(ch))) > 0){
	if (ch == '\n')
	    count++;
	if (count == 3)
	    break;
    }

    //leggo le righe e inserisco i valori dentro alla struct
    count = 0;
    n = read(fd, &ch, sizeof(ch));
    for (;count < 10;){
	//hour
	if (n < 0){
	    printf(2, "Error: file ended too early");
	    break;
	}
	if (ch == '*')
	    r.hour = 99;
	else {
	    if (ch >= '0' && ch <= '9'){
		buf[0] = ch;
		n = read(fd, &ch, sizeof(ch));
		if (n < 0 || ch < '0' || ch > '9'){
		    printf(2, "Error: wrong file format");
	    	    break;
		}
		buf[1] = ch;
		if ((r.hour = atoi(buf)) > 23){
		    printf(2, "Error: wrong hour");
		    break;
		}
	    } else {
		printf(2, "Error: wrong file format");
		break;		
	    }
	}
	n = read(fd, &ch, sizeof(ch));
	if (ch != '\t' || n < 0){
	    printf(2, "Error: missing tab");
	    break;
	}

	//minute
	n = read(fd, &ch, sizeof(ch));
	if (n < 0){
	    printf(2, "Error");
	    break;
	}
	if (ch == '*')
	    r.minute = 99;
	else {
	    if (ch >= '0' && ch <= '9'){
		buf[0] = ch;
		n = read(fd, &ch, sizeof(ch));
		if (n < 0 || ch < '0' || ch > '9'){
		    printf(2, "Error: wrong file format");
	    	    break;
		}
		buf[1] = ch;
		if ((r.minute = atoi(buf)) > 59){
		    printf(2, "Error: wrong minute");
		    break;
		}
	    } else {
		printf(2, "Error: wrong file format");
		break;		
	    }
	}
	n = read(fd, &ch, sizeof(ch));
	if (ch != '\t' || n < 0){
	    printf(2, "Error");
	    break;
	}

	//dom
	n = read(fd, &ch, sizeof(ch));
	if (n < 0){
	    printf(2, "Error");
	    break;
	}
	if (ch == '*')
	    r.dom = 99;
	else {
	    if (ch >= '0' && ch <= '9'){
		buf[0] = ch;
		n = read(fd, &ch, sizeof(ch));
		if (n < 0 || ch < '0' || ch > '9'){
		    printf(2, "Error: wrong file format");
	    	    break;
		}
		buf[1] = ch;
		if ((r.dom = atoi(buf)) > 31){
		    printf(2, "Error: wrong dom");
		    break;
		}
	    } else {
		printf(2, "Error: wrong file format");
		break;		
	    }
	}
	n = read(fd, &ch, sizeof(ch));
	if (ch != '\t' || n < 0){
	    printf(2, "Error");
	    break;
	}

	//month
	n = read(fd, &ch, sizeof(ch));
	if (n < 0){
	    printf(2, "Error");
	    break;
	}
	if (ch == '*')
	    r.month = 99;
	else {
	    if (ch >= '0' && ch <= '9'){
		buf[0] = ch;
		n = read(fd, &ch, sizeof(ch));
		if (n < 0 || ch < '0' || ch > '9'){
		    printf(2, "Error: wrong file format");
	    	    break;
		}
		buf[1] = ch;
		if ((r.month = atoi(buf)) > 12){
		    printf(2, "Error: wrong month");
		    break;
		}
	    } else {
		printf(2, "Error: wrong file format");
		break;		
	    }
	}
	n = read(fd, &ch, sizeof(ch));
	if (ch != '\t' || n < 0){
	    printf(2, "Error");
	    break;
	}

	//dow
	n = read(fd, &ch, sizeof(ch));
	if (n < 0){
	    printf(2, "Error");
	    break;
	}
	if (ch == '*')
	    r.dow = 99;
	else {
	    if (ch >= '1' && ch <= '7'){
		buf[0] = '0';
		buf[1] = ch;
		r.dow = atoi(buf);
	    } else {
		printf(2, "Error: wrong file format");
		break;		
	    }
	}
	n = read(fd, &ch, sizeof(ch));
	if (ch != '\t' || n < 0){
	    printf(2, "Error");
	    break;
	}	

	//command
	for (i = 0; i<20; i++)
	    command[i] = '\0';
	i=0;
	while((n = read(fd, &ch, sizeof(ch))) > 0){
	    if (ch == '\n')
		break;
	    command[i] = ch;
	    i++;
	}

	strcpy(r.command, command);

	r.arg_c = malloc(2 * sizeof(char*));
	r.arg_c[0] = malloc(strlen(command));
	r.arg_c[1] = malloc(sizeof(char));
	strcpy(r.arg_c[0], command);
	strcpy(r.arg_c[1], "&");	

	//printf(2, "%d, %d, %d, %d, %d, %s, %s, %s\n", r.hour, r.minute, r.dom, r.month, r.dow, r.command, r.arg_c[0], r.arg_c[1]);

	commands[count] = r;
	count++;

	if ((n = read(fd, &ch, sizeof(ch))) > 0)
	    continue;
	else {
	    //printf(2, "File ended\n");
	    break;
	}
    }
    close(fd);
    //lettura terminata

    for(;;){
    	date(&rtc);
	dow = dayofweek(rtc.year, rtc.month, rtc.day);
	//printf(2, "%d	%d	%d	%d	%d\n", rtc.hour, rtc.minute, rtc.day, rtc.month, dow);
	for (i = 0; i < 10; i++){
	    if ((commands[i].hour == 99 || commands[i].hour == rtc.hour) && (commands[i].minute == 99 || commands[i].minute == rtc.minute) && (commands[i].dom == 99 || commands[i].dom == rtc.day) && (commands[i].month == 99 || commands[i].month == rtc.month) && (commands[i].dow == 99 || commands[i].dow == dow)){
		printf(2, "cron: sto lanciando il comando %s\n", commands[i].command);
	        pid = fork();
	        if (pid == 0){
		    //child process
		    exec(commands[i].command, commands[i].arg_c);
		    exit();
	        } else if (pid > 0){
		    //father process
		    wait();
		    continue;
	        } else {
		    printf(2, "Error");
		    exit();
		}
	    }
	}
	//100 ticks = 1 sec --- sleep take a number of ticks
	sleep(58*100);
    }

    exit();
}
