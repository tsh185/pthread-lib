/*
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
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#include "ptl_thread_pool.h"


/* creates the thread pool */
ptl_thread_pool_t ptl_create_thread_pool(int core_pool_size,
										 int max_pool_size,
										 long keep_alive_time){

	ptl_thread_pool_t thread_pool = (ptl_thread_pool_t)malloc(sizeof(struct ptl_thread_pool));
	assert(thread_pool);
											 
	thread_pool->core_pool_size = core_pool_size;
	thread_pool->max_pool_size = max_pool_size;
	thread_pool->keep_alive_time = keep_alive_time;
											 
	thread_pool->current_pool_size = 0;
	thread_pool->threads = NULL;
	thread_pool->completed_tasks = 0;
											 
	/* create memory enough for max_pool_threads */
	thread_pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * max_pool_size);
}
											 
										
