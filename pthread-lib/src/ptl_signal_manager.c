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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include "ptl_signal_manager.h"
#include "ptl_util.h"


/* Global Variables */
int signal_manager_running = 0;

/* Private Functions */
void *_signal_handler_function(void *functions);


/* Creates a single thread to handle all interrupt signals */
int ptl_signal_handler_create(pthread_t sig_mgr, ptl_sh_funcs_t func_ptrs){
  if(func_ptrs == NULL){ return 0; }

  // _signal_handler_function is a private function declared below
  int create_rc = pthread_create(&sig_mgr, NULL, _signal_handler_function, (void *)func_ptrs);
	
  return (create_rc == 0);
}


/* Stops the thread manager by setting a flag and sending a signal. */
void stop_signal_manager(pthread_t sig_mgr){

 /* set flag to stop */
 signal_manager_running = 0;

 /* send signal to thread to break it out of wait */
 pthread_kill(sig_mgr, SIGNAL_HANDLER_DESTROY_SIGNAL);

 destroy_signal_manager(sig_mgr);

}


/* Synchronizes the signal handler and frees the memory. */
int destroy_signal_manager(pthread_t sig_mgr){
  void *ret_value = 0;

  /* wait for the thread to stop */
  int status = pthread_join(sig_mgr, &ret_value);
	
  return (status == 0);
}


/**
 * The function the signal handler executes when a signal is sent to this 
 * process. This function executes one of the function pointers assigned to it
 * during it's creation. The signal handler stays in this function for it's
 * entire life until it is told to break out of the loop.
 *
 * @param void *functions Set of functions to execute when a signal is sent
 */
void *_signal_handler_function(void *functions){
 ptl_sh_funcs_t func_ptrs = (ptl_sh_funcs_t)functions;
 sigset_t signals;
 int rc = 0;
 int sig_caught;

 /* Check incomming parameter */
 assert(functions);

 sigfillset(&signals);
	while(signal_manager_running){ //TODO: this is probably the wrong test condition

   rc = sigwait(&signals, &sig_caught);

  printf("Caught signal [%d]...\n", sig_caught);
	 
   switch(sig_caught){
     case SIGTERM:
       signal_manager_running = 0;
       if(func_ptrs->term_func_ptr != NULL){
         func_ptrs->term_func_ptr();
       }
       break;
     case SIGUSR1:
       if(func_ptrs->user1_func_ptr != NULL){
         func_ptrs->user1_func_ptr();
       }
       break;
     case SIGUSR2:
       if(func_ptrs->user2_func_ptr != NULL){
         func_ptrs->user2_func_ptr();
       }
       break;
     case SIGHUP:
       // do something
       break;
	   // TODO: Do the rest of the signals
     default:
       printf("Caught Unsupported Signal [%d], Igoring.\n", sig_caught);
       break;
   } /* end switch */
 } /* end while(still_running) */

  // we should get here if the thread is told to stop
  pthread_exit(NULL); 
	
}


/* Blocks all signal for the thread calling this function */
int block_all_signals(){

 sigset_t signals;
 sigfillset(&signals);

 int sigmask_rc = pthread_sigmask(SIG_BLOCK, &signals, NULL);
 return (sigmask_rc == 0);
}

// removing the function below due to shift in logic
/*
    Blocks all signals except SIGCHLD

   Assigns the function \a _handle_pint_signal() for when the SIGCHLD
   signal is sent.
*/

//int block_most_signals(){
 //struct sigaction sig_action;
 //int rc = 0;

 /* Block all the signals */
 //rc = block_all_signals();

 //sigemptyset(&sig_action.sa_mask);
 /* Add SIGCHLD to the list */
 //sigaddset(&sig_action.sa_mask, SIGCHLD);

 /* Add a action to the SIGCHLD signal */
// sig_action.sa_handler = _handle_ping_signal;
 //sig_action.sa_flags = 0;


 /* Unblock the signals in sig_action.sa_mask */
 //rc = pthread_sigmask(SIG_UNBLOCK, &sig_action.sa_mask, NULL);

 /* assign action to the ping signal */
 //sigaction(SIGCHLD, &sig_action, NULL);

// return rc;
//}
