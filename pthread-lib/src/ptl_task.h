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

#ifndef __PTL_TASK_H__
#define __PTL_TASK_H__


#define PTL_TASK_STATE_CREATED 0
#define PTL_TASK_STATE_RUNNING 1
#define PTL_TASK_STATE_DONE 2
#define PTL_TASK_STATE_CANCELLED 3
#define PTL_TASK_STATE_REJECTED 4


struct ptl_task {
	int state;
	void* (*function_to_execute)(void*); 
};


typedef struct ptl_task *ptl_task_t;

/* Public Functions */

/**
 * Create a task that contains a state.
 *
 * @param function that will be executed when this task is consumed
 * @return a non-null 'task'
 */
ptl_task_t create_task(void (*function_to_execute)(void*));

/**
 * Destroy a 'task'. This does not free any memory this 'task' may be pointing
 * to. It only frees the memory create during the 'create' function.
 *
 * @param task task to be destroyed
 */
void destroy_task(ptl_task_t task);
#endif
