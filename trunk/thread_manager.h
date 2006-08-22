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

#ifndef __THREAD_MANAGER_H__
#define __THREAD_MANAGER_H__
#include "task.h"

/* Configuration file names */

#define INTERNAL_CONFIG_FILENAME "thread_manager.config"

/* Public Methods */

void create_thread_manager(pthread_t *thread_pool, int num_threads);
int *check_threads(pthread_t *threads, int num_threads);
void read_config_file(char *filename);

print_thread_status(pthread_t *thread_pool, int num_threads);
void set_ping_signal(int signal);

#endif
