
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

    unsigned long total_run_execution_time_per_state_per_cycle[NUMBER_THREADS+1];
    unsigned long total_committed_runs_per_state_per_cycle[NUMBER_THREADS+1];
    unsigned long total_aborted_runs_per_state_per_cycle[NUMBER_THREADS+1];
    unsigned long total_acquired_locks_per_state_per_cycle[NUMBER_THREADS+1];
    unsigned long total_run_execution_time_per_cycle;
    unsigned long total_no_tx_time_per_cycle;
    unsigned long total_spin_time_per_cycle;
    unsigned long start_tx_time;
    unsigned long start_no_tx_time;
    unsigned long wait_cycles;
    unsigned long commits_per_cycle;
	unsigned long aborts_per_cycle;
	unsigned long acquired_locks_per_cycle;
    // MCATS code end

} __attribute__((aligned(64))) thread_metadata_t;

__attribute__((aligned(64))) thread_metadata_t statistics[NUMBER_THREADS];
__attribute__((aligned(64))) static volatile unsigned long tx_cluster_table[NUMBER_ATOMIC_BLOCKS][2];
__attribute__((aligned(64))) unsigned long runs_limit;
__attribute__((aligned(64))) unsigned long main_thread;
__attribute__((aligned(64))) unsigned long current_collector_thread;
__attribute__((aligned(64))) float lambda;
__attribute__((aligned(64))) float mu;
__attribute__((aligned(64))) unsigned long m;
__attribute__((aligned(64))) unsigned long total_aborted_runs_per_state[NUMBER_THREADS+1]; \
__attribute__((aligned(64))) unsigned long total_committed_runs_per_state[NUMBER_THREADS+1]; \
__attribute__((aligned(64))) unsigned long total_acquired_locks_per_state[NUMBER_THREADS+1]; \
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


#define RESET_MCATS_STATS() { \
		int t,s; \
		for (t = 0; t < NUMBER_THREADS; t++) { \
			for (s = 0; s < NUMBER_THREADS+1; s++) { \
				statistics[t].total_run_execution_time_per_state_per_cycle[s]=0; \
				statistics[t].total_committed_runs_per_state_per_cycle[s]=0; \
				statistics[t].total_aborted_runs_per_state_per_cycle[s]=0; \
				statistics[t].total_acquired_locks_per_state_per_cycle[s]=0; \
			} \
			statistics[t].total_run_execution_time_per_cycle=0; \
			statistics[t].total_no_tx_time_per_cycle=0; \
			statistics[t].total_spin_time_per_cycle=0; \
			statistics[t].commits_per_cycle=0; \
			statistics[t].aborts_per_cycle=0; \
			statistics[t].acquired_locks_per_cycle=0; \
		} \
	}

