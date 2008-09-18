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
void add_thread();
void ensure_queued_task_handled();
void reject();
void run_task(void* task);
void get_next_task();
void interrupt_idle_threads();
void drain_queue();

/* Function Bodies */


/* Create a thread manager. Creates a thread pool as well */
ptl_thread_manager_t create_thread_manager(int core_pool_size, 
						   int max_pool_size, 
						   long keep_alive_time,
						   ptl_q_t work_q,
						   void (*rejected_handler)(void *)){
							   
	ptl_thread_manager_t manager = (ptl_thread_manager_t)calloc(1, sizeof(struct ptl_thread_manager));
	assert(manager);
	
	/* create the thread pool */
	ptl_thread_pool_t thread_pool = 
		ptl_create_thread_pool(core_pool_size, max_pool_size, keep_alive_time);
							   
							   
	/* initilize the manager struct */
	manager->work_q = work_q;
	manager->thread_pool = thread_pool;
	manager->run_state = RUNNING;
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



/* create a thread manager, with a already defined thread pool */
ptl_thread_manager_t 
	create_thread_manager_with_pool(ptl_thread_pool_t thread_pool,
						   			ptl_q_t work_q,
						   			void* rejected_handler){
				
	ptl_thread_manager_t manager = (ptl_thread_manager_t)calloc(1, sizeof(struct ptl_thread_manager));
	assert(manager);
							   					   
	/* initilize the manager struct */
	manager->work_q = work_q;
	manager->thread_pool = thread_pool;
	manager->run_state = RUNNING;
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
    /*
 struct ptl_thread_manager {
	ptl_q_t work_q;						
	void* rejected_handler;				
	int run_state;						
	pthread_mutex_t main_mutex; 		
	 							     		 
	pthread_cond_t termination_mutex;	
	struct ptl_thread_pool;
	void (*before_execute)(void *);	  	
	void (*after_execute)(void *)
};*/

   // pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t));

    /* assign pool to manage */
    //pool_to_manage = thread_pool;

    /* create our manager thread */
    //rc = pthread_create(thread, NULL, begin_managing, (void *)arg);

    /* save our thread handle */
    //global_manager_thread = thread;

    /* set our running flag */
    //thread_manager_running = TRUE;

    /* create mutexes */
    //status = pthread_mutex_init(&thread_pool_mutex, NULL);
    //CHECK_STATUS(status, "pthread_mutex_init", "thread_pool_mutex bad status");

void add_thread(){return;}
void ensure_queued_task_handled(){return;}
void reject(){return;}
void run_task(void* task){return;}
void get_next_task(){return;}
void interrupt_idle_threads(){return;}
void drain_queue(){return;}
 
 
