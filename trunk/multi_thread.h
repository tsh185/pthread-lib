/*
    pthread-lib is a set of pthread wrappers with additional features.
    Copyright (C) 2006  Nick Powers
    See <http://code.google.com/p/pthread-lib/> for more details and source.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*! @file multi_thread.h
    @brief The header file to multi_thread.c
*/

#ifndef __MULTI_THREAD_H__
#define __MULTI_THREAD_H__

#include <pthread.h>
#include "util.h"

/* Error Codes */
#define ERROR_CODE_SIG_HANDLER_NO_FUNCTIONS 2

#define DEFAULT_NUM_POOLS 5

/* Structs */
struct thread_pool_struct {
    int id;
    int capacity;
    int size;
    pthread_t *pool;
    int use_global_stop;
    int stop;
}
typedef struct thread_pool_struct THREAD_POOL;

/* Public Methods */
pthread_t *create_threads(void *(*func_ptr)(), void *parameter, int num_threads);
void join_threads(int *t_status);
int  timed_wait(int wait_secs);
int  timed_wait_milli(int wait_secs);

void stop_threads();
int  should_stop();

BOOL set_status_element(int index, int status);
int *get_status_array();
void init_status_array();
void create_status_array();
int  get_pool_size();


#endif