#define GET_THROUGHPUT() { \
		gsl_vector *b = gsl_vector_alloc(2 * NUMBER_THREADS); \
		gsl_vector *x = gsl_vector_alloc(2 * NUMBER_THREADS); \
		gsl_matrix *Q = gsl_matrix_alloc(2 * NUMBER_THREADS, 2 * NUMBER_THREADS); \
		gsl_vector_set_zero(b); \
		gsl_vector_set(b, 2*NUMBER_THREADS-1,1); \
		gsl_matrix_set_zero(Q); \
		float value; \
		int i; \
		for (i = 1; i <= NUMBER_THREADS; i++) { \
			value=(NUMBER_THREADS-(i-1))*lambda; \
			gsl_matrix_set(Q,i, i-1, value); \
			gsl_matrix_set(Q,i-1, i-1, -value+gsl_matrix_get(Q,i-1, i-1)); \
		} \
		for (i = NUMBER_THREADS+2; i <= 2*NUMBER_THREADS-1; i++) { \
			value=(NUMBER_THREADS-(i-NUMBER_THREADS))*lambda; \
			gsl_matrix_set(Q,i, i-1, value); \
			gsl_matrix_set(Q,i-1, i-1, -value+gsl_matrix_get(Q,i-1, i-1)); \
		} \
		for (i = 1; i <= m; i++) { \
			value=i*mu*((float)total_committed_runs_per_state[i]/((float)total_committed_runs_per_state[i]+(float)total_aborted_runs_per_state[i])); \
			gsl_matrix_set(Q,i-1, i, value); \
			gsl_matrix_set(Q,i, i, -value+gsl_matrix_get(Q,i,i)); \
		} \
		for (i = m+1; i <= NUMBER_THREADS; i++) { \
			value= m*mu*((float)total_committed_runs_per_state[m]/((float)total_committed_runs_per_state[m]+(float)total_aborted_runs_per_state[m])); \
			gsl_matrix_set(Q,i-1, i,value); \
			gsl_matrix_set(Q,i, i, -value+gsl_matrix_get(Q,i,i)); \
		} \
		for (i = NUMBER_THREADS+1; i<= 2*NUMBER_THREADS-1; i++) { \
			gsl_matrix_set(Q,i-NUMBER_THREADS, i, mu); \
			gsl_matrix_set(Q,i, i, -mu+gsl_matrix_get(Q,i,i)); \
		} \
		for (i = 2; i<= m; i++) { \
			value= i*mu*((float)total_acquired_locks_per_state[i]/((float)total_committed_runs_per_state[i]+(float)total_aborted_runs_per_state[i])); \
			gsl_matrix_set(Q,NUMBER_THREADS+i-1, i,value); \
			gsl_matrix_set(Q,i,i, -value+gsl_matrix_get(Q,i,i)); \
		} \
		for (i = m+1; i <= NUMBER_THREADS; i++) { \
			value=m*mu*((float)total_acquired_locks_per_state[m]/((float)total_committed_runs_per_state[m]+(float)total_aborted_runs_per_state[m])); \
			gsl_matrix_set(Q,NUMBER_THREADS+i-1, i, value); \
			gsl_matrix_set(Q,i,i, -value+gsl_matrix_get(Q,i,i)); \
		} \
		for (i = 0; i <= 2*NUMBER_THREADS-1; i++) { \
			gsl_matrix_set(Q,2*NUMBER_THREADS-1, i, 1); \
		} \
		printf ("\nQ:\n"); \
		int j; \
		for (i = 0; i <= 2*NUMBER_THREADS-1; i++) { \
		    for (j = 0; j <= 2*NUMBER_THREADS-1; j++) \
		    	printf ("%.2f\t",gsl_matrix_get (Q, i, j)); \
		    printf ("\n"); \
		} \
		gsl_permutation *p=gsl_permutation_alloc(2 * NUMBER_THREADS); \
		int s; \
		gsl_linalg_LU_decomp(Q, p, &s); \
		gsl_linalg_LU_solve(Q, p, b, x); \
		printf ("x:\n"); \
		for (i = 0; i <= 2*NUMBER_THREADS-1; i++) { \
		    	printf ("%.2f\t",gsl_vector_get (x, i)); \
		} \
		gsl_permutation_free(p); \
		gsl_vector_free(x); \
}

#define TUNE_MCATS() { \
	printf("\nTuning ... Current_collector_thread_id %i", current_collector_thread_id); \
	int t,s; \
	fflush(stdout); \
	tm_time_t total_run_execution_time=0; \
	tm_time_t total_no_tx_time=0; \
	tm_time_t total_tx_spin_time=0; \
	long total_committed_runs=0; \
	long total_aborted_runs=0; \
	long total_acquired_locks=0; \
	float avg_running_tx=0; \
	memset(total_aborted_runs_per_state, 0, (NUMBER_THREADS+1) * sizeof(long)); \
	memset(total_committed_runs_per_state, 0, (NUMBER_THREADS+1) * sizeof(long)); \
	memset(total_acquired_locks_per_state, 0, (NUMBER_THREADS+1) * sizeof(long)); \
	for (t = 0; t < NUMBER_THREADS; t++) { \
		total_run_execution_time+=statistics[t].total_run_execution_time_per_cycle; \
		total_no_tx_time+=statistics[t].total_no_tx_time_per_cycle; \
		total_tx_spin_time+=statistics[t].total_spin_time_per_cycle; \
		total_committed_runs+=statistics[t].commits_per_cycle; \
		total_aborted_runs+=statistics[t].aborts_per_cycle; \
		total_acquired_locks+=statistics[t].acquired_locks_per_cycle; \
		int s; \
		printf("\nThread %i", t); \
		for (s = 0; s < NUMBER_THREADS+1; s++) { \
			total_committed_runs_per_state[s]+=statistics[t].total_committed_runs_per_state_per_cycle[s]; \
			printf("\tc %llu", statistics[t].total_committed_runs_per_state_per_cycle[s]); \
			total_aborted_runs_per_state[s]+=statistics[t].total_aborted_runs_per_state_per_cycle[s]; \
			printf("\ta %llu", statistics[t].total_aborted_runs_per_state_per_cycle[s]); \
			total_acquired_locks_per_state[s]+=statistics[t].total_acquired_locks_per_state_per_cycle[s]; \
			printf("\tl %llu", statistics[t].total_acquired_locks_per_state_per_cycle[s]); \
			printf(" |"); \
		} \
	} \
	long t_total_committed_runs_per_state=0; \
	long t_total_aborted_runs_per_state=0; \
	long t_total_acquired_locks_per_state=0; \
	for (s = 0; s < NUMBER_THREADS+1; s++) { \
		t_total_committed_runs_per_state+=total_committed_runs_per_state[s]; \
		t_total_aborted_runs_per_state+=total_aborted_runs_per_state[s]; \
		t_total_acquired_locks_per_state+=total_acquired_locks_per_state[s]; \
	} \
	printf("\n-------------"); \
	printf("\ntotal_run_execution_time_per_cycle %llu",total_run_execution_time); \
	printf("\ntotal_no_tx_time_per_cycle %llu",total_no_tx_time); \
	printf("\ntotal_spin_time_per_cycle %llu",total_tx_spin_time); \
	printf("\ncommits_per_cycle %llu %llu",total_committed_runs, t_total_committed_runs_per_state); \
	printf("\naborted_txs_per_cycle %llu %llu",total_aborted_runs, t_total_aborted_runs_per_state); \
	printf("\nacquired_locks_per_aborted_txs_per_cycle %llu %llu",total_acquired_locks, t_total_acquired_locks_per_state); \
	lambda = 1.0 / (((float) total_no_tx_time/(float)1000000)/(float) total_committed_runs); \
	mu= 1.0 / ((((float) total_run_execution_time / (float)1000000) / (float)(total_committed_runs+total_aborted_runs))); \
	m=tx_cluster_table[0][1]; \
	GET_THROUGHPUT(); \
	double measured_th =NUMBER_THREADS*(double)t_total_committed_runs_per_state*1000000/((double)(TM_TIMER_READ()-last_tuning_time)); \
	printf("\nPredicted throughput %f\nMeasured throughput %f\n-----------------",predicted_throughput, measured_th); \
	fflush(stdout); \
	last_tuning_time=TM_TIMER_READ(); \
};

