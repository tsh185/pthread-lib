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
#include "signal_manager.h"
#include "multi_thread.h"
#include "util.h"

/* Global Variables */
pthread_t *global_signal_manager = NULL;

/* Private Functions */
void  _handle_ping_signal();
void *_signal_handler_function(void *functions);


/*****************************************************************************/
/* Creates a single thread signal handler to handle all interrupt signals    */
/* for all threads.                                                          */
/* Parameters                                                                */
/* function_ptrs - FUNCTION_PTRS with the term_func_ptr populated,at the     */
/*                 very least.                                               */
/*****************************************************************************/
int signal_handler_create(void *function_ptrs){
  int rc =0;
  pthread_t thread;

  rc = pthread_create(&thread, NULL, _signal_handler_function, (void *)function_ptrs);

  global_signal_manager = thread;

  return rc;
}

/*****************************************************************************/
/* This is the function the signal handler executes.                         */
/* It runs until the term signal (SIGTERM) is sent, then it executes the     */
/* defined exit function and exits itself.                                   */
/*                                                                           */
/* Parameters                                                                */
/* functions - FUNCTION_PTRS function with the term_func_ptr member          */
/*             populated with a function.                                    */
/*****************************************************************************/
void *_signal_handler_function(void *functions){
  const char *METHOD_NM = "signal_handler_function: ";
  FUNCTION_PTRS *function_ptrs = (FUNCTION_PTRS *)functions;
  sigset_t signals;
  int rc = 0;
  int sig_caught;
  int still_running = 1;

  /* Check incomming parameter */
  if(functions == NULL){
    LOG_ERROR(METHOD_NM, "Error, void *functions parameter is NULL");
    stop_threads();
    exit(ERROR_CODE_SIG_HANDLER_NO_FUNCTIONS);
  }

  sigfillset(&signals);
  while(still_running){

    rc = sigwait(&signals, &sig_caught);

    printf("%s Caught signal [%d]\n",METHOD_NM,sig_caught);

    switch(sig_caught){
      case SIGTERM:
        still_running = 0;
        if(function_ptrs->term_func_ptr != NULL){
          function_ptrs->term_func_ptr();
        }
        break;
      case SIGUSR1:
        if(function_ptrs->user1_func_ptr != NULL){
          function_ptrs->user1_func_ptr();
        }
        break;
      case SIGUSR2:
        if(function_ptrs->user2_func_ptr != NULL){
          function_ptrs->user2_func_ptr();
        }
        break;
      case SIGHUP:
        //re-read configuration file
        break;
      default:
	printf("%s Caught Unsupported Signal [%d], Igoring.\n", METHOD_NM, sig_caught);
	break;
    } /* end switch */
  } /* end while(still_running) */

  pthread_exit(NULL);
}/* end signal_handler_function */

/*****************************************************************************/
/* Blocks all unix signals to the thread that calls it.                      */
/* Be sure and create a signal handler before this is called.                */
/*****************************************************************************/
int block_all_signals(){

  sigset_t signals;
  sigfillset(&signals);
  /* sigdelset(sigset_t *set, int signo); for removing a signal to poll the threads */
  int rc;

  rc = pthread_sigmask(SIG_BLOCK, &signals, NULL);
  return rc;
}

/*****************************************************************************/
/*!
    @breif Blocks all signals except SIGCHLD

*/
/*****************************************************************************/
int block_most_signals(){
  struct sigaction sig_action;
  int rc;

  /* Block all the signals */
  rc = block_all_signals();

  sigemptyset(&sig_action.sa_mask);
  /* Add SIGCHLD to the list */
  sigaddset(&sig_action.sa_mask, SIGCHLD);

  /* Add a action to the SIGCHLD signal */
  sig_action.sa_handler = _handle_ping_signal;
  sig_action.sa_flags = 0;

  /* Unblock the signals in sig_action.sa_mask */
  rc = pthread_sigmask(SIG_UNBLOCK, &sig_action.sa_mask, NULL);

  /* assign action to the ping signal */
  sigaction(SIGCHLD, &sig_action, NULL);

  return rc;
}

/*****************************************************************************/
/*! @fn void *_handle_ping_signal()
    @brief Handles the ping signal by setting a int to 1 or TRUE

    This function will handle the ping signal by setting it's corresponding
    integer in the \a status_array to be true.  If this value is not
    populated, it means the thread is dead.

*/
/*****************************************************************************/
void _handle_ping_signal(){
  pthread_t self = pthread_self();

  int index = _find_my_index(self);

  set_status_element(index, 1);

#ifdef DEBUG
  printf("Thread %d populated the array with %d\n",index,1);
#endif

}