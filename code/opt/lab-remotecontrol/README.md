# XV6 Monitoring Tool #
Simple tool for monitoring xv6 internal (processes) state in a convinient manner using an external termianal.

## Run It ##
The tool is fully integrated with the generate.sh script used to build the system's patches.
To run it just use:

* $ sudo ./generate.sh -l <labn>
* (Also to clean you have to use sudo)
Important notes:

* Make sure that the xv6_monitor.c script is in the same folder of generate.sh
* xv6_monitor.c to run properly requires a special tap interface called tap0, if you have other tap interfaces in your system the one used by xv6 will be different (tap1,tap2 ...) in that case you need to edit the value of iface inside the script 

## Contributors ##
* Christian Gilli

## Credits ##
The code for the test_scheduler tool was taken from previous labs.
All the code for the network driver was taken from github:

* https://github.com/vibhorvatsa/xv6-networking-stack

Thanks to:

* Pradeep Kumar Beri
* Anmol Vatsa
* Vinod Reddy

