/*
 * This file is part of the pthread-lib Library.
 * Copyright (C) 2008-2009 Nick Powers.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */
 
#ifndef __PTL_THREAD_MANAGER_H__
#define __PTL_THREAD_MANAGER_H__

#include "ptl_queue.h"
#include "ptl_thread_pool.h"
#include "ptl_task.h"


#define RUNNING = 0;
#define SHUTDOWN = 1;
#define STOP = 2;
#define TERMINATED = 3;

/* Structures */

struct ptl_thread_manager {
	ptl_q_t work_q;						/**< queue/list that is being used */
	void (*rejected_handler)(void *);	/**< rejected handler function */
	int run_state;						/**< current running state of this manager */
	pthread_mutex_t main_mutex; 		/**< Lock held on updates to pool_size, 
	 							     		 core_pool_size,max_pool_size, run_state, 
	 								 		 and workers set. */
	pthread_cond_t termination_mutex;	/**< wait condition to support termination */
	ptl_thread_pool_t thread_pool;
	void (*before_execute)(void *);	  	/**< executes before function pointer */
	void (*after_execute)(void *);	  	/**< executes after function pointer */
};


/* Type Definitions */
typedef struct ptl_rejected_handler *ptl_rejected_handler_t;
typedef struct ptl_thread_manager_state *ptl_thread_manager_state_t;
typedef struct ptl_thread_manger *ptl_thread_manger_t;


/* Methods */

/**
 * Create and initilize the thread pool manager.
 * This will create the pool of threads and put it in the RUNNING state.
 * After this function is execute, the manager is ready to accept tasks.
 * 
 */
ptl_thread_manger_t create_thread_manager(int core_pool_size, 
						   				  int max_pool_size, 
						   				  long keep_alive_time,
						   				  ptl_q_t work_q,
						   				  void (*rejected_handler)(void *));
/**
 * Create and initilize the thread pool manager.
 * This will create the pool of threads and put it in the RUNNING state.
 * After this function is execute, the manager is ready to accept tasks.
 */
ptl_thread_manger_t 
	create_thread_manager_with_pool(ptl_thread_pool_t thread_pool,
						   			ptl_q_t work_q,
						   			void* rejected_handler);

/**
 * Submits the function pointer to the queue that is being watched by 
 * the pool of threads.
 *
 * @return 1 if successful, 0 otherwise
 */
int submit(ptl_thread_manger_t manager, void* function_to_execute);

/**
 * Submits the task (ptl_task_t) to the queue that is being watched by the
 * pool of threads. This is a different flavor of submit(manager, void*).
 *
 * @return 1 if successful, 0 otherwise
 */
int submit_task(ptl_thread_manger_t manager, ptl_task_t task);

/**
 * Initiates an orderly shutdown in which previously submitted
 * tasks are executed, but no new tasks will be
 * accepted. Invocation has no additional effect if already shut
 * down. Shutsdown the entire thread pool, manager, and signal handler.
 * 
 */
void shutdown(ptl_thread_manger_t manager);

/**
 * Attempts to stop all actively executing tasks, halts the
 * processing of waiting tasks, and returns a list of the tasks
 * that were awaiting execution. These tasks are drained (removed)
 * from the task queue upon return from this method.
 *
 * @return a list of tasks that never commenced execution
 */
ptl_q_t shutdown_now(ptl_thread_manger_t manager);

/**
 * Returns 1 if the thread manager is terminated.
 *
 * @return 1 if terminated, 0 otherwise
 */
int is_terminated(ptl_thread_manger_t manager);

/**
 * Returns 1 if the thread manager is trying to terminate after shutdown.
 *
 * @return 1 if terminating, 0 otherwise
 */
int is_terminating(ptl_thread_manger_t manager);

/**
 * Tries to remove from the work queue all tasks that have been cancelled.
 */
void purge_cancelled(ptl_thread_manger_t manager);



 /* Policies */
/** 
 * This policy will return NULL when the task is rejected.
 * This is the default policy.
 *
 * @return returns NULL if the task is rejected.
 */
void *ptl_abort_policy();

/** 
 * The thread that submitted the task, will run the task.
 * This provides a simple feedback control mechanism that will slow down 
 * the rate that new tasks are submitted.
 */
void *ptl_q_caller_runs_policy();

/** 
 * A task that cannot be executed is simply dropped.
 */
void *ptl_q_discard_policy();

/** 
 * If the executor is not shut down, the task at the head of the work queue 
 * is dropped, and then execution is retried (which can fail again, causing 
 * this to be repeated.) 
 */
void *ptl_q_discard_oldest_policy();


#endif
 
