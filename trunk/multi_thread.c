#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include "multi_thread.h"
#include "util.h"

/* Work Pool of Threads */
pthread_t *thread_pool = NULL;

/* Mutexes */
pthread_mutex_t stop_mutex;

/* Global Variables */
/* Sizes */
int pool_size = 1;

/* Thread status */
int thread_hold;
int thread_stop = 0;

/* Constants */
const char *CLASS_NM = "multi_thread.c";

/* Private functions */
void  lock_stop();
void  unlock_stop();
void *signal_handler_function(void *functions);

/*****************************************************************************/
/*! @fn void create_threads(void *(*func_ptr)(), void *parameter, int num_threads)
    @brief Creates \a num_threads threads executing \a func_ptr.

    @param void *(void *) func_ptr ptr to the function to be executed by each thread
    @param void *parameter generic parameter, each thread will have it's own local copy.
    @param int num_threads the number of threads to create

    Creates the threads and passes them the function to be executed and the   
    generic parameter as a void ptr. The array of threads are stored in the  
    thread_pool variable.                                                   
*/
/*****************************************************************************/
void create_threads(void *(*func_ptr)(), void *parameter, int num_threads){
  int status;
  int thread_num;
  const char *METHOD_NM = "create_threads: ";

  pool_size = num_threads;

  /* Create memory */
  thread_pool = (pthread_t *)malloc(pool_size * sizeof(*thread_pool));

  if(thread_pool == NULL){
    printf("*** FATAL_ERROR: %s Could not create threads! ***\n", METHOD_NM);
    exit(ERROR_CODE_MALLOC);
  }

  /* Initalize local mutex variables */
  status = pthread_mutex_init(&stop_mutex, NULL);
  check_status(status, "pthread_mutex_init", "stop_mutex bad status");

  /* Set the thread_stop flag to false */
  thread_stop = 0;

  /* Initalize all threads and pass the parameter */
  int i;
  for(i = 0; i < pool_size; i++){
    status = pthread_create(&thread_pool[i], NULL, func_ptr, parameter);
    check_status(status, "pthread_create", "thread bad status");
  }

} /* end create_threads */

/******************************************************************************/
/* Calls pthread_join to syncronize and stop all thread workers. If the       */
/* return value is not equal the worker_num, it gives an error to stdout.     */
/* It also frees the thread_pool and parameter memory if allocated.             */
/* Lastly is destroys all mutexs defined in the create method.                */
/*                                                                            */
/* Parameters: int *: A set of integers that matches the number of threads    */
/*                    defined.  This should contain their status, 1 if ok,    */
/*                    0 if dead or lost.                                      */
/******************************************************************************/
void join_threads(int *t_status){
  int status = 0;
  void *ret_value = 0;
  int will_check_status = 1;
  const char *METHOD_NM = "join_threads: ";

  if(t_status == NULL){
    will_check_status = 0;
    printf("*** Joining on ALL threads reguardless of state ***\n");
  }

  int i;
  for(i = 0; i < pool_size; i++ ) {
    if(will_check_status){

      if(t_status[i]){

        status = pthread_join(thread_pool[i], &ret_value);
        check_status( status, "pthread_join", "bad status");

      } else {
        printf("Thread [%d] is hung, not joining\n", i + 1);
      }

    } else {
      printf("Going to stop thread [%d]\n", i + 1);
      status = pthread_join(thread_pool[i], &ret_value);
      check_status( status, "pthread_join", "bad status");
    }

    /* check return value? */
  }

  printf("Threads Stopped Successfully\n");

  FREE(thread_pool);

  destroy_all_mutexes();

} /* end join_threads */

/******************************************************************************/
/* Checks that status of the function and takes action if the status is       */
/* a non-zero number.                                                         */
/*                                                                            */
/* Parameters: int status - status to be checked                              */
/*             char *api  - name of the function                              */
/*             hcar *msg  - message to be printed (for debug purposes)        */
/******************************************************************************/
void check_status(int status, char *api, char *msg) {
  if( status != 0 ) {
    int _errno = errno;
    if( api )
      printf( "%s ", api );
    printf( "failure" );
    if( msg )
      printf( ": %s", msg );
    printf( " (status = %d, errno = %d)\n", status, _errno );
    exit( 1 );
  }
} /* check_status */

/*****************************************************************************/
/* Does a timed wait on the number of seconds passed in as a parameter.      */
/*                                                                           */
/* Parameters: int wait_secs - time, in seconds, to  wait before waking up   */
/*                             on it's own accord.                           */
/* Notes:                                                                    */
/* A local mutex and cond variable is used and destroyed.                    */
/*****************************************************************************/
int timed_wait(int wait_secs){
  int rc;
  struct timespec   ts;
  struct timeval    tp;
  const char *METHOD_NM = "timed_wait: ";

  memset(&tp,0,sizeof(tp));

  pthread_mutex_t timed_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t  timed_wait_cond = PTHREAD_COND_INITIALIZER;

  rc = gettimeofday(&tp, NULL);
  if(rc){
    printf("%s gettimeofday failed in timed_wait", METHOD_NM);
    return rc;
  }

  /* Lock Mutex */
  int status = pthread_mutex_lock( &timed_wait_mutex);
  check_status( status, "pthread_mutex_lock", "bad status (timed_wait_mutex)");

  /* Get time in seconds */
  ts.tv_sec  = tp.tv_sec;
  ts.tv_nsec = tp.tv_usec * 1000;
  ts.tv_sec += wait_secs;

  /* Perform Wait */
  rc = pthread_cond_timedwait(&timed_wait_cond, &timed_wait_mutex, &ts);

  /* Unlock Mutex */
  status = pthread_mutex_unlock(&timed_wait_mutex);
  check_status( status, "pthread_mutex_unlock", "bad status (timed_wait_mutex)");

  /* Destroy local mutex and cond */
  status = pthread_mutex_destroy(&timed_wait_mutex);
  check_status(status,"pthread_mutex_destroy", "bad status (timed_wait_mutex)");
  status = pthread_cond_destroy(&timed_wait_cond);
  check_status(status,"pthread_cond_destroy", "bad status (timed_wait_cond)");

  return rc;
} /* end timed_wait */

