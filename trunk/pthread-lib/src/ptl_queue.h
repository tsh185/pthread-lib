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
 
/**
 * This class acts as an interface for all types of queues that pthread-lib can use.
 * Each function will be represented as a pointer.
 */

#ifndef __PTL_QUEUE_H__
#define __PTL_QUEUE_H__

/* A Single Queue Element */
struct ptl_q_element {
	void *value; // value of this element
	struct ptl_q_element *next; // next element in this list
	struct ptl_q_element *prev; // previous element in this list
};
	
/* Essential Data Elements */
struct ptl_q {
	char *type; // string description of this queue (array, linked, etc.)
	long capacity; // total capacity (may be used to restrict size)
	long size; // current size
	struct ptl_q_element *head; // first element
	struct ptl_q_element *tail; // last element
	void *functions;
	/*struct ptl_q_funcs *functions;*/ // functions used to operate on the queue
 };

/* Functions Pointers */
struct ptl_q_funcs {
	
	/** 
	 * Initilizes the queue after the initial creation.
	 */
	void (*ptl_q_init_queue)(struct ptl_q *);
	
	/**
	 * Destroys the queue including all elements inside it.
	 */
	void (*ptl_q_destroy_queue)(struct ptl_q *);
	
	/**
 	 * Inserts the specified element into this queue if it is possible to do so 
 	 * immediately without violating capacity restrictions, returning true 
 	 * upon success and false if no space is currently available.
 	 */
	int (*ptl_q_add)(struct ptl_q*, void *);

	/**
	 * Inserts the specified element at the tail of this queue, waiting if
     * necessary up to the specified wait time for space to become available.
	 */
	int (*ptl_q_add_wait)(struct ptl_q*, void *, long);

	/**
	 * Removes all of the elements from this queue.
	 * It will block all operations until it is finished.
	 */
	void (*ptl_q_clear)(struct ptl_q*);

	/**
	 * Retrieves, but does not remove, the head of this queue.
	 * This function is read-only and therefore does not block.
	 */
	void* (*ptl_q_peek)(struct ptl_q*);

	/**
	 * Retrieves and removes the head of this queue. It will block until an
	 * element is available. 
	 * This function will block once it enters the function mutex.
	 */
	void *(*ptl_q_get)(struct ptl_q*);

	/**
	 * Retrieves and removes the head of this queue, waiting up to the specified
	 * wait time if necessary for an element to become available.
	 * This function will block once it enters the function mutex.
	 */
	void *(*ptl_q_get_wait)(struct ptl_q*, long);

};


/* Typedefs */
typedef struct ptl_q* ptl_q_t;
typedef struct ptl_q_funcs* ptl_q_funcs_t;
typedef struct ptl_q_element* ptl_q_element_t;

/* Public Functions */
 
/**
 * Creates the queue assigning the function pointers.
 * This will create the top-level or parent memory needed for this 'interface'.
 * All other memory shall be created in the various functions supplied in the
 * q_functions parameter.
 *
 * @param q_functions list of functions that will be used to implement the 
 *                    operations
 * @return new memory for this queue
 */
ptl_q_t ptl_q_create_queue(ptl_q_funcs_t q_functions);

/**
 * Creates an element/node that houses the 'value' given to it. This element
 * can be null to create a dummy node, however, logic may think it the end of 
 * the list due to the logic in the 'get' function.
 *
 * @param value element to be wrapped in the ptl_q_element_t
 */
ptl_q_element_t ptl_q_create_element(void *value);

/**
 * Clears and destroys the queue. It calls the destroy_queue function that 
 * was supplied during creation. After that call, it destroys any other memory
 * used to create this queue from a parent level.
 * 
 * @param q non-null queue that will be destroyed
 */
void ptl_q_destroy_queue(ptl_q_t q);
 

 
#endif
