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

/* See header file for documentation */

#include <pthread.h>
#include <stdlib.h> 
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include "ptl_queue.h"
#include "ptl_linked_queue.h"
#include "ptl_util.h"

/* Global Variables */

pthread_mutex_t ptl_lq_mutex = PTHREAD_MUTEX_INITIALIZER;

/* initialize memory needed for this type of queue. */
void ptl_lq_init_queue (ptl_q_t q){
	pthread_mutex_lock(&ptl_lq_mutex); // lock
	
	q->tail = q->head = ptl_q_create_element(NULL);
	
	pthread_mutex_unlock(&ptl_lq_mutex); // unlock
}


/* free the memory created using this type of list. */
void ptl_lq_destroy_queue(ptl_q_t q){
	ptl_lq_clear(q); // clears all

	FREE(q->head); // remove final piece of memory in queue
	
	pthread_mutex_destroy(&ptl_lq_mutex);
	// leave destroying of ptl_q_t to the 'interface'
}


/* add 'value' to the head of the queue. */
int ptl_lq_add(ptl_q_t q, void *value){

	if((q == NULL) || (value == NULL)){ return 0; }

	// no capacity check as this list is unbounded

	pthread_mutex_lock(&ptl_lq_mutex); // lock
	// create the element/node
	ptl_q_element_t element = ptl_q_create_element (value);
	
	q->tail = q->tail->next = element;
	q->size++;
	
	pthread_mutex_unlock(&ptl_lq_mutex); // unlock
	
	return 1;
}

/* There is no waiting for this type of queue because it is unbounded. */
int ptl_lq_add_wait(ptl_q_t q, void *value, long timeout){
	 /* This is an unbounded queue, add will always
	    succeed if there is enough memory */
	return ptl_lq_add(q, value);
}

/* Removes all of the elements from this queue. */
void ptl_lq_clear(ptl_q_t q){
	if(q == NULL){ return; }
	
	void *e = NULL;
	while((e = ptl_lq_get (q)) != NULL){
		FREE(e);
	}
	
	return;
}


/* Removes all of the elements from this queue using the 
   free_func to free memory. */
void ptl_lq_clear_freefunc(ptl_q_t q, void (*free_func)(void *)){
		if(q == NULL){ return; }
	
	void *e = NULL;
	while((e = ptl_lq_get (q)) != NULL){
		// use the function to free 'e'
		free_func(e);
	}
	
	return;
}


/* Retrieves, but does not remove, the head of this queue. */
void* ptl_lq_peek(ptl_q_t q){

	if(q->size <= 0){ return NULL; } // need to lock (seperate lock?)

	pthread_mutex_lock(&ptl_lq_mutex); // lock
	
	ptl_q_element_t first = q->head->next; // get first element
	void *return_elem = NULL;
	
	if(first != NULL){

		if (first != NULL) { return_elem = first->value; } // create copy pointer

	}
	pthread_mutex_unlock(&ptl_lq_mutex); // unlock

	return return_elem;
}


/* Retrieves and removes the head of this queue. */
void* ptl_lq_get(ptl_q_t q){
	if(q->size <= 0){ return NULL; } // need to lock (seperate lock?)

	pthread_mutex_lock(&ptl_lq_mutex); // lock
	
    ptl_q_element_t first = q->head->next;
	
	void* value = NULL;
	if(first != NULL){ // check if we have no elements
		FREE(q->head); // moving head ptr, free previous head
		
		q->head = first;
		value = first->value;	
		first->value = NULL;

		q->size--;
	
	}
	pthread_mutex_unlock(&ptl_lq_mutex); // unlock
	
    return value;
}


/* Retrieves and removes the head of this queue, waiting up to the specified
   wait time if necessary for an element to become available. */
void* ptl_lq_get_wait(ptl_q_t q, long timeout){
	if(q == NULL || q->size <= 0){ return NULL; } // try to break out early if missing info
	
    time_t start_time;
	time_t curr_time;
	time(&start_time);
	double time_diff;
	void *element = NULL;
	
	// keep trying until we reach the max allowed time
	while((element = ptl_lq_get(q)) == NULL){
		
		// check the time
		time(&curr_time);
		time_diff = difftime(curr_time, start_time);
		
		if(timeout >= ((long)time_diff)){
			break; // I chose to use break to get out before the sleep
		}
		
		ptl_timed_wait(3); // wait 3 microseconds
		// this wait time may be a bad idea...
	}
	
	
	// this may be NULL if nothing was retrieved
	return element;
}
