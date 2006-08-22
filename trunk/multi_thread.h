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

#ifndef __MULTI_THREAD_H__
#define __MULTI_THREAD_H__

#include <pthread.h>
#include "util.h"

/* Error Codes */
#define ERROR_CODE_MALLOC 1
#define ERROR_CODE_SIG_HANDLER_NO_FUNCTIONS 2


typedef void (*FUNC_PTR)();

/* Thread Operations */
pthread_t *create_threads(void *(*func_ptr)(), void *parameter, int num_threads);
void join_threads(int *t_status);
void check_status(int status, char *api, char *msg);
int  timed_wait(int wait_secs);
int  timed_wait_milli(int wait_secs);
void destroy_all_mutexes();

void stop_threads();
int  should_stop();

BOOL set_status_element(int index, int status);
int *get_status_array();
void init_status_array();
void create_status_array();
int  block_most_signals();
int  get_pool_size();


struct function_ptrs_struct {
  void *(*term_func_ptr)();
  void *(*user1_func_ptr)();
  void *(*user2_func_ptr)();
};
typedef struct function_ptrs_struct FUNCTION_PTRS;

#endif
