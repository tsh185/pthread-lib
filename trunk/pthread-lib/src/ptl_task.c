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
#include "ptl_task.h"
#include "ptl_util.h"

/* create a task in dynamic memory */
ptl_task_t create_task(void (*function_to_execute)(void*)){
	if(function_to_execute == NULL) { return NULL; }
	
	ptl_task_t task = (ptl_task_t)calloc(1, sizeof(struct ptl_task));
	assert(task);
	
	task->function_to_execute = function_to_execute;
	task->state = PTL_TASK_STATE_CREATED;
	
	return task;
}

/* destroy memory for a task. Does not destroy any memory it's pointing to */
void destroy_task(ptl_task_t task){
	if(task == NULL) { return; }
	   
	FREE(task);
}
