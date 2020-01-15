import matplotlib.pyplot as plt
import time
import subprocess
import shlex
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt
subprocess.call(shlex.split('./generate_file.sh'))
# time.sleep necessary for creating data.txt with values test
time.sleep(2)

numbers_array = []
#retrieve data from data.txt
for line in open('data.txt'):
    numbers = line.split(',')

# convert string to int
numbers_array = [int(numeric_string) for numeric_string in numbers]

# data are located in specific indeces inside data.txt
# TURNAROUND TIME INDECES
# cp--> cpu bound
cp_rr = 3
cp_pr = 15
cp_fcfs = 27
cp_cfs = 39
cp_sml = 51

# io -> I/O bound
io_rr = 11
io_pr = 23
io_fcfs = 35
io_cfs = 47
io_sml = 59

# cps -> CPU-S bound time
cps_rr = 7
cps_pr = 19
cps_fcfs = 31
cps_cfs = 43
cps_sml = 55

#SLEEPING TIME (sl)
#cp --> cpu bound
sl_cp_rr = 2
sl_cp_pr = 14
sl_cp_fcfs = 26
sl_cp_cfs = 38
sl_cp_sml = 50

#i/o --> I/O bound
sl_io_rr = 10
sl_io_pr = 22
sl_io_fcfs = 34
sl_io_cfs = 46
sl_io_sml = 58

#cpu-s bound
sl_cps_rr = 6
sl_cps_pr = 18
sl_cps_fcfs = 30
sl_cps_cfs= 42
sl_cps_sml= 54


#READY TIME (rt)
#cp --> cpu bound
rt_cp_rr = 0
rt_cp_pr = 12
rt_cp_fcfs = 24
rt_cp_cfs = 36
rt_cp_sml = 48

#i/o --> I/O bound
rt_io_rr = 8
rt_io_pr = 20
rt_io_fcfs = 32
rt_io_cfs = 44
rt_io_sml = 56

#cpu-s bound
rt_cps_rr = 4
rt_cps_pr = 16
rt_cps_fcfs = 28
rt_cps_cfs = 40
rt_cps_sml= 52


#RUNNING TIME (rut)
#cp --> cpu bound
rut_cp_rr = 1
rut_cp_pr = 13
rut_cp_fcfs = 25
rut_cp_cfs = 37
rut_cp_sml = 49

#i/o --> I/O bound
rut_io_rr = 9
rut_io_pr = 21
rut_io_fcfs = 33
rut_io_cfs = 45
rut_io_sml = 57

#cpu-s bound
rut_cps_rr = 5
rut_cps_pr = 17
rut_cps_fcfs = 29
rut_cps_cfs = 41
rut_cps_sml = 53


# NOT USEFUL DATA, ONLY FOR DIFFERENTIATE COLUMNS IN HISTOGRAM AND DISTINGUISH DIFFERENT SCHEDULERS

schedulers = []
schedulers.append(10)  # DEFAULT // RR
schedulers.append(20)
schedulers.append(30)
schedulers.append(40)  # PRIORITY
schedulers.append(50)
schedulers.append(60)
schedulers.append(70)  # FCFS
schedulers.append(80)
schedulers.append(90)
schedulers.append(100)  # CFS
schedulers.append(110)
schedulers.append(120)
schedulers.append(130)  # SML
schedulers.append(140)
schedulers.append(150)
#print(schedulers)

# TURNAROUND time for diffrent type of process and different type of scheduler
values = []
values.append(numbers_array[cp_rr])
values.append(numbers_array[io_rr])
values.append(numbers_array[cps_rr])

values.append(numbers_array[cp_pr])
values.append(numbers_array[io_pr])
values.append(numbers_array[cps_pr])

values.append(numbers_array[cp_fcfs])
values.append(numbers_array[io_fcfs])
values.append(numbers_array[cps_fcfs])

values.append(numbers_array[cp_cfs])
values.append(numbers_array[io_cfs])
values.append(numbers_array[cps_cfs])

values.append(numbers_array[cp_sml])
values.append(numbers_array[io_sml])
values.append(numbers_array[cps_sml])
#print(values)


