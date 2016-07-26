from sys import argv
import matplotlib.pyplot as plt
import matplotlib.pylab as pylab
import glob

folder = '../charts/'

filenames = glob.iglob("auto-results/results_*.txt")

for filename in filenames:

    x_timestamp = []

    y_commits = []
    y_window = []
    y_aborts = []
    y_locks = []

    program = filename.split("_")[1]
    threads = filename.split("_")[2].split(".")[0]
    mode = ""

    with open(filename, 'r') as r:

        for line in r:

            if line.startswith('Chart_data'):
            
                split_line = line.split('\t')
            
                commits = split_line[1]
                window = split_line[2]
                timestamp = split_line[3]
                # threads = split_line[4]
                mode = split_line[5]
                aborts = split_line[6]
                locks = split_line[7]
            
                x_timestamp.append(timestamp)
                y_commits.append(commits)
                y_window.append(window)
                y_aborts.append(aborts)
                y_locks.append(locks)

    # x_timestamp = [i for i in range(0, len(y_commits))]
    # x_timestamp = [(float(x) / (len(x_timestamp) - 1)) * 100 for x in x_timestamp]

    plt.xlabel('Millis since TM_STARTUP')
    plt.ylabel('Commits per cycle')

    plt.plot(x_timestamp, y_commits, 'b-', label=program + ' - ' + mode + ' - ' + threads + ' threads')

    plt.legend(loc='upper left', fontsize='small')

    plt.show()

    pylab.savefig(folder+'commits_'+program+'_'+mode+'_'+threads+'.png')
    pylab.savefig(folder+'commits_'+program+'_'+mode+'_'+threads+'.pdf')



    plt.xlabel('Millis since TM_STARTUP')
    plt.ylabel('Window size per cycle')

    plt.plot(x_timestamp, y_window, 'r-', label=program + ' - ' + mode + ' - ' + threads + ' threads')

    plt.legend(loc='upper left', fontsize='small')

    plt.show()

    pylab.savefig(folder+'window_'+program+'_'+mode+'_'+threads+'.png')
    pylab.savefig(folder+'window_'+program+'_'+mode+'_'+threads+'.pdf')



    plt.xlabel('ETA since TM_STARTUP')
    plt.ylabel('Aborts per cycle')

    plt.plot(x_timestamp, y_aborts, 'r-', label=program + ' - ' + mode + ' - ' + threads + ' threads')

    plt.legend(loc='upper left', fontsize='small')

    plt.show()

    pylab.savefig(folder+'aborts_'+program+'_'+mode+'_'+threads+'.png')
    pylab.savefig(folder+'aborts_'+program+'_'+mode+'_'+threads+'.pdf')



    plt.xlabel('ETA since TM_STARTUP')
    plt.ylabel('Locks per cycle')

    plt.plot(x_timestamp, y_locks, 'r-', label=program + ' - ' + mode + ' - ' + threads + ' threads')

    plt.legend(loc='upper left', fontsize='small')

    plt.show()

    pylab.savefig(folder+'locks_'+program+'_'+mode+'_'+threads+'.png')
    pylab.savefig(folder+'locks_'+program+'_'+mode+'_'+threads+'.pdf')