
#ifndef TM_H
#define TM_H 1


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

typedef enum {
	LOCK_ONLY,
	F2C2
} tm_mode_t;

__attribute__((aligned(64))) unsigned int concurrency_window_size;
__attribute__((aligned(64))) unsigned int min_concurrency_window_size;
__attribute__((aligned(64))) unsigned int max_concurrency_window_size;
__attribute__((aligned(64))) unsigned long long avg_concurrency_window_size;
__attribute__((aligned(64))) unsigned long startup_timestamp;
__attribute__((aligned(64))) unsigned long last_cycle_timestamp;
__attribute__((aligned(64))) unsigned int last_cycle_commits;
__attribute__((aligned(64))) unsigned int current_cycle_commits;
__attribute__((aligned(64))) unsigned int gated[NUMBER_THREADS];
__attribute__((aligned(64))) sem_t gateSemaphore[NUMBER_THREADS];
__attribute__((aligned(64))) unsigned long  min_cycle_duration;
__attribute__((aligned(64))) unsigned long  max_cycle_duration;
__attribute__((aligned(64))) unsigned long  avg_cycle_duration;
__attribute__((aligned(64))) unsigned long min_num_commits;
__attribute__((aligned(64))) unsigned long max_num_commits;
__attribute__((aligned(64))) unsigned long avg_num_commits;
__attribute__((aligned(64))) unsigned int aborts;
__attribute__((aligned(64))) unsigned long long num_cycles;
__attribute__((aligned(64))) unsigned long long thread_stats[NUMBER_THREADS];
__attribute__((aligned(64))) tm_state_t state;
__attribute__((aligned(64))) tm_mode_t mode;
__attribute__((aligned(64))) unsigned int max_attempts;
__attribute__((aligned(64))) unsigned int tries[NUMBER_THREADS];
__attribute__((aligned(64))) unsigned int current_cycle_locks;

__attribute__((aligned(64))) static volatile unsigned long gate_lock = 0;


#define CURRENT_TIMESTAMP() ({ \
	struct timeval  tv; \
	gettimeofday(&tv, NULL); \
	unsigned long time_in_mill = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ; \         	
	time_in_mill; \
})

#define CYCLE_MILLIS 50

#define NUMBER_CORES sysconf(_SC_NPROCESSORS_ONLN)

#define SET_MODE(m) { \
	mode = m; \
	printf("Mode set to %s\n", mode == LOCK_ONLY ? "LOCK_ONLY" : "F2C2"); \
}

typedef unsigned long tm_time_t;

#define TM_TIMER_READ() ({ \
	unsigned int lo; \
	unsigned int hi; \
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi)); \
	((tm_time_t)hi) << 32 | lo; \
})

#  define TM_STARTUP(numThread, bId) { \
		assert(numThread == NUMBER_THREADS); \
		SET_MODE(F2C2); \
		printf("startup num_threads = %lu\n", NUMBER_THREADS); \
		concurrency_window_size = NUMBER_CORES; \
		startup_timestamp = CURRENT_TIMESTAMP(); \
		last_cycle_timestamp = startup_timestamp; \
		concurrency_window_size = 1; \
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
		aborts = 0; \
		min_concurrency_window_size = 0; \
		max_concurrency_window_size = 0; \
		avg_concurrency_window_size = 0; \
		num_cycles = 0; \
		max_attempts = TOTAL_ATTEMPTS; \
		current_cycle_locks = 0; \
		memset(thread_stats, 0, sizeof(thread_stats)); \
		memset(tries, 0, sizeof(tries)); \
	}

#  define TM_SHUTDOWN() { \
	PRINT_SUMMARY_STATS(); \
}

#  define TM_THREAD_ENTER() { \
	printf("id: %i\tthread enter\n", myThreadId); \
}

#  define TM_THREAD_EXIT() { \
	printf("id: %i\tthread exit\n", myThreadId); \
	if (mode == F2C2) { \
		int index; \
		for (index = 1; index < NUMBER_THREADS; index++) { \
			gated[index] = 0; \
			sem_post(&gateSemaphore[index]); \
		} \
		concurrency_window_size = NUMBER_THREADS; \
	} \
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
		printf("id = %i\tstate = %i\tcurrent_cwnd = %u\tthreads = %i\taborts = %lu\tlocks = %lu\n", myThreadId, state, concurrency_window_size, NUMBER_THREADS, aborts, current_cycle_locks); \
		printf("Current Cycle Commits = %u\tLast Cycle Commits = %u\n", current_cycle_commits, last_cycle_commits); \
		printf("Cycle duration = %lums\tOverall duration = %lums\n", TM_CYCLE_ETA(), TM_OVERALL_ETA()); \
		printf("Chart_data\t%u\t%u\t%lu\t%i\t%s\t%lu\t%lu\n", current_cycle_commits, concurrency_window_size, TM_OVERALL_ETA(), NUMBER_THREADS, mode == F2C2 ? "F2C2" : "LOCK_ONLY", aborts, current_cycle_locks); \
		printf("===============================================\n"); \
	}

