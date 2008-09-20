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

 /*
  * For a "class" description, see the header file. 
  */

#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "ptl_queue.h"
#include "ptl_util.h"
#include "ptl_array_queue.h"


/* Private Functions */
int _ptl_aq_is_at_end(ptl_q_t q, ptl_q_element_t ptr);


/* Global Variables */
pthread_mutex_t ptl_aq_mutex = PTHREAD_MUTEX_INITIALIZER;


/* initalize the ptl_q structure for an array queue */
void ptl_aq_init_queue (ptl_q_t q){
	assert(q);
	
	pthread_mutex_lock(&ptl_aq_mutex); // lock
	
	strncpy(q->type, "array", PTL_Q_TYPE_LENGTH);
	// capacity is already set
	q->size = 0;
	q->head = NULL; // this will point to the beginning of the array
	q->tail = NULL; // this will point to the end of the array
	q->ptr = NULL; // this will point to the beginning of the array always
	// functions is already set
	
	// create our finite array with a size of 'capacity'
	ptl_q_element_t array = (ptl_q_element_t)calloc(q->capacity, sizeof(struct ptl_q_element));
	assert(array);
	q->head = q->tail = q->ptr = array;
	
	pthread_mutex_unlock(&ptl_aq_mutex); // unlock
	
	return;
}


/* free all the memory associated with an array queue */
void ptl_aq_destroy_queue(ptl_q_t q){
	assert(q);
	
	pthread_mutex_lock(&ptl_aq_mutex); // lock
	
	strncpy(q->type, "\0", PTL_Q_TYPE_LENGTH);
	q->capacity = 0;
	q->size = 0;
	FREE(q->head); // free our dynamic array memory
	q->tail = NULL;
	
	pthread_mutex_unlock(&ptl_aq_mutex); // unlock
	
	return;
}


/* try to add to end, if at capacity, return 0 */
int ptl_aq_add(ptl_q_t q, void *value){
	if(q == NULL || value == NULL){ return 0;}
	
	// take from head, put at tail
	pthread_mutex_lock(&ptl_aq_mutex); // lock
	
	int at_capacity = q->size == q->capacity;
	// since we take from the head, we should always be able to add at
	// size, unless we are at capacity
	
	if(!at_capacity){ // also head should equal tail
		q->tail->value = value; // assign the value
		
		// increment tail - ensure it goes to zero if at 'capacity'
		if(_ptl_aq_is_at_end(q, q->tail)){
			q->tail = q->ptr; // point to the "beginning" of the list
		} else {
			q->tail++; // point to next element
		}
		
		q->size++; // increment our size
	}
	
	pthread_mutex_unlock(&ptl_aq_mutex); // unlock
	
	return at_capacity;
}


/* try to add, if it fails, try again until timeout */
int ptl_aq_add_wait(ptl_q_t q, void *value, long timeout){
	if(q == NULL || value == NULL || timeout < 0) { return 0; }
	
	time_t start_time;
	time_t curr_time;
	time(&start_time);
	double time_diff;
	int added_a_element = 0;
	
	// while our add fails...
	while(!(added_a_element = ptl_aq_add(q, value))){
		// check the time
		time(&curr_time);
		time_diff = difftime(curr_time, start_time);
		
		if(timeout >= ((long)time_diff)){
			break; // I chose to use break to get out before the sleep
		}
		
		ptl_timed_wait(3); // wait 3 microseconds
		// this wait time may be a bad idea...later rewrite to use conds
	}
	
	return added_a_element;
}


/* clear the elements from the list. The 'value' elements aren't freed */
void ptl_aq_clear(ptl_q_t q){
	if(q == NULL) { return; }
	
	pthread_mutex_lock(&ptl_aq_mutex); // lock

	memset(q->ptr, 0, sizeof(struct ptl_q_element) * q->capacity);
	
	pthread_mutex_unlock(&ptl_aq_mutex); // unlock
	
	return;
}


/* clear the elements from the list. Free the'value' elements using 
   the supplied function */
void ptl_aq_clear_freefunc(ptl_q_t q, void (*free_func)(void *)){
	
	pthread_mutex_lock(&ptl_aq_mutex); // lock

	// interate through and free all 'value' elements
	ptl_q_element_t ptr = q->ptr;
	int i = 0; 
	for(i=0; i < q->capacity;i++){
		ptr += i; // move to desired position
		free_func(ptr->value); // call the free function for 'value'
		ptr->value = NULL; //set it to null
	}
	
	pthread_mutex_unlock(&ptl_aq_mutex); // unlock
	
	return;
}


/* looks at and returns the first element, but does not remove */
void* ptl_aq_peek(ptl_q_t q){
	if(q == NULL){ return NULL; }
	
	pthread_mutex_lock(&ptl_aq_mutex); // lock
	
	void* value = q->head->value;
	// don't decrement size
	// don't move 'head'
	
	pthread_mutex_unlock(&ptl_aq_mutex); // unlock
	
	return value;
}


/* gets and removes the first element */
void* ptl_aq_get(ptl_q_t q){
	if(q == NULL){ return NULL; }
	
	pthread_mutex_lock(&ptl_aq_mutex); // lock
	
	void* value = NULL;
	
	// check if we have anything in the queue first
	if(q->size > 0){
		// take from head, put at tail
		value = q->head->value;
		q->head->value = NULL;
		
		// time to increment 'head'
		if(_ptl_aq_is_at_end(q, q->head)){
			q->head = q->ptr; // set to beginning of memory
		} else {
			q->head++; // just increment
		}
		
		q->size++;
		
	}
	
	pthread_mutex_unlock(&ptl_aq_mutex); // unlock
	
	return value;
}


/* try to get an element, if no elements exist, then keep 
   trying until 'timeout' */
void* ptl_aq_get_wait(ptl_q_t q, long timeout){
	if(q == NULL || timeout < 0) { return NULL; }
	
	time_t start_time;
	time_t curr_time;
	time(&start_time);
	double time_diff;
	void* element = NULL;
	
	while((element = ptl_aq_get(q)) == NULL){
		
		// check the time
		time(&curr_time);
		time_diff = difftime(curr_time, start_time);
		
		if(timeout >= ((long)time_diff)){
			break; // I chose to use break to get out before the sleep
		}
		
		ptl_timed_wait(3); // wait 3 microseconds
		// this wait time may be a bad idea...later rewrite to use conds
	}
	
	return element;
}


/* checks of 'ptr' is pointing to the end of the list */
int _ptl_aq_is_at_end(ptl_q_t q, ptl_q_element_t ptr){
	ptl_q_element_t end_of_list = q->ptr + (q->capacity - 1);
	
	return ptr == end_of_list;
}
