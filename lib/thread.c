/* =============================================================================
 *
 * thread.c
 *
 * =============================================================================
 *
 * Copyright (C) Stanford University, 2006.  All Rights Reserved.
 * Author: Chi Cao Minh
 *
 * =============================================================================
 *
 * For the license of bayes/sort.h and bayes/sort.c, please see the header
 * of the files.
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of kmeans, please see kmeans/LICENSE.kmeans
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of ssca2, please see ssca2/COPYRIGHT
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of lib/mt19937ar.c and lib/mt19937ar.h, please see the
 * header of the files.
 * 
 * ------------------------------------------------------------------------
 * 
 * For the license of lib/rbtree.h and lib/rbtree.c, please see
 * lib/LEGALNOTICE.rbtree and lib/LICENSE.rbtree
 * 
 * ------------------------------------------------------------------------
 * 
 * Unless otherwise noted, the following license applies to STAMP files:
 * 
 * Copyright (c) 2007, Stanford University
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 * 
 *     * Neither the name of Stanford University nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY STANFORD UNIVERSITY ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL STANFORD UNIVERSITY BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * =============================================================================
 */


#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <assert.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include "thread.h"
#include "types.h"
#include "random.h"

static THREAD_LOCAL_T    global_threadId;
static long              global_numThread       = 1;
static THREAD_BARRIER_T* global_barrierPtr      = NULL;
static long*             global_threadIds       = NULL;
static THREAD_ATTR_T     global_threadAttr;
static THREAD_T*         global_threads         = NULL;
static void            (*global_funcPtr)(void*) = NULL;
static void*             global_argPtr          = NULL;
static volatile bool_t   global_doShutdown      = FALSE;



/*
__attribute__((aligned(64))) spin_lock_t paddedSpinLocks[NUMBER_ATOMIC_BLOCKS];
__attribute__((aligned(64))) spin_lock_t paddedCpuLocks[8];

__attribute__((aligned(64))) global_snapshot_t globalSnapshot[NUMBER_THREADS];
__attribute__((aligned(64))) global_optimizer_t globalOptimizer;
__attribute__((aligned(64))) unsigned long totalAbortStats[NUMBER_ATOMIC_BLOCKS][NUMBER_ATOMIC_BLOCKS];
__attribute__((aligned(64))) unsigned long totalCommitStats[NUMBER_ATOMIC_BLOCKS][NUMBER_ATOMIC_BLOCKS];
__attribute__((aligned(64))) double condAbortProb[NUMBER_ATOMIC_BLOCKS][NUMBER_ATOMIC_BLOCKS];
__attribute__((aligned(64))) double uncondAbortProb[NUMBER_ATOMIC_BLOCKS][NUMBER_ATOMIC_BLOCKS];
__attribute__((aligned(64))) short globalLocksToAcquire[NUMBER_ATOMIC_BLOCKS][NUMBER_ATOMIC_BLOCKS];
*/

// MCATS code start
__attribute__((aligned(64))) thread_metadata_t statistics[NUMBER_THREADS];
__attribute__((aligned(64))) static volatile unsigned long tx_cluster_table[NUMBER_ATOMIC_BLOCKS][2];
__attribute__((aligned(64))) unsigned long runs_limit;
__attribute__((aligned(64))) unsigned long main_thread;
__attribute__((aligned(64))) unsigned long current_collector_thread;
// MCATS code end

int benchmarkId;
int current_collector_thread_id;
int MAX_ATTEMPTS;
int APRIORI_ATTEMPTS;
int TXS_PER_MCATS_TUNING_CYCLE;

thread_args_t* threadArgsArr;

__attribute__((aligned(64))) volatile unsigned long is_fallback = 0;

__thread unsigned short myThreadId;



/* =============================================================================
 * threadWait
 * -- Synchronizes all threads to start/stop parallel section
 * =============================================================================
 */
static void
threadWait (void* argPtr)
{
    thread_args_t* args = (thread_args_t*) argPtr;
    long threadId = args->threadId;
    myThreadId = (unsigned short) threadId;

    THREAD_LOCAL_SET(global_threadId, (long)threadId);

    bindThread(threadId);

    thread_metadata_t* myStats = &(statistics[myThreadId]);

    while (1) {
        THREAD_BARRIER(global_barrierPtr, threadId); /* wait for start parallel */
        if (global_doShutdown) {
            break;
        }
        global_funcPtr(global_argPtr);
        THREAD_BARRIER(global_barrierPtr, threadId); /* wait for end parallel */
        if (threadId == 0) {
            break;
        }
    }
}

