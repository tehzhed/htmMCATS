
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

typedef struct thread_metadata {
    long totalAborts;
    long abortedTxs;
    long totalCommits;
    long i_am_the_collector_thread;
    long i_am_waiting;
    long first_tx_run;
    char suffixPadding[64];
    unsigned long updateStatsCounter;
    // MCATS code start

    unsigned long total_wasted_time_per_active_transactions_per_tuning_cycle[NUMBER_THREADS+1];
    unsigned long total_useful_time_per_active_transactions_per_tuning_cycle[NUMBER_THREADS+1];
    unsigned long total_committed_txs_per_active_transactions_per_tuning_cycle[NUMBER_THREADS+1];
    unsigned long total_aborted_txs_per_active_transactions_per_tuning_cycle[NUMBER_THREADS+1];

    unsigned long total_useful_time_per_tuning_cycle;
    unsigned long total_no_tx_time_per_tuning_cycle;
    unsigned long total_wasted_time_per_tuning_cycle;
    unsigned long total_spin_time_per_tuning_cycle;
    unsigned long first_start_tx_time;
    unsigned long last_start_tx_time;
    unsigned long start_no_tx_time;
    unsigned long wait_cycles;
    unsigned long commits_per_tuning_cycle;
    unsigned long aborted_txs_per_tuning_cycle;
	unsigned long aborts_per_tuning_cycle;
    // MCATS code end

} __attribute__((aligned(64))) thread_metadata_t;

__attribute__((aligned(64))) thread_metadata_t statistics[NUMBER_THREADS];
__attribute__((aligned(64))) static volatile unsigned long tx_cluster_table[NUMBER_ATOMIC_BLOCKS][2];
__attribute__((aligned(64))) unsigned long runs_limit;
__attribute__((aligned(64))) unsigned long main_thread;
__attribute__((aligned(64))) unsigned long current_collector_thread;
__attribute__((aligned(64))) float lambda;
__attribute__((aligned(64))) float *mu;
__attribute__((aligned(64))) unsigned long m;
__attribute__((aligned(64))) float predicted_throughput;

typedef unsigned long tm_time_t;

#define TM_TIMER_READ() ({ \
	unsigned int lo; \
	unsigned int hi; \
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi)); \
	((tm_time_t)hi) << 32 | lo; \
})
// MCATS code end

tm_time_t last_tuning_time; \

#  define TM_STARTUP(numThread, bId){ \
        benchmarkId = bId; \
        current_collector_thread_id=0; \
        tx_cluster_table[0][1]=NUMBER_THREADS; \
        MAX_ATTEMPTS = TOTAL_ATTEMPTS; \
        APRIORI_ATTEMPTS = APRIORI_LOCK_ATTEMPTS; \
        TXS_PER_MCATS_TUNING_CYCLE = TXS_PER_TUNING_CYCLE/NUMBER_THREADS; \
        int i; \
		for (i = 0; i < NUMBER_THREADS; i++) { \
			statistics[i].totalCommits = 0; \
			statistics[i].abortedTxs = 0; \
			statistics[i].totalAborts = 0; \
		} \
    	RESET_MCATS_STATS(); \
    	last_tuning_time=TM_TIMER_READ(); \
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


# define PRINT_CLOCK_THROUGHPUT(CLOCKS) { \
	unsigned long total_commits = 0; \
	int t; \
	for (t = 0; t < NUMBER_THREADS; t++) { \
		total_commits += statistics[t].totalCommits; \
	} \
	printf("clock throghput = %f ", (float)(1000000 *total_commits)/(float)(CLOCKS)); \
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
		if (__sync_val_compare_and_swap(&tx_cluster_table[0][0], active_txs, active_txs+1) == active_txs) { \
			if(myStats->i_am_the_collector_thread){ \
				myStats->first_start_tx_time=myStats->last_start_tx_time=TM_TIMER_READ(); \
				myStats->total_no_tx_time_per_tuning_cycle+=myStats->first_start_tx_time - myStats->start_no_tx_time; \
			} \
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
			if(active_txs<tx_cluster_table[0][1]) \
				if (__sync_val_compare_and_swap(&tx_cluster_table[0][0], active_txs, active_txs+1) == active_txs) { \
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
		if (entered==0) { \
			myStats->first_start_tx_time=myStats->last_start_tx_time=TM_TIMER_READ(); \
			myStats->total_spin_time_per_tuning_cycle+=myStats->first_start_tx_time-start_spin_time; \
		} \
	} \
}

