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
 * This "class" is an array implemention of a queue. It can be used with the
 * ptl_queue "interface" to have a finite, bounded set of work that can be
 * executed using the thread pool. This queue uses simple locking/blocking
 * operations for all get and put operations.
 */


#ifndef __PTL_ARRAY_QUEUE_H__
#define __PTL_ARRAY_QUEUE_H__

/**
 * Destroys the queue and frees the memory. This should be used when the queue
 * is no longer going to be used.
 * 
 * @param q the queue to destroy
 */
void ptl_aq_destroy_queue(ptl_q_t q);

/**
 * Initializes the queue, creating all memory needed to support this data
 * structure.
 * 
 * @param q queue to be initized.
 */
void ptl_aq_init_queue (ptl_q_t q);

/**
 * Inserts the specified element into this queue. Returns true
 * upon success and false if no space is currently available.
 *
 *
 * @param q non-null queue
 * @param value the value to be stored in the queue
 * @return 1 if successful, 0 otherwise
 */
int ptl_aq_add(ptl_q_t q, void *value);

/**
 * Tries to insert the item into the array queue. If there is not room, it will
 * wait until there is room or until 'timeout' occurs.
 *
 * 
 * @param q non-null queue to add the value
 * @param value data that will be added to the queue
 * @param timeout this parameter is ignored
 * @return 1 if successful, 0 otherwise
 * @see ptl_lq_add()
 **/
int ptl_aq_add_wait(ptl_q_t q, void *value, long timeout);

/**
 * Removes all of the elements from this queue freeing memory as it iterates
 * through. Please note, it does not free the 'values' put in the list using add.
 * To provide your own function to free the 'values', 
 * please use ptl_lq_clear_freefunc().
 *
 * @param q non-null queue to be cleared
 * @see ptl_lq_clear_freefunc()
 */
void ptl_aq_clear(ptl_q_t q);

/**
 * Removes all of the elements from this queue freeing memory as it iterates
 * through. It frees the 'values' put in the list under add using the function
 * provided in the free_func parameter. To use the default free, use 
 * ptl_lq_clear().
 *
 * @param q non-null queue to be cleared
 * @param free_func function that will be used to free the 'value' elements
 * @see ptl_lq_clear()
 */
void ptl_aq_clear_freefunc(ptl_q_t q, void (*free_func)(void *)); 

/**
 * Retrieves, but does not remove, the head of this queue.
 *
 * @param q non-null queue to peek on
 * @return pointer to the head element or NULL if no element was found
 */
void* ptl_aq_peek(ptl_q_t q);

/**
 * Retrieves and removes the head of this queue. It will return null if the 
 * queue is empty. This function is blocks once it tries to get an element.
 *
 * @param q non-null queue to get an element from
 * @return the head element or NULL if no element was found
 */
void* ptl_aq_get(ptl_q_t q);

/**
 * Retrieves and removes the head of this queue, waiting up to the specified
 * wait time if necessary for an element to become available.
 * This function will block only when trying to get an element, meaning another
 * thread may beat 'this' thread to the next element.
 *
 * @note one may experience bad performance by continually checking if an element
 *       is available and blocking until we reach the timeout value.
 *
 * @param q non-null queue to get an element from
 * @param timeout time in seconds
 * @return the head element or NULL if no element was found
 */
void* ptl_aq_get_wait(ptl_q_t q, long timeout);


#endif