fig = plt.figure(figsize=(15,10))
ax1 = fig.add_subplot(111)
ax1.set_title("TURNAROUND TIME COMPARISON ")
ax1.set_ylabel('TIMES (TICKS CLOCK)')
ax1.bar(schedulers, values, width=1.4)

rects = ax1.patches
label1 = "CPU-bound"
label2 = "I/O-bound"
name = []

for i in range(0, 5):
    name.append("CPU-Bound")
    name.append("I/O Bound")
    name.append("CPU-S Bound")

# plot names above columns
def autolabel(rects):
# attach some text labels
    for ii,rect in enumerate(rects):
        height = rect.get_height()
        plt.text(rect.get_x()+rect.get_width()/2., 1.02*height, '%s'% (name[ii]),
                ha='center', va='bottom')
autolabel(rects)



for i in range(0,3):
    rects[i].set_facecolor('orange')
for i in range(3,6):
    rects[i].set_facecolor('yellow')
for i in range(6,9):
    rects[i].set_facecolor('red')
for i in range(9, 12):
    rects[i].set_facecolor('green')
for i in range(12, 15):
    rects[i].set_facecolor('blue')

orange_patch = mpatches.Patch(color='orange', label='Default Xv6 scheduler')
yellow_patch = mpatches.Patch(color='yellow', label='Priority')
red_patch = mpatches.Patch(color='red', label='FCFS')
green_patch = mpatches.Patch(color='green', label='CFS')
blue_patch = mpatches.Patch(color='blue', label='SML')



plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05),
          fancybox=True, shadow=True, ncol=5, handles=[orange_patch, yellow_patch, red_patch,green_patch,blue_patch], title="SCHEDULER")

plt.xticks([])
fig1 = plt.gcf()

plt.show()
plt.draw()

# READY TIME
values = []
values.append(numbers_array[rt_cp_rr])
values.append(numbers_array[rt_io_rr])
values.append(numbers_array[rt_cps_rr])

values.append(numbers_array[rt_cp_pr])
values.append(numbers_array[rt_io_pr])
values.append(numbers_array[rt_cps_pr])

values.append(numbers_array[rt_cp_fcfs])
values.append(numbers_array[rt_io_fcfs])
values.append(numbers_array[rt_cps_fcfs])

values.append(numbers_array[rt_cp_cfs])
values.append(numbers_array[rt_io_cfs])
values.append(numbers_array[rt_cps_cfs])

values.append(numbers_array[rt_cp_sml])
values.append(numbers_array[rt_io_sml])
values.append(numbers_array[rt_cps_sml])

fig = plt.figure(figsize=(15,10))
ax1 = fig.add_subplot(111)
ax1.set_title("READY TIME COMPARISON ")
ax1.set_ylabel('TIMES (TICKS CLOCK)')
ax1.bar(schedulers, values, width=1.4)


rects = ax1.patches
label1 = "CPU-bound"
label2 = "I/O-bound"
name = []
for i in range(0, 5):
    name.append("CPU-Bound")
    name.append("I/O Bound")
    name.append("CPU-S Bound")



# plot names above columns
def autolabel(rects):
# attach some text labels
    for ii,rect in enumerate(rects):
        height = rect.get_height()
        plt.text(rect.get_x()+rect.get_width()/2., 1.02*height, '%s'% (name[ii]),
                ha='center', va='bottom')
autolabel(rects)

for i in range(0,3):
    rects[i].set_facecolor('orange')
for i in range(3,6):
    rects[i].set_facecolor('yellow')
for i in range(6,9):
    rects[i].set_facecolor('red')
for i in range(9, 12):
    rects[i].set_facecolor('green')
for i in range(12, 15):
    rects[i].set_facecolor('blue')
plt.text(20,500,'SCHEDULER',horizontalalignment='center')

plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05),
          fancybox=True, shadow=True, ncol=5, handles=[orange_patch, yellow_patch, red_patch,green_patch,blue_patch], title="SCHEDULER")

plt.xticks([])
fig1 = plt.gcf()

plt.show()
plt.draw()


# RUNNING TIME

values = []
values.append(numbers_array[rut_cp_rr])
values.append(numbers_array[rut_io_rr])
values.append(numbers_array[rut_cps_rr])

values.append(numbers_array[rut_cp_pr])
values.append(numbers_array[rut_io_pr])
values.append(numbers_array[rut_cps_pr])

