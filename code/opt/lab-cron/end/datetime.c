#include "types.h"
#include "user.h"
#include "date.h"

static char *months[] = {"NULL", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

static char *days[] = {"NULL", "Mon", "Tue","Wed", "Thu", "Fri", "Sat", "Sun"};

int dayofweek(int y, int m, int d){
	d = d + m;
	if (d < 3)
		y--;
	else
		y= y - 2;
	return ((23*m/9+d+4+y/4-y/100+y/400)%7);
	//return (d+=m<3?y−−:y−2, 23∗m/9+d+4+y/4−y/100+y/400)%7;
}

int main (int argc, char *argv[]){
	int day;
	struct rtcdate r;

	if (date(&r)) {
		printf(2, "Error");
		exit();	
	}

	day = dayofweek(r.year, r.month, r.day);

	printf(1, "%d..%s..%s..%d..", r.day, days[day], months[r.month], r.year);
	printf(1, "%d:%d:%d..UTC\n", r.hour, r.minute, r.second);
	
	exit();
}




