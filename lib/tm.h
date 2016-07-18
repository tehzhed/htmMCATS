
#ifndef TM_H
#define TM_H 1

#  include <gsl/gsl_linalg.h>


#  define MAIN(argc, argv)              int main (int argc, char** argv)
#  define MAIN_RETURN(val)              return val

#  define GOTO_SIM()                    /* nothing */
#  define GOTO_REAL()                   /* nothing */
#  define IS_IN_SIM()                   (0)

#  define SIM_GET_NUM_CPU(var)          /* nothing */

#  define TM_PRINTF                     printf
#  define TM_PRINT0                     printf
#  define TM_PRINT1                     printf
#  define TM_PRINT2                     printf
#  define TM_PRINT3                     printf

#  define P_MEMORY_STARTUP(numThread)   /* nothing */
#  define P_MEMORY_SHUTDOWN()           /* nothing */

#  include <assert.h>
#  include "memory.h"
#  include "thread.h"
#  include "types.h"
#  include "thread.h"

#  include <immintrin.h>
#  include <rtmintrin.h>

#  include <stddef.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <time.h>
#  include <unistd.h>
#  include <semaphore.h>
#  include <math.h>

#  define TM_ARG                        /* nothing */
#  define TM_ARG_ALONE                  /* nothing */
#  define TM_ARGDECL                    /* nothing */
#  define TM_ARGDECL_ALONE              /* nothing */
#  define TM_CALLABLE                   /* nothing */

#define CACHE_LINE_SIZE ({ \
   	FILE *p = fopen("/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size", "r"); \
   	unsigned int i = 0; \
   	if (p) { \
  		fscanf(p, "%d", &i); \
   		fclose(p); \
    } \
    i; \
})

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

typedef enum {
	INCREASING,
	DECREASING
} tm_state_t;

__attribute__((aligned(64))) unsigned int concurrency_window_size;
__attribute__((aligned(64))) unsigned long last_cycle_timestamp;
__attribute__((aligned(64))) unsigned int last_cycle_commits;
__attribute__((aligned(64))) unsigned int current_cycle_commits;
__attribute__((aligned(64))) unsigned int gated[NUMBER_THREADS];
__attribute__((aligned(64))) sem_t gateSemaphore[NUMBER_THREADS];
__attribute__((aligned(64))) unsigned long  min_cycle_duration;
__attribute__((aligned(64))) unsigned long  max_cycle_duration;
__attribute__((aligned(64))) unsigned long  avg_cycle_duration;
__attribute__((aligned(64))) unsigned int min_num_commits;
__attribute__((aligned(64))) unsigned int max_num_commits;
__attribute__((aligned(64))) unsigned int avg_num_commits;
__attribute__((aligned(64))) unsigned int num_cycles;
__attribute__((aligned(64))) unsigned int thread_stats[NUMBER_THREADS];
__attribute__((aligned(64))) tm_state_t state;

__attribute__((aligned(64))) static volatile unsigned long gate_lock = 0;


#define CURRENT_TIMESTAMP() ({ \
	struct timeval  tv; \
	gettimeofday(&tv, NULL); \
	unsigned long time_in_mill = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ; \         	
	time_in_mill; \
})

#define CYCLE_MILLIS 10000

#define NUMBER_CORES sysconf(_SC_NPROCESSORS_ONLN)

typedef unsigned long tm_time_t;

#define TM_TIMER_READ() ({ \
	unsigned int lo; \
	unsigned int hi; \
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi)); \
	((tm_time_t)hi) << 32 | lo; \
})
// MCATS code end

tm_time_t last_tuning_time; \

