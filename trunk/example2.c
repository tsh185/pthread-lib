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

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include "multi_thread.h"
#include "thread_manager.h"


/*
example2.c

This example demonstrates the use of multi_thread.c in it's most simple
form. This example demonstrates the following:
  1.) Creates 5 threads
  2.) Assigns an action to the kill and user1 signals
  3.) sleeps until the KILL signal is issued
  4.) Waits for all the threads to exit

To make the process print "Report 1" to the console, type the following:
  kill -s SIGUSR1 <pid> - with the pid being the pid of this process

To stop the process, type the following:
  kill <pid> - with the pid being the pid of this process


*NOTE: To get the pid of a process, type ps or ps -fu <username>.

*/

void *stop(){
  stop_threads();
}

void *print_report(){
  printf("Report 1\n");
}

void *do_something(){

  /* have this thread block all the signals */
  block_all_signals();

  while(!should_stop()){
    timed_wait(5);
  }
}

int main(){
  /* assign the stop function to the signal handler */
  FUNCTION_PTRS f_ptrs;
  f_ptrs.term_func_ptr = stop;
  f_ptrs.user1_func_ptr = print_report;

  /* create the signal handler */
  signal_handler_create(&f_ptrs);

  pthread_t threads = create_threads(do_something, NULL, 5);

  create_thread_manager(threads, 5);
  read_tasks_from_file();


  while(!should_stop()){
    timed_wait(2);
  }

  join_threads(NULL);

}