#define GET_THROUGHPUT() { \
	float *c=(float*)malloc(sizeof(float)*NUMBER_THREADS+1); \
	float *p=(float*)malloc(sizeof(float)*NUMBER_THREADS+1); \
	predicted_throughput=0; \
	int k; \
	c[0]=1; \
	float a=0.0,b=0.0; \
	for (k=1;k<=NUMBER_THREADS;k++){ \
			if(k<=m){ \
				c[k]= c[k-1] * (lambda*((float)NUMBER_THREADS-k+1)/(k * mu[k])); \
				a+=c[k]; \
			}else{ \
				c[k]=c[k-1] * (lambda*((float)NUMBER_THREADS-k+1)/(m * mu[m])); \
				b+=c[k]; \
			} \
	} \
	p[0]=1/(1+a+b); \
	for (k=1;k<=NUMBER_THREADS;k++){ \
		p[k]=p[0]*c[k]; \
	} \
	for (k=1;k<=m;k++){ \
		predicted_throughput+=p[k]*k*mu[k]; \
	} \
	for (k=m+1;k<=NUMBER_THREADS;k++){ \
		predicted_throughput+=p[k]*m*mu[m]; \
	} \
}

#define TUNE_MCATS() { \
	printf("\nTuning ... Current_collector_thread_id %i", current_collector_thread_id); \
	int t,s; \
	fflush(stdout); \
	tm_time_t total_tx_wasted_time=0; \
	tm_time_t total_tx_time=0; \
	tm_time_t total_no_tx_time=0; \
	tm_time_t total_tx_spin_time=0; \
	long tx_committed_table=0; \
	long tx_conflict_table_times=0; \
	float avg_running_tx=0; \
	tm_time_t *wasted_time_k=(tm_time_t *)malloc((NUMBER_THREADS+1)*sizeof(tm_time_t)); \
	tm_time_t *useful_time_k=(tm_time_t *)malloc((NUMBER_THREADS+1)*sizeof(tm_time_t)); \
	long * conflict_active_threads=(long *)malloc((NUMBER_THREADS + 1) * sizeof(long)); \
	long * commit_active_threads=(long *)malloc((NUMBER_THREADS + 1) * sizeof(long)); \
	memset(conflict_active_threads, 0, (NUMBER_THREADS+1) * sizeof(long)); \
	memset(commit_active_threads, 0, (NUMBER_THREADS+1) * sizeof(long)); \
	memset(wasted_time_k, 0, (NUMBER_THREADS+1) * sizeof(tm_time_t)); \
	memset(useful_time_k, 0, (NUMBER_THREADS+1) * sizeof(tm_time_t));	 \
	for (t = 0; t < NUMBER_THREADS; t++) { \
		total_tx_time+=statistics[t].total_useful_time_per_tuning_cycle; \
		total_no_tx_time+=statistics[t].total_no_tx_time_per_tuning_cycle; \
		total_tx_wasted_time+=statistics[t].total_wasted_time_per_tuning_cycle; \
		total_tx_spin_time+=statistics[t].total_spin_time_per_tuning_cycle; \
		tx_committed_table+=statistics[t].commits_per_tuning_cycle; \
		tx_conflict_table_times+=statistics[t].aborts_per_tuning_cycle; \
		int s; \
		printf("\nThread %i", t); \
		for (s = 0; s < NUMBER_THREADS+1; s++) { \
			wasted_time_k[s]+=statistics[t].total_wasted_time_per_active_transactions_per_tuning_cycle[s]; \
			printf("\tw %llu", statistics[t].total_wasted_time_per_active_transactions_per_tuning_cycle[s]); \
			useful_time_k[s]+=statistics[t].total_useful_time_per_active_transactions_per_tuning_cycle[s]; \
			printf("\tu %llu", statistics[t].total_useful_time_per_active_transactions_per_tuning_cycle[s]); \
			commit_active_threads[s]+=statistics[t].total_committed_txs_per_active_transactions_per_tuning_cycle[s]; \
			printf("\tc %llu", statistics[t].total_committed_txs_per_active_transactions_per_tuning_cycle[s]); \
			conflict_active_threads[s]+=statistics[t].total_aborted_txs_per_active_transactions_per_tuning_cycle[s]; \
			printf("\ta %llu", statistics[t].total_aborted_txs_per_active_transactions_per_tuning_cycle[s]); \
			printf(" |"); \
		} \
	} \
	tm_time_t t_wasted_time_k=0; \
	tm_time_t t_useful_time_k=0; \
	long t_commit_active_threads=0; \
	long t_conflict_active_threads=0; \
	for (s = 0; s < NUMBER_THREADS+1; s++) { \
		t_wasted_time_k+=wasted_time_k[s]; \
		t_useful_time_k+=useful_time_k[s]; \
		t_commit_active_threads+=commit_active_threads[s]; \
		t_conflict_active_threads+=conflict_active_threads[s]; \
	} \
	printf("\n-------------"); \
	printf("\ntotal_useful_time_per_tuning_cycle %llu %llu",total_tx_time, t_useful_time_k); \
	printf("\ntotal_no_tx_time_per_tuning_cycle %llu",total_no_tx_time); \
	printf("\ntotal_wasted_time_per_tuning_cycle %llu %llu",total_tx_wasted_time, t_wasted_time_k); \
	printf("\ntotal_spin_time_per_tuning_cycle %llu",total_tx_spin_time); \
	printf("\ncommits_per_tuning_cycle %llu %llu",tx_committed_table, t_commit_active_threads); \
	printf("\naborted_txs_per_tuning_cycle %llu %llu",tx_conflict_table_times, t_conflict_active_threads); \
	mu=(float*)malloc((NUMBER_THREADS+1) * sizeof(float)); \
	lambda = 1.0 / (((float) total_no_tx_time/(float)1000000)/(float) tx_committed_table); \
	int i; \
	for (i=0;i<NUMBER_THREADS+1;i++){ \
		if((wasted_time_k[i]>0 || useful_time_k[i]>0) && commit_active_threads[i] > 0){ \
			mu[i]= 1.0 / ((((float) wasted_time_k[i] / (float)1000000) / (float)commit_active_threads[i]) + (((float) useful_time_k[i]/(float)1000000) / (float) commit_active_threads[i])); \
		}else{ \
			mu[i]= 1.0 / ((((float)total_tx_wasted_time/(float)1000000)/(float)tx_committed_table)+(((float)total_tx_time/(float)1000000) / (float) tx_committed_table)); \
		} \
	} \
	m=tx_cluster_table[0][1]; \
	GET_THROUGHPUT(); \
	double measured_th =NUMBER_THREADS*(double)t_commit_active_threads*1000000/((double)(TM_TIMER_READ()-last_tuning_time)); \
	printf("\nPredicted throughput %f\nMeasured throughput %f\n-----------------",predicted_throughput, measured_th); \
	fflush(stdout); \
	last_tuning_time=TM_TIMER_READ(); \
};