#  define TM_STARTUP(numThread, bId) { \
		assert(numThread == NUMBER_THREADS); \
		printf("startup num_threads = %lu\n", NUMBER_THREADS); \
		fflush(stdout); \
		concurrency_window_size = NUMBER_CORES; \
		last_cycle_timestamp = CURRENT_TIMESTAMP(); \
		concurrency_window_size = NUMBER_THREADS > 1 ? 2 : 1; \
		int t_index; \
		for (t_index = concurrency_window_size; t_index < NUMBER_THREADS; t_index++) { \
			gated[t_index] = t_index < concurrency_window_size ? 0 : 1; \
		} \
		min_cycle_duration = 0; \
		max_cycle_duration = 0; \
		avg_cycle_duration = 0; \
		min_num_commits = 0; \
		max_num_commits = 0; \
		avg_num_commits = 0; \
		num_cycles = 0; \
		memset(thread_stats, 0, sizeof(thread_stats)); \
	}

#  define TM_SHUTDOWN() { \
	PRINT_SUMMARY_STATS(); \
}

#  define TM_THREAD_ENTER() { \
	printf("id: %i\tthread enter\n", myThreadId); \
}

#  define TM_THREAD_EXIT() { \
	printf("id: %i\tthread exit\n", myThreadId); \
}

#  define TM_BEGIN_WAIVER()
#  define TM_END_WAIVER()

#  define P_MALLOC(size)                malloc(size)
#  define P_FREE(ptr)                   free(ptr)
#  define TM_MALLOC(size)               malloc(size)
#  define TM_FREE(ptr)                  free(ptr)

# define SETUP_NUMBER_TASKS(n)
# define SETUP_NUMBER_THREADS(n)

#  define PRINT_STATS() { \
		printf("==================CYCLE STATS==================\n"); \
		printf("id = %i\tstate = %i\tcurrent_cwnd = %i\n", myThreadId, state, concurrency_window_size); \
		printf("Current Cycle Commits = %ld\tLast Cycle Commits = %ld\n", current_cycle_commits, last_cycle_commits); \
		printf("Cycle duration: %lu\n", TM_CYCLE_ETA()); \
		printf("===============================================\n"); \
	}

#  define PRINT_SUMMARY_STATS() { \
	printf("==============SUMMARY STATS==================\n"); \
	printf("DURATION: min = %lu\tmax = %lu\tavg = %lu\n", min_cycle_duration, max_cycle_duration, avg_cycle_duration/num_cycles); \
	printf("COMMITS: min = %lu\tmax = %lu\tavg = %lu\n", min_num_commits, max_num_commits, avg_num_commits/num_cycles); \
	int i; \
	printf("===============THREAD STATS==================\n"); \
	for (i = 0; i < NUMBER_THREADS; i++) { \
		printf("thread %i: %lu\t", thread_stats[i]); \
	} \
	printf("\n"); \
	printf("=============================================\n"); \
}

# define PRINT_CLOCK_THROUGHPUT(CLOCKS) { \
		printf("Clock throughtput = %f\n", (float)(1000000 * current_cycle_commits)/(float)(CLOCKS)); \
	}

# define IS_LOCKED(lock)        *((volatile int*)(&lock)) != 0

# define TASK_LOCKS 0

# define AL_LOCK(idx)

# define TM_BEGIN(b) { \
		TM_GATE(); \
    }

#define TM_GATE() { \
		int cycles = 0; \
		int rand_wait = rand() * 1000; \
		if (IS_LOCKED(gate_lock)) { \
        	while (IS_LOCKED(gate_lock)) { \
       	    	for (cycles = 0; cycles < rand_wait; cycles++) \
                __asm__ ( "pause;"); \
        	} \
 		} \
    	while (__sync_val_compare_and_swap(&gate_lock, 0, 1) == 1) { \
   	    	for (cycles = 0; cycles < rand_wait; cycles++) { \
            	__asm__ ("pause;"); \
        	} \
    	} \
		if (gated[myThreadId]) { \
			gate_lock = 0; \
			sem_wait(&gateSemaphore[myThreadId]); \
		} else { \
			gate_lock = 0; \
		} \
  	}

# define TM_END() { \
		if (myThreadId == 0) { \
			current_cycle_commits++; \
			if (TM_CYCLE_ETA() >= CYCLE_MILLIS) { \
				TM_NEW_CWND(); \
			} \
		} \
    }

# define TM_CYCLE_ETA() ({ \
    CURRENT_TIMESTAMP() - last_cycle_timestamp; \
}) 

