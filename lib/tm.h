#ifndef TM_H
#define TM_H 1

#  include <stdio.h>

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
#  include <math.h>

#  include <immintrin.h>
#  include <rtmintrin.h>

#  define TM_ARG                        /* nothing */
#  define TM_ARG_ALONE                  /* nothing */
#  define TM_ARGDECL                    /* nothing */
#  define TM_ARGDECL_ALONE              /* nothing */
#  define TM_CALLABLE                   /* nothing */

// MCATS code start

typedef unsigned long tm_time_t;

#define TM_TIMER_READ() ({ \
	unsigned int lo; \
	unsigned int hi; \
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi)); \
	((tm_time_t)hi) << 32 | lo; \
})
// MCATS code end

#  define TM_STARTUP(numThread, bId){ \
        benchmarkId = bId; \
        MAX_ATTEMPTS = TOTAL_ATTEMPTS; \
        APRIORI_ATTEMPTS = APRIORI_LOCK_ATTEMPTS; \
        printf("BenchId = %d\tNumThread = %d\tAttBefGlLock = %d\tAPriLockAtt = %d ",benchmarkId, numThread, MAX_ATTEMPTS, APRIORI_ATTEMPTS); \
}

#  define TM_SHUTDOWN()

#  define TM_THREAD_ENTER()

#  define TM_THREAD_EXIT()

#  define TM_BEGIN_WAIVER()
#  define TM_END_WAIVER()

#  define P_MALLOC(size)                malloc(size)
#  define P_FREE(ptr)                   free(ptr)
#  define TM_MALLOC(size)               malloc(size)
#  define TM_FREE(ptr)                  free(ptr)

# define SETUP_NUMBER_TASKS(n)
# define SETUP_NUMBER_THREADS(n)

# define PRINT_STATS() { \
	unsigned long total_aborts = 0;\
    unsigned long total_commits = 0; \
unsigned long aborted_txs = 0; \
        	int t; \
        	for (t = 0; t < NUMBER_THREADS; t++) { \
        		total_aborts += statistics[t].totalAborts; \
                total_commits += statistics[t].totalCommits; \
                aborted_txs += statistics[t].abortedTxs; \
        	} \
    printf("Commits = %ld\taborted = %ld\taborts = %ld ", total_commits, aborted_txs, total_aborts); \
}
# define IS_LOCKED(lock)        *((volatile int*)(&lock)) != 0

# define TASK_LOCKS 0

# define AL_LOCK(idx)


# define TM_BEGIN(b) { \
        thread_metadata_t* myStats = &statistics; \
        int cycles = 0; \
        int tries = MAX_ATTEMPTS; \
        while (1) { \
            if (IS_LOCKED(is_fallback)) { \
            	while (IS_LOCKED(is_fallback)) { \
            	    for (cycles = 0; cycles < myStats->wait_cycles; cycles++) \
                        __asm__ ( "pause;"); \
            	} \
            } \
            int status = _xbegin(); \
            if (status == _XBEGIN_STARTED) { break; } \
            if (tries == MAX_ATTEMPTS) myStats->abortedTxs++; \
            tries--; \
            myStats->totalAborts++; \
            if (tries <= 0) {   \
                while (__sync_val_compare_and_swap(&is_fallback, 0, 1) == 1) { \
                    for (cycles = 0; cycles < myStats->wait_cycles; cycles++) { \
                        __asm__ ("pause;"); \
                    } \
                } \
                break; \
            } \
        }

# define TM_BEGIN_STM(b) { \
        thread_metadata_t* myStats = &statistics; \
        int cycles = 0; \
        int tries = MAX_ATTEMPTS; \
        while (1) { \
            if (IS_LOCKED(is_fallback)) { \
            	while (IS_LOCKED(is_fallback)) { \
            	    for (cycles = 0; cycles < myStats->wait_cycles; cycles++) \
                        __asm__ ( "pause;"); \
            	} \
            } \
            while (__sync_val_compare_and_swap(&is_fallback, 0, 1) == 1) { \
                for (cycles = 0; cycles < myStats->wait_cycles; cycles++) { \
                    __asm__ ("pause;"); \
                } \
            } \
            break; \
        }


# define TM_END() \
	if (tries > 0) { \
        if (IS_LOCKED(is_fallback)) { _xabort(30); } \
		_xend(); \
    } else {    \
        is_fallback = 0; \
    } \
    myStats->totalCommits++; \
};


# define TM_END_STM() \
    is_fallback = 0; \
    myStats->totalCommits++; \
};




#    define TM_BEGIN_RO()                 TM_BEGIN(0)
#    define TM_RESTART()                  _xabort(0xab);
#    define TM_EARLY_RELEASE(var)

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