#define RESET_MCATS_STATS() { \
		int t,s; \
		for (t = 0; t < NUMBER_THREADS; t++) { \
			for (s = 0; s < NUMBER_THREADS+1; s++) { \
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
		if (myStats->i_am_the_collector_thread==1){ \
			    myStats->commits_per_tuning_cycle++; \
			myStats->start_no_tx_time=TM_TIMER_READ(); \
			int active_txs=tx_cluster_table[0][0]; \
            while ((__sync_val_compare_and_swap(&tx_cluster_table[0][0], active_txs, active_txs-1) != active_txs)) { \
                for (cycles = 0; cycles < myStats->wait_cycles; cycles++) { \
                    __asm__ ("pause;"); \
                } \
				active_txs=tx_cluster_table[0][0]; \
            } \
			tm_time_t useful_time = myStats->start_no_tx_time - myStats->last_start_tx_time; \
			tm_time_t wasted_time = myStats->last_start_tx_time-myStats->first_start_tx_time; \
			myStats->total_useful_time_per_active_transactions_per_tuning_cycle[active_txs]+=useful_time; \
			myStats->total_committed_txs_per_active_transactions_per_tuning_cycle[active_txs]++; \
			myStats->total_useful_time_per_tuning_cycle+=useful_time; \
			myStats->total_wasted_time_per_tuning_cycle +=wasted_time; \
			if(myStats->commits_per_tuning_cycle==TXS_PER_MCATS_TUNING_CYCLE){ \
				if(myThreadId==NUMBER_THREADS - 1) { \
					myStats->total_no_tx_time_per_tuning_cycle+=TM_TIMER_READ() - myStats->start_no_tx_time; \
					TUNE_MCATS(); \
					RESET_MCATS_STATS(); \
					myStats->start_no_tx_time=TM_TIMER_READ(); \
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
	}


# define TM_BEGIN(b) { \
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
            	} \
            } \
            int status = _xbegin(); \
    		if(myStats->i_am_the_collector_thread && !myStats->first_tx_run){ \
    			tm_time_t last_timer_value=TM_TIMER_READ(); \
    			myStats->total_wasted_time_per_active_transactions_per_tuning_cycle[tx_cluster_table[0][0]]+=last_timer_value - myStats->last_start_tx_time; \
    			myStats->last_start_tx_time=last_timer_value; \
    		} \
			myStats->first_tx_run=0; \
            if (status == _XBEGIN_STARTED) { break; } \
            if (tries == MAX_ATTEMPTS) { \
            	myStats->abortedTxs++; \
                if(myStats->i_am_the_collector_thread) myStats->aborted_txs_per_tuning_cycle++; \
            } \
            tries--; \
            myStats->totalAborts++; \
            if(myStats->i_am_the_collector_thread) { \
		myStats->aborts_per_tuning_cycle++; \
            	myStats->total_aborted_txs_per_active_transactions_per_tuning_cycle[tx_cluster_table[0][0]]++; \
	    } \
            if (tries <= 0) {   \
                while (__sync_val_compare_and_swap(&is_fallback, 0, 1) == 1) { \
                    for (cycles = 0; cycles < myStats->wait_cycles; cycles++) { \
                        __asm__ ("pause;"); \
                    } \
                } \
                break; \
            } \
        }

# define TM_BEGIN_(b) { \
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


# define TM_END() \
	if (tries > 0) { \
        if (IS_LOCKED(is_fallback)) { _xabort(30); } \
		_xend(); \
    } else {    \
        is_fallback = 0; \
    } \
    myStats->totalCommits++; \
    TM_SIGNAL(); \
};


# define TM_END_() \
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
