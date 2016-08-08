from sys import argv
import matplotlib.pyplot as plt
import glob

filename = '../totals'
folder = '../charts/'


x_threads = [1, 2, 3, 4, 5, 6, 7, 8]

y_f2c2 = []
y_lock_only = []
y_probe = []
y_throttle = []

with open(filename, 'r') as r:

    for line in r:

        split_line = line.split('\t')

        program = split_line[0]
        mode = split_line[1]
        threads = split_line[2]
        throughput = split_line[3]
        time = split_line[4]
    
        if mode == 'f2c2':
            y_f2c2.append(time)
        elif mode == 'lock_only':
            y_lock_only.append(time)
        elif mode == 'probe':
            y_probe.append(time)
        else:
            y_throttle.append(time)

# x_timestamp = [i for i in range(0, len(y_commits))]
# x_timestamp = [(float(x) / (len(x_timestamp) - 1)) * 100 for x in x_timestamp]

plt.xlabel('Maximum Concurrency Specification')
plt.ylabel('Execution Time (seconds)')

plt.plot(x_threads, y_f2c2, 'b-', label='F2C2')
plt.plot(x_threads, y_lock_only, 'g-', label='None')
plt.plot(x_threads, y_probe, 'r-', label='Probe')
plt.plot(x_threads, y_throttle, 'c-', label='Throttle')

plt.legend(loc='upper center', fontsize='smaller', bbox_to_anchor=(0.5, 1.05),
          ncol=4, fancybox=True, shadow=True)

fig = plt.gcf()

plt.show()

fig.savefig(folder+'totals_'+program+'.png')
# pylab.savefig(folder+'commits_'+program+'_'+mode+'_'+threads+'.pdf')

plt.close()