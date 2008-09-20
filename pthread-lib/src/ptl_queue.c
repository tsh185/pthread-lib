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

#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <assert.h>
#include "ptl_queue.h"
#include "ptl_util.h"


/* Private Function Declarations */
int _check_function_ptrs(ptl_q_funcs_t q_functions);



/* create the queue and assign the functions */
ptl_q_t ptl_q_create_queue(ptl_q_funcs_t q_functions, int capacity){
	_check_function_ptrs(q_functions);
	
	ptl_q_t q = (ptl_q_t)malloc(sizeof(struct ptl_q));
	assert(q);
	
	q->capacity = capacity;
	q->functions = q_functions; // TODO is this right?
	
	ptl_q_funcs_t funcs = (ptl_q_funcs_t)(q->functions);
	funcs->ptl_q_init_queue(q); // initilize the queue using the supplied function
	
	return q;
}


/* creates an element/node that houses the 'value' given to it  */
ptl_q_element_t ptl_q_create_element(void *value){
	ptl_q_element_t e = (ptl_q_element_t)malloc(sizeof(struct ptl_q_element));
	if(e != NULL){
		e->value = value;
	}

	e->next = NULL;
	e->prev = NULL;
	
	return e;
}


/* free any memory used to create this queue. */
void ptl_q_destroy_queue(ptl_q_t q){
	if(q == NULL) { return; }
	
	ptl_q_funcs_t funcs = (ptl_q_funcs_t)(q->functions);
	
	funcs->ptl_q_destroy_queue(q); // call the destroy function supplied
	
	FREE(q); // free the entire q
	
	return;
}


/* add an element to the queue (calls supplied function) */
int ptl_q_add(ptl_q_t q, void *value){
	if(q == NULL) { return 0; }
	
	ptl_q_funcs_t funcs = (ptl_q_funcs_t)(q->functions);
	
	return funcs->ptl_q_add(q, value);
}


/* add an element to the queue (it may wait for it to have room)
   (calls supplied function) */
int ptl_q_add_wait(ptl_q_t q, void *value, long timeout){
	if(q == NULL) { return 0; }
	
	ptl_q_funcs_t funcs = (ptl_q_funcs_t)(q->functions);
	
	return funcs->ptl_q_add_wait(q, value, timeout);
}


/* look, but don't remove, the first element */
void* ptl_q_peek(ptl_q_t q){
	if(q == NULL) { return NULL; }
	
	ptl_q_funcs_t funcs = (ptl_q_funcs_t)(q->functions);
	
	return funcs->ptl_q_peek(q);
}


/* get and remove the first element */
void* ptl_q_get(ptl_q_t q){
	if(q == NULL) { return NULL; }
	
	ptl_q_funcs_t funcs = (ptl_q_funcs_t)(q->functions);
	
	return funcs->ptl_q_get(q);
}

/* same as ptl_q_get, but it waits if an element is not in the queue */
void* ptl_q_get_wait(ptl_q_t q, long timeout){
	if(q == NULL) { return NULL; }
	
	ptl_q_funcs_t funcs = (ptl_q_funcs_t)(q->functions);
	
	return funcs->ptl_q_get_wait(q, timeout);
}


/* clears the queue of any elements */
void ptl_q_clear(ptl_q_t q){
	if(q == NULL) { return; }
	
	ptl_q_funcs_t funcs = (ptl_q_funcs_t)(q->functions);
	
	return funcs->ptl_q_clear(q);
	
	return;
}


/*
 * Checks to ensure all the function pointers are set.
 * Returns 1 if set, 0 otherwise.
 */
int _check_function_ptrs(ptl_q_funcs_t q_functions) {
	assert(q_functions);
	assert(q_functions->ptl_q_add);
	assert(q_functions->ptl_q_clear);
	assert(q_functions->ptl_q_peek);
	assert(q_functions->ptl_q_get);
	
	return 1;
}
