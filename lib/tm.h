
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
	PROBE,
	THROTTLE
} tm_policy_t;

typedef enum {
	UP,
	DOWN
} tm_direction_t;

__attribute__((aligned(64))) static volatile unsigned long begin_lock = 0;
__attribute__((aligned(64))) unsigned int active_count;
__attribute__((aligned(64))) unsigned int commits;
__attribute__((aligned(64))) unsigned int aborts;
__attribute__((aligned(64))) unsigned int quota;
__attribute__((aligned(64))) unsigned int stalled;
__attribute__((aligned(64))) unsigned int peak;
__attribute__((aligned(64))) unsigned int laps;
__attribute__((aligned(64))) unsigned int last_commits;
__attribute__((aligned(64))) unsigned int last_laps;
__attribute__((aligned(64))) tm_direction_t direction;
__attribute__((aligned(64))) tm_policy_t policy;
__attribute__((aligned(64))) pthread_t * daemon_thread;
__attribute__((aligned(64))) unsigned int num_interval;
__attribute__((aligned(64))) unsigned int min_num_commits;
__attribute__((aligned(64))) unsigned int max_num_commits;
__attribute__((aligned(64))) unsigned long long avg_num_commits;
__attribute__((aligned(64))) unsigned int min_num_aborts;
__attribute__((aligned(64))) unsigned int max_num_aborts;
__attribute__((aligned(64))) unsigned long long avg_num_aborts;
__attribute__((aligned(64))) unsigned int min_num_laps;
__attribute__((aligned(64))) unsigned int max_num_laps;
__attribute__((aligned(64))) unsigned long long avg_num_laps;
__attribute__((aligned(64))) unsigned int min_quota;
__attribute__((aligned(64))) unsigned int max_quota;
__attribute__((aligned(64))) unsigned long long avg_quota;
__attribute__((aligned(64))) unsigned long last_cycle_timestamp;
__attribute__((aligned(64))) unsigned long startup_timestamp;
__attribute__((aligned(64))) unsigned long min_duration;
__attribute__((aligned(64))) unsigned long max_duration;
__attribute__((aligned(64))) unsigned long avg_duration;
__attribute__((aligned(64))) unsigned int max_attempts;
__attribute__((aligned(64))) unsigned int current_cycle_locks;
__attribute__((aligned(64))) unsigned int tries[NUMBER_THREADS];


#define CURRENT_TIMESTAMP() ({ \
	struct timeval  tv; \
	gettimeofday(&tv, NULL); \
	unsigned long time_in_mill = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ; \         	
	time_in_mill; \
})

#define TM_OVERALL_ETA() ({ \
	CURRENT_TIMESTAMP() - startup_timestamp; \
})

# define TM_CYCLE_ETA() ({ \
    CURRENT_TIMESTAMP() - last_cycle_timestamp; \
})

#define CYCLE_MILLIS 50
#define INTERVAL_MICROSECS 50000
#define WARMUP 1
#define THRESHOLD 0.8

#define NUMBER_CORES sysconf(_SC_NPROCESSORS_ONLN)

typedef unsigned long tm_time_t;

#define TM_TIMER_READ() ({ \
	unsigned int lo; \
	unsigned int hi; \
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi)); \
	((tm_time_t)hi) << 32 | lo; \
})

#  define PRINT_STATS() { \
		printf("==================INTERVAL STATS==================\n"); \
		printf("id = %i\tinterval = %u\tquota = %u\tstalled = %i\tETA = %lu\n", myThreadId, num_interval, quota, stalled, TM_OVERALL_ETA()); \
		printf("peak = %u\tcommits = %u\tactive count = %u\tthreads = %i\taborts = %lu\tlocks = %lu\n", peak, commits, active_count, NUMBER_THREADS, aborts, current_cycle_locks); \
		printf("commits -> min = %u\t max = %u\tavg = %u\n", min_num_commits, max_num_commits, avg_num_commits/num_interval); \
		printf("aborts -> min = %u\t max = %u\tavg = %u\n", min_num_aborts, max_num_aborts, avg_num_aborts/num_interval); \
		printf("quota -> min = %u\t max = %u\tavg = %u\n", min_quota, max_quota, avg_quota/num_interval); \
		printf("duration -> min = %lu\t max = %lu\tavg = %lu\n", min_duration, max_duration, avg_duration/num_interval); \
		if (policy == PROBE) { \
			printf("laps -> min = %u\t max = %u\tavg = %u\n", min_num_laps, max_num_laps, avg_num_laps/num_interval); \
			printf("probe direction = %s\n", direction == UP ? "UP" : "DOWN"); \
		} \
		printf("Chart_data\t%u\t%u\t%lu\t%i\t%s\t%lu\t%lu\n", commits, active_count, TM_OVERALL_ETA(), NUMBER_THREADS, policy == PROBE ? "PROBE" : "THROTTLE", aborts, current_cycle_locks); \
		printf("==================================================\n"); \
	}

