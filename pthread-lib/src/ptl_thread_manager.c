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

public ThreadPoolExecutor(int corePoolSize,
                              int maximumPoolSize,
                              long keepAliveTime,
                              TimeUnit unit,
                              BlockingQueue<Runnable> workQueue,
                              ThreadFactory threadFactory,
                              RejectedExecutionHandler handler);
								  
void create_thread_manager(int core_pool_size, 
						   int max_pool_size, 
						   long keep_alive_time,
						   ptl_q_t work_q,
						   rejected_handler_t rejected_handler);
    

    pthread_t *thread = (pthread_t *)malloc(sizeof(pthread_t));

    /* assign pool to manage */
    pool_to_manage = thread_pool;

    /* create our manager thread */
    rc = pthread_create(thread, NULL, begin_managing, (void *)arg);

    /* save our thread handle */
    global_manager_thread = thread;

    /* set our running flag */
    thread_manager_running = TRUE;

    /* create mutexes */
    status = pthread_mutex_init(&thread_pool_mutex, NULL);
    CHECK_STATUS(status, "pthread_mutex_init", "thread_pool_mutex bad status");
}

 
 