#  define PRINT_SUMMARY_STATS() { \
	printf("==============SUMMARY STATS==================\n"); \
	printf("DURATION: min = %u\tmax = %u\tavg = %u\n", min_cycle_duration, max_cycle_duration, avg_cycle_duration/num_cycles); \
	printf("COMMITS: min = %u\tmax = %u\tavg = %u\n", min_num_commits, max_num_commits, avg_num_commits/num_cycles); \
	printf("WINDOW: min = %u\tmax = %u\tavg = %.3f\n", min_concurrency_window_size, max_concurrency_window_size, avg_concurrency_window_size/(float)num_cycles); \
	int i; \
	printf("===============THREAD STATS==================\n"); \
	for (i = 0; i < NUMBER_THREADS; i++) { \
		printf("thread %i: %u\t",i ,thread_stats[i]); \
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
		if (mode == F2C2) { \
        	TM_GATE(); \
        } \
        tries[myThreadId] = max_attempts; \
        while (1) { \
            if (IS_LOCKED(is_fallback)) { \
                while (IS_LOCKED(is_fallback)) { \
                    __asm__ ( "pause;"); \
                } \
            } \
            int status = _xbegin(); \
            tries[myThreadId]--; \
            if (!myThreadId) { \
            	aborts++; \
            } \
            if (tries[myThreadId] <= 0) { \
            	if (!myThreadId) { \
            		current_cycle_locks++; \
            	} \
                while (__sync_val_compare_and_swap(&is_fallback, 0, 1) == 1) { \
                    __asm__ ("pause;"); \
                } \
                break; \
            } \
        } \
    }

#define TM_GATE() { \
		if (gated[myThreadId]) { \
			sem_wait(&gateSemaphore[myThreadId]); \
		} \
  	}

# define TM_END() { \
		if (tries[myThreadId] > 0) { \
        	if (IS_LOCKED(is_fallback)) { if (!myThreadId) { aborts++; } _xabort(30); } \
			_xend(); \
    	} else {    \
        	is_fallback = 0; \
    	} \
		if (!myThreadId) { \
			current_cycle_commits++; \
			if (TM_CYCLE_ETA() >= CYCLE_MILLIS) { \
				if(mode == F2C2) { \
					TM_NEW_CWND(); \
				} else { \
					PRINT_STATS(); \
					REFRESH_STATS(); \
				} \
			} \
		} \
    }

# define TM_OVERALL_ETA() ({ \
    CURRENT_TIMESTAMP() - startup_timestamp; \
})

# define TM_CYCLE_ETA() ({ \
    CURRENT_TIMESTAMP() - last_cycle_timestamp; \
}) 

# define TM_NEW_CWND() { \
	PRINT_STATS(); \
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
	REFRESH_STATS(); \
}

# define REFRESH_STATS() { \
	num_cycles++; \
	min_num_commits = min(min_num_commits, current_cycle_commits); \
	max_num_commits = max(max_num_commits, current_cycle_commits); \
	avg_num_commits += current_cycle_commits; \
	int current_cycle_duration = TM_CYCLE_ETA(); \
	min_cycle_duration = min(min_cycle_duration, current_cycle_duration); \
	max_cycle_duration = max(max_cycle_duration, current_cycle_duration); \
	avg_cycle_duration += current_cycle_duration; \
	min_concurrency_window_size = min(min_concurrency_window_size, concurrency_window_size); \
	max_concurrency_window_size = max(max_concurrency_window_size, concurrency_window_size); \
	avg_concurrency_window_size += concurrency_window_size; \
	thread_stats[concurrency_window_size - 1]++; \
	last_cycle_timestamp = CURRENT_TIMESTAMP(); \
	last_cycle_commits = current_cycle_commits; \
	current_cycle_commits = 0; \
	current_cycle_locks = 0; \
	aborts = 0; \
}



#  define TM_BEGIN_RO()                 TM_BEGIN(0)
#  define TM_RESTART()                  { if (!myThreadId) { aborts++; } _xabort(0xab); }
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