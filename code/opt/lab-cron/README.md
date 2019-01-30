## Operating Systems Course - DIEF UNIMORE ##

Cron and Crontab simplified implementation.

## crontab.c ##
Crontab command is used to modify Cronfile which contains the time and the command that must be executed at that specific time. Cronfile has 6 fields separated with tab and the format is the following:
hour	minute	dom	month	dow	command
##	##	##	##	#	string
Hour, minute, dom (day of month), month needs two number and dow (day of week) only one. Every field can be replaced with '*' (single character) in order to execute that command more then once.
Crontab -l --> list all actions contained in cronfile
Crontab -r --> reset cronfile. After this command cronfile will be empty and cron process will stop.
Crontab -e --> edit cronfile. This command stop cron process, open an editor for editing cronfile and restart cron process.

## cron.c ##
Cron process will start on xv6 boot and every time that cronfile is edited. It scans cronfile and save up to 10 commands. After that, and every minute, cron will retrieve the date trough the system call date(), check if there are some actions to execute and then sleep a minute.

## system call ##
I added two system call:
  - date() -> retrieve the date from the rtc (real time clock) and store those information in a date struct.
  - pidcron() -> retrieve the pid of cron process. This information is used in order to kill the right process when cron must be stopped.

## datetime.c ##
Datetime function was created in order to check the date retrieved from the system call date() and print it on standard output.

## Init.c ##
This file was modified for launching cron process at xv6 boot.

## lapic.c ##
This file contains the function that retrieves the date from rtc. The information stored in rtc (emulated by qemu) seems to be in Western Europe format (UTC + 0) so I modified this function for getting Central Europe time by adding one hour (and checking if this change implies the modification of day and month).

## prova.c ##
Contains a function that prints the value of a variable on standard output. It was created in order to check the rigth behaviour of cron process.

Notes:
  - i found the editor online at this link: https://github.com/THSS13/XV6/blob/master/xv6/editor.c
  - the algorithm for calculating the dow (day of week) was found at: https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week