#define TM_WAIT() { \
    thread_metadata_t* myStats = &(statistics[myThreadId]); \
	int active_txs,entered=0; \
	tm_time_t start_spin_time; \
	active_txs=tx_cluster_table[0][0]; \
	if(active_txs<tx_cluster_table[0][1]){ \
		if (__sync_val_compare_and_swap(&tx_cluster_table[0][0], active_txs, active_txs+1) == active_txs) { \
			if(myStats->i_am_the_collector_thread){ \
				myStats->start_tx_time=TM_TIMER_READ(); \
				myStats->total_no_tx_time_per_cycle+=myStats->start_tx_time - myStats->start_no_tx_time; \
			} \
			entered=1; \
		} \
	} \
	if (entered==0){ \
		if(myStats->i_am_the_collector_thread==1){ \
			start_spin_time=TM_TIMER_READ(); \
			myStats->total_no_tx_time_per_cycle+=start_spin_time - myStats->start_no_tx_time; \
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
			myStats->start_tx_time=TM_TIMER_READ(); \
			myStats->total_spin_time_per_cycle+=myStats->start_tx_time-start_spin_time; \
		} \
	} \
}

#define TM_SIGNAL() { \
        thread_metadata_t* myStats = &(statistics[myThreadId]); \
		if (myStats->i_am_the_collector_thread==1){ \
			    myStats->commits_per_cycle++; \
			myStats->start_no_tx_time=TM_TIMER_READ(); \
			int active_txs=tx_cluster_table[0][0]; \
            while ((__sync_val_compare_and_swap(&tx_cluster_table[0][0], active_txs, active_txs-1) != active_txs)) { \
                for (cycles = 0; cycles < myStats->wait_cycles; cycles++) { \
                    __asm__ ("pause;"); \
                } \
				active_txs=tx_cluster_table[0][0]; \
            } \
			tm_time_t run_execution_time = myStats->start_no_tx_time - myStats->start_tx_time; \
			myStats->total_run_execution_time_per_state_per_cycle[active_txs]+=run_execution_time; \
			myStats->total_committed_runs_per_state_per_cycle[active_txs]++; \
			myStats->total_run_execution_time_per_cycle+=run_execution_time; \
			if(myStats->commits_per_cycle==TXS_PER_MCATS_TUNING_CYCLE){ \
				if(myThreadId==NUMBER_THREADS - 1) { \
					myStats->total_no_tx_time_per_cycle+=TM_TIMER_READ() - myStats->start_no_tx_time; \
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
    			myStats->total_run_execution_time_per_state_per_cycle[tx_cluster_table[0][0]]+=last_timer_value - myStats->start_tx_time; \
    			myStats->start_tx_time=last_timer_value; \
    		} \
			myStats->first_tx_run=0; \
            if (status == _XBEGIN_STARTED) { break; } \
			int active_txs=tx_cluster_table[0][0]; \
            tries--; \
            myStats->totalAborts++; \
            if(myStats->i_am_the_collector_thread) { \
            	myStats->aborts_per_cycle++; \
            	myStats->total_aborted_runs_per_state_per_cycle[active_txs]++; \
            } \
            if (tries <= 0) {   \
                if(myStats->i_am_the_collector_thread) { \
                	myStats->total_acquired_locks_per_state_per_cycle[active_txs]++; \
                	myStats->acquired_locks_per_cycle++; \
                } \
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
