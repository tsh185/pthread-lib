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
 
#ifndef __PTL_THREAD_POOL_H__
#define __PTL_THREAD_POOL_H__

/* Structures */
struct ptl_thread_pool {
	int core_pool_size;			/**< core size of the pool this manager is managing*/
	int max_pool_size;			/**< max size of the pool this manager is managing*/ 
	int current_pool_size;		/**< current pool size (between core and max) */
	long keep_alive_time;		/**< keep alive time of each thread in the pool */
	pthread_t *threads;			/**< actual threads */
	long completed_tasks;		/**< number of tasks this thread completed */
};



/* Type Definitions */
typedef struct ptl_thread_pool *ptl_thread_pool_t;

/* Methods */

/**
 * Creates the thread pool that a manager can manage.
 * When create_thread_manager is called, a thread pool is created.
 */
ptl_thread_pool_t ptl_create_thread_pool(int core_pool_size,
										 int max_pool_size,
										 long keep_alive_time);


#endif
