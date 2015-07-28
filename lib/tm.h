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
        current_collector_thread_id=0; \
        tx_cluster_table[0][1]=1; \
        MAX_ATTEMPTS = TOTAL_ATTEMPTS; \
        APRIORI_ATTEMPTS = APRIORI_LOCK_ATTEMPTS; \
        TXS_PER_MCATS_TUNING_CYCLE = TXS_PER_TUNING_CYCLE/NUMBER_THREADS; \
    	RESET_STATS(); \
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



#define TM_WAIT() { \
    thread_metadata_t* myStats = &(statistics[myThreadId]); \
	int active_txs,entered=0; \
	tm_time_t start_spin_time; \
	active_txs=tx_cluster_table[0][0]; \
	if(active_txs<tx_cluster_table[0][1]){ \
		printf("\nThread %i entered1, txs %i, active_txs %i", myThreadId, myStats->commits_per_tuning_cycle, active_txs); \
		fflush(stdout); \
		if (__sync_val_compare_and_swap(&tx_cluster_table[0][0], active_txs, active_txs+1) == active_txs) { \
			if(myStats->i_am_the_collector_thread){ \
				myStats->first_start_tx_time=myStats->last_start_tx_time=start_spin_time=TM_TIMER_READ(); \
				myStats->total_no_tx_time_per_tuning_cycle+=start_spin_time - myStats->start_no_tx_time; \
			} \
			fflush(stdout); \
			entered=1; \
		} \
	} \
	if (entered==0){ \
		if(myStats->i_am_the_collector_thread==1){ \
			start_spin_time=TM_TIMER_READ(); \
			myStats->total_no_tx_time_per_tuning_cycle+=start_spin_time - myStats->start_no_tx_time; \
		} \
		int wait_cycles=myStats->wait_cycles, i=1; \
		while(1) { \
			active_txs=tx_cluster_table[0][0]; \
			printf("\nThread %i waiting, txs %i, tx_cluster_table[0][0] %i", myThreadId, myStats->commits_per_tuning_cycle, tx_cluster_table[0][0]); \
			fflush(stdout); \
			getchar(); \
			if(active_txs<tx_cluster_table[0][1]) \
				if (__sync_val_compare_and_swap(&tx_cluster_table[0][0], active_txs, active_txs+1) == active_txs) { \
					printf("\nThread %i entered2, txs %i, active_txs %i", myThreadId, myStats->commits_per_tuning_cycle, active_txs); \
					fflush(stdout); \
					break; \
				} \
				myStats->i_am_waiting=1; \
			for(i=0;i<wait_cycles;i++){ \
				if(myStats->i_am_waiting==0) break; \
			} \
			myStats->i_am_waiting=0; \
		} \
	} \
	if (myStats->i_am_the_collector_thread==1){ \
		if (entered==0) myStats->first_start_tx_time=myStats->last_start_tx_time=TM_TIMER_READ(); \
		myStats->start_no_tx_time=0; \
		myStats->total_spin_time_per_tuning_cycle+=myStats->first_start_tx_time-start_spin_time; \
	} \
	getchar(); \
}

#define TUNE_MCATS() { \
	printf("\nTuning ... Current_collector_thread_id %i", current_collector_thread_id); \
	int t; \
	for (t = 0; t < NUMBER_THREADS; t++) { \
		printf("\nThread %i",t); \
		printf("\ntotal_useful_time_per_tuning_cycle %llu",statistics[t].total_useful_time_per_tuning_cycle); \
		printf("\ntotal_no_tx_time_per_tuning_cycle %llu",statistics[t].total_no_tx_time_per_tuning_cycle); \
		printf("\ntotal_wasted_time_per_tuning_cycle %llu",statistics[t].total_wasted_time_per_tuning_cycle); \
		printf("\ntotal_spin_time_per_tuning_cycle %llu",statistics[t].total_spin_time_per_tuning_cycle=0); \
		printf("\ncommits_per_tuning_cycle %llu",statistics[t].commits_per_tuning_cycle); \
		printf("\naborted_txs_per_tuning_cycle %llu",statistics[t].aborted_txs_per_tuning_cycle); \
		printf("\naborts_per_tuning_cycle %llu",statistics[t].aborts_per_tuning_cycle); \
	} \
	fflush(stdout); \
};

