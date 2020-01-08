import matplotlib.pyplot as plt

numbers_array = []
#retrieve data from values
for line in open('/home/davide/Desktop/data.txt'):
    numbers = line.split(',')

# convert string to iny
numbers_array = [int(numeric_string) for numeric_string in numbers]



print("TURNAROUND TIME STATISTICS WITH 5 DIFFERENT SCHEDULERS")
print(numbers_array)

schedulers = []
schedulers.append(0); # DEFAULT // RR
schedulers.append(1); # PRIORITY
schedulers.append(2); # FCFS
schedulers.append(3); # CFS
schedulers.append(4); # SML
#print(schedulers)
# TURNAROUND TIME FOR CPU BOUNDS PROCESS
values = []
values.append(numbers_array[3]);
values.append(numbers_array[15]);
values.append(numbers_array[27]);
values.append(numbers_array[39]);
values.append(numbers_array[51]);
#print(values)


fig = plt.figure(figsize=(15,10))
ax1 = fig.add_subplot(111)
ax1.set_title("TURNAROUND TIME COMPARISON FOR CPU-BOUND PROCESS")
ax1.set_xlabel('SCHEDULERS: 0-DEFAULT, 1-PRIORITY, 2-FCFS, 3-CFS, 4-SML');
ax1.set_ylabel('TIMES')
ax1.bar(schedulers,values, width=0.7)

fig1=plt.gcf()
plt.show()
plt.draw()

#x, y = zip(*(line.split() for line in lines))

#f.close()

#print(x, y)

#plt.plot(x,y)
#plt.show()








