#include "types.h"
#include "user.h"
#include "date.h"

#define MAX_BUF 128 
char buf[MAX_BUF];

const char *_days[] = {
  "Wednesday",
  "Thursday", 
  "Friday", 
  "Saturday",
  "Sunday", 
  "Monday", 
  "Tuesday"
};

const char *_months[] = {
  "January", "February", "March",
  "April", "May", "June",
  "July", "August", "September",
  "October", "November", "December"
};

const char *_days_abbrev[] = {
  "Sun", "Mon", "Tue", "Wed", 
  "Thu", "Fri", "Sat"
};

const char *_months_abbrev[] = {
  "Jan", "Feb", "Mar",
  "Apr", "May", "Jun",
  "Jul", "Aug", "Sep",
  "Oct", "Nov", "Dec"
};

struct rtcdate r;

void current_date(){
	if (date(&r)) {
	    printf(2, "Date failed! see cmostime function\n");
	    exit();
	}
}

void get_short_human_time()
{
	printf(1, "%d %s, %d %d:%d:%d\n", 
		r.day,
		_months_abbrev[r.month - 1],
		r.year, 
		r.hour,
		r.minute,
		r.second
	);
}

void get_human_date()
{
	printf(1, "%d %s %d\n", 
		r.day,
		_months[r.month - 1],
		r.year
	);	
}

void get_human_time()
{
	printf(1, "%d %s, %d %d:%d:%d\n", 
		r.day,
		_months[r.month - 1],
		r.year, 
		r.hour,
		r.minute,
		r.second
	);
}

void get_ISO8601_datetime()
{
	printf(1, "%d-%d-%d %d:%d:%d\n", 
		r.year, 
		r.month,
		r.day,
		r.hour,
		r.minute,
		r.second
	);
}

long get_unix_time()
{
	uint y = r.year - 1900;
	return r.second + r.minute*60 + r.hour*3600 + r.day*86400 +
    (y-70)*31536000 + ((y-69)/4)*86400 -
    ((y-1)/100)*86400 + ((y+299)/400)*86400;
}

long get_days_num()
{
	uint m = (r.month + 9) % 12;
	uint y = r.year - (m / 10);
	uint d = r.day;

	return(365*y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + (d - 1));
}

void get_day_of_week()
{
	printf(1, "%s\n", _days[get_days_num() % 7]);
}

void help(){
	printf(1, "Usage: date [OPTION]...\nPrint date information\n");
	printf(1, "Possible arguments are:\n");
	printf(1, "\tnothing: current system date time in UTC\n");
	printf(1, "\t-h: Human Readable\n");
	printf(1, "\t-H: Human Short Readable version\n");
	printf(1, "\t-u: Unix time since 1st Jan, 1970\n");
	printf(1, "\t-w: Weekday\n");
	printf(1, "\t-?: Show this help\n");
	printf(1, "\n");
}

void check_args(int argc, char **argv){

	// As a first, take the system time
	current_date();

	if(argc == 1){
		printf(1, "Current UTC datetime is:\n");
		get_ISO8601_datetime();
		printf(1, "\n");
		exit();
	}

	char buf[64];
	strcpy(buf, argv[1]);

	if(buf[0] == '-'){
		switch(buf[1]){
			case '?':
				help();
				break;
			case 'u':
				printf(1, "Current Unix Time (ISO8601): %d\n", get_unix_time());
				break;
			case 'h':
				printf(1, "Current Time is: \n");
				get_human_time();
				break;
			case 'H':
				printf(1, "Current Time (short) is: \n");
				get_short_human_time();
				break;
			case 'd':
				printf(1, "Current Date is: \n");
				get_human_date();
				break;
			case 'w':
				printf(1, "Day of week is : \n");
				get_day_of_week();
				break;
			default:
				printf(2, "Invalid command supplied: %s\n", buf[1]);
				help();
		}
	}
}

int main(int argc, char **argv) { 
	check_args(argc, argv); 
	exit();
}