/*****************************************************************************/
/* Does a timed wait on the number of milli seconds passed in as a parameter */
/*                                                                           */
/* Parameters: int wait_secs - time, in milliseconds, to  wait before waking */
/*                             up on it's own accord.                        */
/* Notes:                                                                    */
/* A local mutex and cond variable is used and destroyed.                    */
/*****************************************************************************/
int timed_wait_milli(int wait_secs){
  int rc;
  struct timespec   ts;
  struct timeval    tp;
  const char *METHOD_NM = "timed_wait_milli: ";

  if(wait_secs <= 0){
      return 0;
  }

  memset(&tp,0,sizeof(tp));
  memset(&ts,0,sizeof(ts));

  pthread_mutex_t timed_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t  timed_wait_cond = PTHREAD_COND_INITIALIZER;

  rc = gettimeofday(&tp, NULL);
  if(rc){
    printf("%s gettimeofday failed", METHOD_NM);
    return rc;
  }

  /* Lock Mutex */
  int status = pthread_mutex_lock( &timed_wait_mutex);
  check_status( status, "pthread_mutex_lock", "bad status (timed_wait_mutex)");

  /* Get time in milliseconds */
  /* convert milli to nano */
  wait_secs *= 1000000;
  ts.tv_sec  = tp.tv_sec;
  ts.tv_nsec = tp.tv_usec + wait_secs;

  /* Perform Wait */
  rc = pthread_cond_timedwait(&timed_wait_cond, &timed_wait_mutex, &ts);

  /* Unlock Mutex */
  status = pthread_mutex_unlock(&timed_wait_mutex);
  check_status( status, "pthread_mutex_unlock", "bad status (timed_wait_mutex)");

  /* Destroy local mutex and cond */
  status = pthread_mutex_destroy(&timed_wait_mutex);
  check_status(status,"pthread_mutex_destroy", "bad status (timed_wait_mutex)");
  status = pthread_cond_destroy(&timed_wait_cond);
  check_status(status,"pthread_cond_destroy", "bad status (timed_wait_cond)");

  return rc;

}/* end timed_wait */

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
/* Creates a single thread signal handler to handle all interrupt signals    */
/* for all threads.                                                          */
/* Parameters                                                                */
/* function_ptrs - FUNCTION_PTRS with the term_func_ptr populated,at the     */
/*                 very least.                                               */
/*****************************************************************************/
int signal_handler_create(void *function_ptrs){
  int rc =0;
  pthread_t thread_id;

  rc = pthread_create(&thread_id, NULL, signal_handler_function, (void *)function_ptrs);
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
void *signal_handler_function(void *functions){
  const char *METHOD_NM = "signal_handler_function: ";
  FUNCTION_PTRS *function_ptrs = (FUNCTION_PTRS *)functions;
  sigset_t signals;
  int rc = 0;
  int sig_caught;
  int still_running = 1;

  /* Check incomming parameter */
  if(functions == NULL){
    printf("Error, functions is NULL\n");
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
	printf("%s Caught Signal [%d], Not Stopping.\n", METHOD_NM, sig_caught);
	break;
    } /* end switch */
  } /* end while(still_running) */

  pthread_exit(NULL);
}/* end signal_handler_function */


/* Mutex Operations */
/*************************************/
/*    Stop Working Operations        */
/*************************************/
void lock_stop() {
  int status = pthread_mutex_lock( &stop_mutex );
  check_status( status, "pthread_mutex_lock", "bad status (stop_mutex)");
}

void unlock_stop() {
  int status = pthread_mutex_unlock( &stop_mutex );
  check_status( status, "pthread_mutex_unlock", "bad status (stop_mutex)");
}

/******************************************************************************/
/*********************  Safe/Public Operations  *******************************/
/******************************************************************************/

/******************************************************************************/
/* Destroys all mutexes created in the create_threads() function              */
/*                                                                            */
/* Parameters: None                                                           */
/******************************************************************************/

void destroy_all_mutexes(){
  int status;

  status = pthread_mutex_destroy(&stop_mutex);
  check_status(status,"pthread_mutex_destroy", "bad status (stop_mutex)");
}

/* Thread Operations */
/*************************************************/
/* Safe (Mutex stop_working is locked)           */
/* Sets worker_stop to TRUE.  This method should */
/*  be used to tell the workers to stop.         */
/*************************************************/
void stop_threads() {
  lock_stop();
  thread_stop = 1;
  unlock_stop();
}

/******************************************************************************/
/* See if the stop_threads flag is set to TRUE. If it has been set to true,   */
/* should_stop is set to TRUE as well. This method should be used at the top  */
/* of a while loop.                                                           */
/*                                                                            */
/* Parameters: None                                                           */
/******************************************************************************/
int should_stop() {
  int should_stop = 0;

  lock_stop();
  should_stop = thread_stop;
  unlock_stop();

  return should_stop;
}

void set_stop(int stop){
  lock_stop();
  thread_stop = stop;
  unlock_stop();
}

/******************************************************************************/
/********************  UnSafe/Private Operations  *****************************/
/******************************************************************************/