# define TM_NEW_CWND() { \
	PRINT_STATS(); \
	if (num_cycles && !(num_cycles % 100)) { \
		PRINT_SUMMARY_STATS(); \
	} \
	int cycles = 0; \
	int rand_wait = rand() * 1000; \
	if (IS_LOCKED(gate_lock)) { \
        while (IS_LOCKED(gate_lock)) { \
       	    for (cycles = 0; cycles < rand_wait; cycles++) \
                __asm__ ( "pause;"); \
        } \
 	} \
    while (__sync_val_compare_and_swap(&gate_lock, 0, 1) == 1) { \
   	    for (cycles = 0; cycles < rand_wait; cycles++) { \
            __asm__ ("pause;"); \
        } \
    } \
    int plus_signal = current_cycle_commits < last_cycle_commits ? 0 : 1; \
	if (plus_signal) { \
		if (state == INCREASING) { \
			if (concurrency_window_size < NUMBER_THREADS) { \
	 			gated[concurrency_window_size] = 0; \
	 			sem_post(&gateSemaphore[concurrency_window_size]); \
				concurrency_window_size++; \
			} \
		} else { \
			if (concurrency_window_size > 1) { \
				gated[concurrency_window_size - 1] = 1; \
				concurrency_window_size--; \
			}; \
		} \
	} else { \
		if (state == INCREASING) { \
			if (concurrency_window_size > 1) { \
				gated[concurrency_window_size - 1] = 1; \
				concurrency_window_size--; \
			} \
			state = DECREASING; \
		} else { \
			if (concurrency_window_size < NUMBER_THREADS) { \
	 			gated[concurrency_window_size] = 0; \
	 			sem_post(&gateSemaphore[concurrency_window_size]); \
				concurrency_window_size++; \
			} \
			state = INCREASING; \
		} \
	} \
	gate_lock = 0; \
	num_cycles++; \
	min_num_commits = min(min_num_commits, current_cycle_commits); \
	max_num_commits = max(max_num_commits, current_cycle_commits); \
	avg_num_commits += current_cycle_commits; \
	int current_cycle_duration = TM_CYCLE_ETA(); \
	min_cycle_duration = min(min_cycle_duration, current_cycle_duration); \
	max_cycle_duration = max(max_cycle_duration, current_cycle_duration); \
	avg_cycle_duration += current_cycle_duration; \
	thread_stats[concurrency_window_size - 1]++; \
	last_cycle_timestamp = CURRENT_TIMESTAMP(); \
	last_cycle_commits = current_cycle_commits; \
	current_cycle_commits = 0; \
}



#  define TM_BEGIN_RO()                 TM_BEGIN(0)
#  define TM_RESTART()                  _xabort(0xab);
#  define TM_EARLY_RELEASE(var)

#  define TM_SHARED_READ(var)         (var)
#  define TM_SHARED_WRITE(var, val)   ({var = val; var;})
#  define TM_LOCAL_WRITE(var, val)    ({var = val; var;})


#  define TM_SHARED_READ(var)         (var)
#  define TM_SHARED_WRITE(var, val)   ({var = val; var;})

#  define TM_SHARED_READ_I(var)         (var)
#  define TM_SHARED_READ_L(var)         (var)
#  define TM_SHARED_READ_P(var)         (var)
#  define TM_SHARED_READ_F(var)         (var)

#  define TM_SHARED_WRITE_I(var, val)   ({var = val; var;})
#  define TM_SHARED_WRITE_L(var, val)   ({var = val; var;})
#  define TM_SHARED_WRITE_P(var, val)   ({var = val; var;})
#  define TM_SHARED_WRITE_F(var, val)   ({var = val; var;})

#  define TM_LOCAL_WRITE_I(var, val)    ({var = val; var;})
#  define TM_LOCAL_WRITE_L(var, val)    ({var = val; var;})
#  define TM_LOCAL_WRITE_P(var, val)    ({var = val; var;})
#  define TM_LOCAL_WRITE_F(var, val)    ({var = val; var;})

#endif