values.append(numbers_array[rut_cp_fcfs])
values.append(numbers_array[rut_io_fcfs])
values.append(numbers_array[rut_cps_fcfs])

values.append(numbers_array[rut_cp_cfs])
values.append(numbers_array[rut_io_cfs])
values.append(numbers_array[rut_cps_cfs])

values.append(numbers_array[rut_cp_sml])
values.append(numbers_array[rut_io_sml])
values.append(numbers_array[rut_cps_sml])

fig = plt.figure(figsize=(15,10))
ax1 = fig.add_subplot(111)
ax1.set_title("RUNNING TIME COMPARISON ")
ax1.set_ylabel('TIMES (TICKS CLOCK)')
ax1.bar(schedulers, values, width=1.4)


rects = ax1.patches
label1 = "CPU-bound"
label2 = "I/O-bound"
name = []
for i in range(0, 5):
    name.append("CPU-Bound")
    name.append("I/O Bound")
    name.append("CPU-S Bound")



# plot names above columns
def autolabel(rects):
# attach some text labels
    for ii,rect in enumerate(rects):
        height = rect.get_height()
        plt.text(rect.get_x()+rect.get_width()/2., 1.02*height, '%s'% (name[ii]),
                ha='center', va='bottom')
autolabel(rects)

for i in range(0,3):
    rects[i].set_facecolor('orange')
for i in range(3,6):
    rects[i].set_facecolor('yellow')
for i in range(6,9):
    rects[i].set_facecolor('red')
for i in range(9, 12):
    rects[i].set_facecolor('green')
for i in range(12, 15):
    rects[i].set_facecolor('blue')
plt.text(20,500,'SCHEDULER',horizontalalignment='center')

plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05),
          fancybox=True, shadow=True, ncol=5, handles=[orange_patch, yellow_patch, red_patch,green_patch,blue_patch], title="SCHEDULER")

plt.xticks([])
fig1 = plt.gcf()

plt.show()
plt.draw()

# SLEEPING TIME
values = []
values.append(numbers_array[sl_cp_rr])
values.append(numbers_array[sl_io_rr])
values.append(numbers_array[sl_cps_rr])

values.append(numbers_array[sl_cp_pr])
values.append(numbers_array[sl_io_pr])
values.append(numbers_array[sl_cps_pr])

values.append(numbers_array[sl_cp_fcfs])
values.append(numbers_array[sl_io_fcfs])
values.append(numbers_array[sl_cps_fcfs])

values.append(numbers_array[sl_cp_cfs])
values.append(numbers_array[sl_io_cfs])
values.append(numbers_array[sl_cps_cfs])

values.append(numbers_array[sl_cp_sml])
values.append(numbers_array[sl_io_sml])
values.append(numbers_array[sl_cps_sml])

fig = plt.figure(figsize=(15,10))
ax1 = fig.add_subplot(111)
ax1.set_title("SLEEPING TIME COMPARISON ")
ax1.set_ylabel('TIMES (TICKS CLOCK)')
ax1.bar(schedulers, values, width=1.4)


rects = ax1.patches
label1 = "CPU-bound"
label2 = "I/O-bound"
name = []
for i in range(0, 5):
    name.append("CPU-Bound")
    name.append("I/O Bound")
    name.append("CPU-S Bound")



# plot names above columns
def autolabel(rects):
# attach some text labels
    for ii,rect in enumerate(rects):
        height = rect.get_height()
        plt.text(rect.get_x()+rect.get_width()/2., 1.02*height, '%s'% (name[ii]),
                ha='center', va='bottom')
autolabel(rects)

for i in range(0,3):
    rects[i].set_facecolor('orange')
for i in range(3,6):
    rects[i].set_facecolor('yellow')
for i in range(6,9):
    rects[i].set_facecolor('red')
for i in range(9, 12):
    rects[i].set_facecolor('green')
for i in range(12, 15):
    rects[i].set_facecolor('blue')
plt.text(20,500,'SCHEDULER',horizontalalignment='center')

plt.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05),
          fancybox=True, shadow=True, ncol=5, handles=[orange_patch, yellow_patch, red_patch,green_patch,blue_patch], title="SCHEDULER")

plt.xticks([])
fig1 = plt.gcf()

plt.show()
plt.draw()