#define RESET_STATS() { \
		int t,s; \
		for (t = 0; t < NUMBER_THREADS; t++) { \
			for (s = 0; s < NUMBER_THREADS; s++) { \
				statistics[t].total_wasted_time_per_active_transactions_per_tuning_cycle[s]=0; \
				statistics[t].total_useful_time_per_active_transactions_per_tuning_cycle[s]=0; \
				statistics[t].total_committed_txs_per_active_transactions_per_tuning_cycle[s]=0; \
				statistics[t].total_aborted_txs_per_active_transactions_per_tuning_cycle[s]=0; \
			} \
			statistics[t].total_useful_time_per_tuning_cycle=0; \
			statistics[t].total_no_tx_time_per_tuning_cycle=0; \
			statistics[t].total_wasted_time_per_tuning_cycle=0; \
			statistics[t].total_spin_time_per_tuning_cycle=0; \
			statistics[t].commits_per_tuning_cycle=0; \
			statistics[t].aborted_txs_per_tuning_cycle=0; \
			statistics[t].aborts_per_tuning_cycle=0; \
		} \
	}

#define TM_SIGNAL() { \
        thread_metadata_t* myStats = &(statistics[myThreadId]); \
		printf("\nThread %i signaling, txs %i", myThreadId, myStats->commits_per_tuning_cycle); \
		fflush(stdout); \
		if (myStats->i_am_the_collector_thread==1){ \
			fflush(stdout); \
			myStats->start_no_tx_time=TM_TIMER_READ(); \
			int active_txs=tx_cluster_table[0][0]; \
            while ((__sync_val_compare_and_swap(&tx_cluster_table[0][0], active_txs, active_txs-1) != active_txs)) { \
                for (cycles = 0; cycles < myStats->wait_cycles; cycles++) { \
                    __asm__ ("pause;"); \
                } \
				active_txs=tx_cluster_table[0][0]; \
            } \
		    myStats->commits_per_tuning_cycle++; \
			tm_time_t useful_time = myStats->start_no_tx_time - myStats->last_start_tx_time; \
			myStats->total_wasted_time_per_active_transactions_per_tuning_cycle[active_txs]+=myStats->last_start_tx_time-myStats->first_start_tx_time; \
			myStats->total_useful_time_per_active_transactions_per_tuning_cycle[active_txs]+=useful_time; \
			myStats->total_committed_txs_per_active_transactions_per_tuning_cycle[active_txs]++; \
			myStats->total_useful_time_per_tuning_cycle+=useful_time; \
			if(myStats->commits_per_tuning_cycle==TXS_PER_MCATS_TUNING_CYCLE){ \
				if(myThreadId==NUMBER_THREADS - 1) { \
					TUNE_MCATS(); \
					RESET_STATS(); \
					} \
				current_collector_thread_id =(current_collector_thread_id + 1)% NUMBER_THREADS; \
				myStats->i_am_the_collector_thread=0; \
			} \
		}else if(current_collector_thread_id==myThreadId){ \
			fflush(stdout); \
			myStats->start_no_tx_time=TM_TIMER_READ(); \
			int active_txs=tx_cluster_table[0][0]; \
            while ((__sync_val_compare_and_swap(&tx_cluster_table[0][0], active_txs, active_txs-1) != active_txs)) { \
                for (cycles = 0; cycles < myStats->wait_cycles; cycles++) { \
                    __asm__ ("pause;"); \
                } \
				active_txs=tx_cluster_table[0][0]; \
            } \
			myStats->i_am_the_collector_thread=1; \
		} else { \
			fflush(stdout); \
			int active_txs=tx_cluster_table[0][0]; \
            while ((__sync_val_compare_and_swap(&tx_cluster_table[0][0], active_txs, active_txs-1) != active_txs)) { \
                for (cycles = 0; cycles < myStats->wait_cycles; cycles++) { \
                    __asm__ ("pause;"); \
                } \
				active_txs=tx_cluster_table[0][0]; \
            } \
		} \
		int t; \
    	for (t = 0; t < NUMBER_THREADS; t++) { \
			if(statistics[t].i_am_waiting==1){ \
				statistics[t].i_am_waiting=0; \
				break; \
			} \
    	} \
		printf("\nThread %i exited, txs %i", myThreadId, myStats->commits_per_tuning_cycle); \
		fflush(stdout); \
	}