#  define PROBE_POLICY() { \
	if (!peak && !active_count) { \
		return; \
	} else if (commits + aborts < WARMUP) { \
		laps++; \
		return; \
	} \
	laps++; \
	num_interval++; \
	if (!last_laps) { \
		printf("Last laps = 0!"); \
	} \
	if (peak < quota) { \
		quota = peak + 1; \
		direction = DOWN; \
	} else if (quota == 1) { \
		direction = UP; \
	} else if ((float)commits/laps < (float)last_commits/last_laps) { \
		direction = direction == UP ? DOWN : UP; \
	} \
	if (direction == DOWN) { \
		quota--; \
	} else { \
		quota++; \
	} \
	min_num_commits = min(min_num_commits, commits); \
	max_num_commits = max(max_num_commits, commits); \
	avg_num_commits += commits; \
	min_num_aborts = min(min_num_aborts, aborts); \
	max_num_aborts = max(max_num_aborts, aborts); \
	avg_num_aborts += aborts; \
	min_quota = min(min_quota, quota); \
	max_quota = max(max_quota, quota); \
	avg_quota += quota; \
	min_num_laps = min(min_num_laps, laps); \
	max_num_laps = max(max_num_laps, laps); \
	avg_num_laps += laps; \
	min_duration = min(min_duration, TM_CYCLE_ETA()); \
	max_duration = max(max_duration, TM_CYCLE_ETA()); \
	avg_duration += TM_CYCLE_ETA(); \
	PRINT_STATS(); \
	last_commits = commits; \
	last_laps = laps; \
	peak = 0; \
	commits = 0; \
	aborts = 0; \
	laps = 0; \
	current_cycle_locks = 0; \
}

#  define THROTTLE_POLICY() { \
	if (commits < WARMUP) { \
		return; \
	} \
	num_interval++; \
	float ratio = (float)commits/(commits + aborts); \
	if (peak < quota) { \
		quota = peak; \
	} else if (ratio < THRESHOLD) { \
		quota--; \
	} else if (stalled) { \
		quota++; \
	} \
	min_num_commits = min(min_num_commits, commits); \
	max_num_commits = max(max_num_commits, commits); \
	avg_num_commits += commits; \
	min_num_aborts = min(min_num_aborts, aborts); \
	max_num_aborts = max(max_num_aborts, aborts); \
	avg_num_aborts += aborts; \
	min_quota = min(min_quota, quota); \
	max_quota = max(max_quota, quota); \
	avg_quota += quota; \
	min_duration = min(min_duration, TM_CYCLE_ETA()); \
	max_duration = max(max_duration, TM_CYCLE_ETA()); \
	avg_duration += TM_CYCLE_ETA(); \
	PRINT_STATS(); \
	peak = 0; \
	commits = 0; \
	aborts = 0; \
	stalled = 0; \
	current_cycle_locks = 0; \
}

#  define SET_POLICY(p) { \
	policy = p; \
	printf("Startup: Policy has been set to %s\n", policy ? "THROTTLE" : "PROBE"); \
}

#  define TM_STARTUP(numThread, bId) { \
		assert(numThread == NUMBER_THREADS); \
		SET_POLICY(THROTTLE); \
		max_attempts = TOTAL_ATTEMPTS; \
		active_count = 0; \
		quota = 1; \
		peak = 0; \
		commits = 0; \
		aborts = 0; \
		stalled = 0; \
		num_interval = 0; \
		min_num_commits = 0; \
		max_num_commits = 0; \
		avg_num_commits = 0; \
		min_quota = 0; \
		max_quota = 0; \
		avg_quota = 0; \
		min_duration = 0; \
		max_duration = 0; \
		avg_duration = 0; \
		current_cycle_locks = 0; \
		last_cycle_timestamp = CURRENT_TIMESTAMP(); \
		memset(tries, 0, sizeof(tries)); \
	}

#  define TM_SHUTDOWN()

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

# define PRINT_CLOCK_THROUGHPUT(CLOCKS) { \
		printf("Clock throughtput = %f\n", (float)(1000000 * commits)/(float)(CLOCKS)); \
	}

# define IS_LOCKED(lock)        *((volatile int*)(&lock)) != 0

# define TASK_LOCKS 0

# define AL_LOCK(idx)

# define TM_BEGIN(b) { \
		int active_txs; \
		while(1) { \
			active_txs=active_count; \
			if(active_txs<quota || !myThreadId) { \
				if (__sync_bool_compare_and_swap(&active_count, active_txs, active_txs+1)) { \
					peak = max(peak, active_count); \
					break; \
				} \
			} else { \
				stalled = 1; \
			} \
			__asm__ ("pause;"); \
		} \
		tries[myThreadId] = max_attempts; \
		while (1) { \
            if (IS_LOCKED(is_fallback)) { \
                while (IS_LOCKED(is_fallback)) { \
                    __asm__ ( "pause;"); \
                } \
            } \
            int status = _xbegin(); \
            if (status == _XBEGIN_STARTED) { break; } \
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

# define TM_END() { \
		if (tries[myThreadId] > 0) { \
        	if (IS_LOCKED(is_fallback)) { if (!myThreadId) { aborts++; } _xabort(30); } \
			_xend(); \
    	} else {    \
        	is_fallback = 0; \
    	} \
    	int active_txs; \
    	while (1) { \
    		active_txs = active_count; \
    		if (__sync_bool_compare_and_swap(&active_count, active_txs, active_txs - 1)) { \
    			break; \
            } else { \
				__asm__ ("pause;"); \
            } \
        } \
        if (!myThreadId) { \
			commits++; \
			if (TM_CYCLE_ETA() >= CYCLE_MILLIS) { \
				if(policy == PROBE) { \
					PROBE_POLICY(); \
				} else { \
					THROTTLE_POLICY(); \
				} \
				last_cycle_timestamp = CURRENT_TIMESTAMP(); \
			} \
		} \
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