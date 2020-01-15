# XV6 Testing Schedulers #
A patch that allows you to test how five different types of schedulers (Round Robin, Priority, FCFS, CFS, SML) behave on different task types. 
The main parameters in measuring the efficiency of a scheduler are evaluated: turnaround time, ready time, sleeping time, running time.
There are three different type of process : CPU bound (intesive use of CPU), S-CPU (short CPU, intesive use for a short period), I/O bound (low use of CPU).
To communicate, Xv6 and Ubuntu use file system (fs.img).


# How it is made
There is a new runnable command inside Xv6:
```command
test_schedulers num_process
```
The command creates n number of processes, each process is categorized into a certain type (those described in the introduction). The test if it reports data beyond 10k ticks could cause a problem in the interpretation of the data from the ubuntu side, it is better not to create too many processes (6 processes are enough to see the differences between the schedulers)
The test is performed for each scheduler present in the system (in total 5). The scheduler that the user is using is temporarily saved and restored once the test is finished. 
This operation is possible thanks to the fact that there is the possibility to change the run time scheduler. Once the test is completed, the numeric data, in this case int type, are converted into string format (more precisely ASCII characters are needed) to allow easier retrieval on the Ubuntu / Linux side.
 A marker character ('S!') is also inserted
```cpp 
write (fd," S! \ n ", strlen (" S! "));
```
to understand when the numerical sequence concerned starts.
When the command finishes, you can go back to Ubuntu and launch a python file that allows you to graph the test results. 
A total of 4 graphs are created, each of them represents a significant time (ready, sleeping, turnaround, running). 
Schedulers are proposed on the x-axis, each of them has three columns representing the 3 types of processes to spot easily diffrences of performance. Each scheduler has an assigned color.
At the beginning of the python file, a generate_file.sh script file is launched:
```sh
#!/bin/bash
strings /home/davide/operatingsystemsmsc/code/xv6-public-lab-scheduler-test/fs.img | grep 'S!' > tmp.txt
head -1 tmp.txt > data.txt
sed -r 's/.{2}//' data.txt > tmp.txt
sed -r 's/.{3}$//' tmp.txt > data.txt
rm -rf tmp.txt
```
which searches inside fs.img the marker character, cleans up the files and creates a data.txt file. 
Subsequently, the data is put into arrays and graphed thanks to the matplotlib library.

## Run It ##
* $ sudo ./generate.sh -l <labn>
* pyhton graphs.py 
Important notes:
* Pay attention to the paths inside each file (generate_file.sh and graphs.py), you may need to change them. To run properly, graphs.py needs graphics packages and the creation a project.

## Papers ##
https://bib.irb.hr/datoteka/463774.cisse2009.pdf
https://www.researchgate.net/publication/333079926_A_Comparative_Study_of_Various_CPU_Scheduling_Algorithms_using_MOOS_Simulator

## Contributors ##
* Davide Zini

## Credits ##
All the code for the test and runtime scheduler are taken from previous these github links:
https://github.com/nbicocchi/operatingsystemsmsc/tree/master/code/opt/lab-scheduler-runtime
https://github.com/marf/xv6-scheduling/tree/master/lab_scheduling/

Thanks to:

* Nicola Bicocchi
* Davide Iavicoli
* Marco Fontana