/* =============================================================================
 * thread_startup
 * -- Create pool of secondary threads
 * -- numThread is total number of threads (primary + secondaries)
 * =============================================================================
 */
void
thread_startup (long numThread)
{
    int i;
    int k;
    int l,m;

    for (i = 0; i < NUMBER_THREADS; i++) {
        statistics[i].totalCommits = 0;
        statistics[i].abortedTxs = 0;
        statistics[i].totalAborts = 0;
    }

    global_numThread = numThread;
    global_doShutdown = FALSE;

    /* Set up barrier */
    assert(global_barrierPtr == NULL);
    global_barrierPtr = THREAD_BARRIER_ALLOC(numThread);
    assert(global_barrierPtr);
    THREAD_BARRIER_INIT(global_barrierPtr, numThread);

    /* Set up ids */
    THREAD_LOCAL_INIT(global_threadId);
    assert(global_threadIds == NULL);
    global_threadIds = (long*)malloc(numThread * sizeof(long));
    assert(global_threadIds);
    for (i = 0; i < numThread; i++) {
        global_threadIds[i] = i;
    }

    /* Set up thread list */
    assert(global_threads == NULL);
    global_threads = (THREAD_T*)malloc(numThread * sizeof(THREAD_T));
    assert(global_threads);

    threadArgsArr = (thread_args_t*) malloc(numThread * sizeof(thread_args_t));
    threadArgsArr[0].aborts = 0;
    threadArgsArr[0].commits = 0;
    threadArgsArr[0].threadId = global_threadIds[0];

    /* Set up pool */
    THREAD_ATTR_INIT(global_threadAttr);
    for (i = 1; i < numThread; i++) {
        threadArgsArr[i].aborts = 0;
        threadArgsArr[i].commits = 0;
        threadArgsArr[i].threadId = global_threadIds[i];
        THREAD_CREATE(global_threads[i],
                      global_threadAttr,
                      &threadWait,
                      &(threadArgsArr[i]));
    }
}

void
thread_start (void (*funcPtr)(void*), void* argPtr)
{
    global_funcPtr = funcPtr;
    global_argPtr = argPtr;

    threadWait((void*)&(threadArgsArr[0]));
}


void
thread_shutdown ()
{
    /* Make secondary threads exit wait() */
    global_doShutdown = TRUE;
    THREAD_BARRIER(global_barrierPtr, 0);

    long numThread = global_numThread;

    long i;
    for (i = 1; i < numThread; i++) {
        THREAD_JOIN(global_threads[i]);
    }

    THREAD_BARRIER_FREE(global_barrierPtr);
    global_barrierPtr = NULL;

    free(global_threadIds);
    global_threadIds = NULL;

    free(global_threads);
    global_threads = NULL;

    global_numThread = 1;
}

barrier_t *barrier_alloc() {
    return (barrier_t *)malloc(sizeof(barrier_t));
}

void barrier_free(barrier_t *b) {
    free(b);
}

void barrier_init(barrier_t *b, int n) {
    pthread_cond_init(&b->complete, NULL);
    pthread_mutex_init(&b->mutex, NULL);
    b->count = n;
    b->crossing = 0;
}

void barrier_cross(barrier_t *b) {
    pthread_mutex_lock(&b->mutex);
    /* One more thread through */
    b->crossing++;
    /* If not all here, wait */
    if (b->crossing < b->count) {
        pthread_cond_wait(&b->complete, &b->mutex);
    } else {
        /* Reset for next time */
        b->crossing = 0;
        pthread_cond_broadcast(&b->complete);
    }
    pthread_mutex_unlock(&b->mutex);
}

void
thread_barrier_wait()
{
    long threadId = thread_getId();
    THREAD_BARRIER(global_barrierPtr, threadId);
}

long
thread_getId()
{
    return (long)THREAD_LOCAL_GET(global_threadId);
}

long
thread_getNumThread()
{
    return global_numThread;
}
