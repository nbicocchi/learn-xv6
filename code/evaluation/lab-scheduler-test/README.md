# XV6 Testing Schedulers #
A patch that allows you to test how five different types of schedulers (Round Robin, Priority, FCFS, CFS, SML) behave on different task types, CPU bound or I/O bound. The main parameters in measuring the efficiency of a scheduler are evaluated, such as the turnaround time, the time needed to complete a request.
The turnaround time of the schedulers of a CPU-bound task is shown in a graph using the python file. To communicate, Xv6 and Ubuntu use file system (fs.img).

## Run It ##
* $ sudo ./generate.sh -l <labn>
* ./generate_file.sh
* python graphs.py 
Important notes:

* Pay attention to the paths inside each file (generate_file.sh and graphs.py), you may need to change them. To run properly, graphs.py needs graphics packages and creating a project.

## Papers ##
https://bib.irb.hr/datoteka/463774.cisse2009.pdf
https://www.researchgate.net/publication/333079926_A_Comparative_Study_of_Various_CPU_Scheduling_Algorithms_using_MOOS_Simulator

## Contributors ##
* Davide Zini

## Credits ##
All the code for the test and runtime scheduler are taken from these github links:
https://github.com/nbicocchi/operatingsystemsmsc/tree/master/code/opt/lab-scheduler-runtime
https://github.com/marf/xv6-scheduling/tree/master/lab_scheduling/

Thanks to:

* Nicola Bicocchi
* Davide Iavicoli
* Marco Fontana




