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

#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "ptl_thread_manager.h"
#include "ptl_util.h"


/* Private Functions */
void _reject_handler(ptl_task_t task, void (*rejected_handler) (void *));
void add_thread();
void ensure_queued_task_handled();
void reject();
void run_task(void* task);
void get_next_task();
void interrupt_idle_threads();
void drain_queue();


/* Public Functions */

/* create a manager with before and after execute functions */
ptl_thread_manager_t create_thread_manager_with_functions(int core_pool_size, 
						   int max_pool_size, 
						   long keep_alive_time,
						   ptl_q_t work_q,
						   void (*rejected_handler)(void *),
						   void (*before_execute)(void *),
						   void (*after_execute)(void *)){
					
 ptl_thread_manager_t manager = (ptl_thread_manager_t)calloc(1, sizeof(struct ptl_thread_manager));
	assert(manager);
	
	/* create the thread pool */
	ptl_thread_pool_t thread_pool = 
		ptl_create_thread_pool(core_pool_size, max_pool_size, keep_alive_time);
							   
							   
	/* initilize the manager struct */
	manager->work_q = work_q;
	manager->thread_pool = thread_pool;
	manager->run_state = PTL_RUNNING;
	manager->num_completed_tasks = 0;
	/* functions */
	manager->rejected_handler = rejected_handler;
	manager->before_execute = before_execute;
	manager->after_execute = after_execute;
	
	/* create mutexes and conditions */
	pthread_mutex_t main_mutex = PTHREAD_MUTEX_INITIALIZER;
  	pthread_cond_t  termination_mutex = PTHREAD_COND_INITIALIZER;
							   
	manager->main_mutex = main_mutex;
	manager->termination_mutex = termination_mutex;
							   
	return manager;						   
}


/* Create a thread manager. Creates a thread pool as well */
ptl_thread_manager_t create_thread_manager(int core_pool_size, 
						   int max_pool_size, 
						   long keep_alive_time,
						   ptl_q_t work_q,
						   void (*rejected_handler)(void *)){
		
	/* see create_thread_manager_with_functions for details */
	return create_thread_manager_with_functions(core_pool_size, 
												max_pool_size, 
												keep_alive_time, 
												work_q, 
												rejected_handler, 
												NULL,
												NULL);			  							
}


/* create a thread manager, with a already defined thread pool */
ptl_thread_manager_t 
	create_thread_manager_with_pool(ptl_thread_pool_t thread_pool,
						   			ptl_q_t work_q,
						   			void (*rejected_handler) (void *)){
				
	ptl_thread_manager_t manager = (ptl_thread_manager_t)calloc(1, sizeof(struct ptl_thread_manager));
	assert(manager);
							   					   
	/* initilize the manager struct */
	manager->work_q = work_q;
	manager->thread_pool = thread_pool;
	manager->run_state = PTL_RUNNING;
	manager->num_completed_tasks = 0;
	/* functions */
	manager->rejected_handler = rejected_handler;
	manager->before_execute = NULL;
	manager->after_execute = NULL;
	
	/* create mutexes and conditions */
	pthread_mutex_t main_mutex = PTHREAD_MUTEX_INITIALIZER;
  	pthread_cond_t  termination_mutex = PTHREAD_COND_INITIALIZER;
							   
	manager->main_mutex = main_mutex;
	manager->termination_mutex = termination_mutex;
										   
	return manager;									   
}


/* put on work_q. If not able to, then call rejected handler with function */
int submit(ptl_thread_manager_t manager, void (*function_to_execute)(void *)){
	if(manager == NULL || function_to_execute == NULL){
		return 0;
	}
	
	// wrap the function in a 'task'
	ptl_task_t task = create_task(function_to_execute);
	
	return submit_task(manager, task);
}


/* put on work_q. If not able to, then call rejected handler with function */
int submit_task(ptl_thread_manager_t manager, ptl_task_t task){
	if(manager == NULL || task == NULL){
		return 0;
	}
	
	// put it in the work queue
	int successful = ptl_q_add(manager->work_q, task);
	
	// if not successful, then call the rejected handler
	if(!successful){
		_reject_handler(task, manager->rejected_handler);
	}
	
	return successful;
}

void shutdown(ptl_thread_manager_t manager){
	return; //TODO: implement
}

ptl_q_t shutdown_now(ptl_thread_manager_t manager){
	return NULL; //TODO: implement
}

int is_terminated(ptl_thread_manager_t manager){
	return manager->run_state == PTL_TERMINATED;
}

int is_terminating(ptl_thread_manager_t manager){
	return manager->run_state == PTL_SHUTDOWN || manager->run_state == PTL_STOP;
}

void purge_cancelled(ptl_thread_manager_t manager){
	return; //TODO: Implement
}

void *ptl_abort_policy(){
	return NULL; //TODO: Implement
}

void *ptl_q_caller_runs_policy(){
	return NULL; //TODO: Implement
}


void *ptl_q_discard_policy(){
	return NULL; //TODO: Implement
}


void *ptl_q_discard_oldest_policy(){
	return NULL; //TODO: Implement
}

 

/* Private Functions */


//TODO: use this as a rejected function wrapper
void _reject_handler(ptl_task_t task, void (*rejected_handler) (void *)){
	task->state = PTL_TASK_STATE_REJECTED;
	
	rejected_handler(task->function_to_execute);
	
	destroy_task(task);
}

/* add a thread to the currnent thread pool */
void add_thread(){return;}
void add_if_under_max_pool_size(){
	return;
}
	
void ensure_queued_task_handled(){return;}
void reject(){return;}
void run_task(void* task){return;}
void get_next_task(){return;}
void interrupt_idle_threads(){return;}
void drain_queue(){return;}

 
 