# define TM_BEGIN_(b) { \
        thread_metadata_t* myStats = &(statistics[myThreadId]); \
        int cycles = 0; \
        int tries = MAX_ATTEMPTS; \
        TM_WAIT(); \
        myStats->first_tx_run=1; \
        while (1) { \
            if (IS_LOCKED(is_fallback)) { \
            	while (IS_LOCKED(is_fallback)) { \
            	    for (cycles = 0; cycles < myStats->wait_cycles; cycles++) { \
                        __asm__ ( "pause;"); \
            	    } \
            		if(myStats->i_am_the_collector_thread){ \
            			unsigned int last_timer_value=TM_TIMER_READ(); \
            			myStats->total_tx_wasted_per_active_transactions[tx_cluster_table[0][0]]+=last_timer_value - myStats->last_start_tx_time; \
            			myStats->last_start_tx_time=last_timer_value; \
            		} \
            	} \
            } \
            int status = _xbegin(); \
    		if(myStats->i_am_the_collector_thread && !myStats->first_tx_run){ \
    			unsigned int last_timer_value=TM_TIMER_READ(); \
    			myStats->total_tx_wasted_per_active_transactions[tx_cluster_table[0][0]+1]+=last_timer_value - myStats->last_start_tx_time; \
    			myStats->last_start_tx_time=last_timer_value; \
    		} \
			myStats->first_tx_run=0; \
            if (status == _XBEGIN_STARTED) { break; } \
            if (tries == MAX_ATTEMPTS) { \
            	myStats->abortedTxs++; \
                myStats->aborted_txs_per_tuning_cycle++; \
            } \
            tries--; \
            myStats->totalAborts++; \
            myStats->aborts_per_tuning_cycle++; \
            if (tries <= 0) {   \
                while (__sync_val_compare_and_swap(&is_fallback, 0, 1) == 1) { \
                    for (cycles = 0; cycles < myStats->wait_cycles; cycles++) { \
                        __asm__ ("pause;"); \
                    } \
            		if(myStats->i_am_the_collector_thread){ \
            			unsigned int last_timer_value=TM_TIMER_READ(); \
            			myStats->total_tx_wasted_per_active_transactions[tx_cluster_table[0][0]]+=last_timer_value - myStats->last_start_tx_time; \
            			myStats->last_start_tx_time=last_timer_value; \
            		} \
                } \
                break; \
            } \
        }

# define TM_BEGIN(b) { \
        thread_metadata_t* myStats = &statistics; \
        int cycles = 0; \
        int tries = MAX_ATTEMPTS; \
        TM_WAIT(); \
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


# define TM_END_() \
	if (tries > 0) { \
        if (IS_LOCKED(is_fallback)) { _xabort(30); } \
		_xend(); \
    } else {    \
        is_fallback = 0; \
    } \
    myStats->totalCommits++; \
    myStats->commits_per_tuning_cycle; \\
    TM_SIGNAL(); \
};


# define TM_END() \
    is_fallback = 0; \
    myStats->totalCommits++; \
    TM_SIGNAL(); \
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
