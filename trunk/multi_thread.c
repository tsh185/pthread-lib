#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include "multi_thread.h"

/* Work Pool of Threads */
pthread_t *thread_pool = NULL;

/* Queue of widgets */
void *queue = NULL;

/* Mutexes */
pthread_mutex_t queue_mutex;
pthread_mutex_t stop_mutex;

/* Global Variables */
/* Sizes */
int pool_size = 1;
int queue_size = 0;

/* Thread status */
int thread_hold;
int thread_stop;

/* Queue variables */
void *q_ptr = 0;
int q_avail = 0;

/* Constants */
const char *CLASS_NM = "multi_thread.c";

/* Private functions */
void lock_queue();
void unlock_queue();
void lock_stop();
void unlock_stop();
int _get_queue_size();
void _set_queue_size(int size);
void* _get_queue_ptr();
void _set_queue_ptr(int pos);
void _set_queue_unavailable();
void _set_queue_available();
int _is_queue_available();


/******************************************************************************/
/* Creates the threads and passes them the function to be executed and the    */
/* generic parameter as a void ptr. The array of threads are stored in the    */
/* thread_pool variable.                                                      */
/*                                                                            */
/* Parameters: void *(void *) func_ptr - ptr to the function to be executed   */
/*                                       by each thread.                      */
/*             void *parameter - generic parameter, each thread will have     */
/*                               it's own local copy.                         */
/*             int num_threads - the number of threads to create              */
/******************************************************************************/
void create_threads(void *(*func_ptr)(void *), void *parameter, int num_threads){
  int status;
  int thread_num;
  const char *METHOD_NM = "create_with_parameter(): ";

  pool_size = num_threads;

  /* Create memory */
  thread_pool = (pthread_t *)malloc(pool_size * sizeof(*thread_pool));

  if(thread_pool == NULL){
    printf("*** FATAL_ERROR: %s Could not create threads! ***\n", METHOD_NM);
    exit(1);
  }

  /* Initalize local mutex variables */
  status = pthread_mutex_init(&queue_mutex, NULL);
  check_status(status, "pthread_mutex_init", "queue_mutex bad status");

  status = pthread_mutex_init(&stop_mutex, NULL);
  check_status(status, "pthread_mutex_init", "stop_mutex bad status");

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

    if(((int) ret_value ) != 10 ){
      printf("Worker %d stopped, return value [%d] DOES NOT EQUAL  worker_num [%d]\n", i + 1, (int)ret_value, 10);
    }
  }

  printf("Threads Stopped Successfully\n");

  if(thread_pool){ free(thread_pool); thread_pool = NULL; }

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

  memset(&tp,0,sizeof(tp));

  pthread_mutex_t timed_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t  timed_wait_cond = PTHREAD_COND_INITIALIZER;

  rc = gettimeofday(&tp, NULL);
  if(rc){
    printf("gettimeofday failed in WorkPool_timedWait");
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

  memset(&tp,0,sizeof(tp));
  memset(&ts,0,sizeof(ts));

  pthread_mutex_t timed_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t  timed_wait_cond = PTHREAD_COND_INITIALIZER;

  rc = gettimeofday(&tp, NULL);
  if(rc){
    printf("gettimeofday failed in WorkPool_timedWait");
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
/*****************************************************************************/
int block_all_signals(){

  sigset_t signals;
  sigfillset(&signals);
  int rc;

  rc = pthread_sigmask(SIG_BLOCK, &signals, NULL);
  return rc;
}

/*****************************************************************************/
/*****************************************************************************/
int signal_handler_create(void *arg){
  int rc =0;
  pthread_t thread_id;

  rc = pthread_create(&thread_id, NULL, signal_handler_function, (void *)arg);
  return rc;
}

/*****************************************************************************/
/*****************************************************************************/
void *signal_handler_function(void *functions){
  FUNCTION_PTRS function_ptrs;
  sigset_t signals;
  int rc = 0;
  int sig_caught;
  int still_running = 1;
  void *(*term_func_ptr)(void *) = (void *)term_func;
  void *(*user1_func_ptr)(void *) = (void *)user_func1;
  void *(*user2_func_ptr)(void *) = (void *)user_func2;
  /* FUNC_PTR fun_ptr = (FUNC_PTR)arg; */
   
  sigfillset(&signals);
  while(still_running){
    rc = sigwait(&signals, &sigcaught);
    switch(sig_caught){
      case SIGTERM:
        term_func_ptr();
	still_running = 0;
	break;
      default:
	printf("signal_handler: Caught Signal [%d], Not Stopping.\n", sig_caught);
	break;
    } /* end switch */
  } /* end while(still_running) */

  pthread_exit(NULL);
}/* end signal_handler_function */


/* Mutex Operations */
/*************************************/
/*    Queue Mutex Operations         */
/*************************************/
void lock_queue() {
  int status = pthread_mutex_lock( &queue_mutex );
  check_status( status, "pthread_mutex_lock", "bad status (queue_mutex)");
}

void unlock_queue() {
  int status = pthread_mutex_unlock( &queue_mutex );
  check_status( status, "pthread_mutex_unlock", "bad status (work_queue_mutex)");
}

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

  status = pthread_mutex_destroy(&queue_mutex);
  check_status(status,"pthread_mutex_destroy", "bad status (queue_mutex)");

  status = pthread_mutex_destroy(&stop_mutex);
  check_status(status,"pthread_mutex_destroy", "bad status (stop_mutex)");
}
/* Queue Operations */
/******************************************************************************/
/* Returns the queue_size in the global variable.                             */
/*                                                                            */
/* Parametes: None                                                            */
/******************************************************************************/
int get_queue_size(){
  int size = 0;

  lock_queue();
  size = queue_size;
  unlock_queue();

  return size;
}  

void set_queue_size(int size){
  lock_queue();
  queue_size = size;
  unlock_queue();
}

void* get_queue_ptr(){
  void* ptr = NULL;
  lock_queue();
  ptr = q_ptr;
  unlock_queue();

  return ptr;
}

void increment_queue_ptr(int pos){
  lock_queue();
  q_ptr += (sizeof(*q_ptr))*(pos);
  unlock_queue();
}

/* increments q_ptr */
void* next_queue_element(){
  void *element = NULL;

  lock_queue();
  q_ptr += sizeof(q_ptr);
  element = q_ptr;
  unlock_queue();

  return element;
}  

int is_queue_available(){
  int avail = 0;

  lock_queue();
  avail = q_avail;
  unlock_queue();

  return avail;
}

void set_queue_unavailable(){
  lock_queue();
  q_avail = 0;
  unlock_queue();
}

void set_queue_available(){
  lock_queue();
  q_avail = 1;
  unlock_queue();
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

/******************************************************************************/
/********************  UnSafe/Private Operations  *****************************/
/******************************************************************************/

/*************************************************/
/* Unsafe (No locking)                           */
/* Returns the queue_size in the global variable.*/
/*************************************************/
int _get_queue_size(){
  return queue_size;
}

/*************************************************/
/* Unsafe (No locking)                           */
/* sets the queue_size in the global variable.   */
/*************************************************/
void _set_queue_size(int size){
  queue_size = size;
}

/*************************************************/
/* Unsafe (No locking)                           */
/* Returns the current position of the queue     */
/*  pointer.                                     */
/*************************************************/
void* _get_queue_ptr(){
  return q_ptr;
}

/*************************************************/
/* Unsafe (No locking)                           */
/* sets the current position of the queue        */
/*  pointer.                                     */
/*************************************************/
void _set_queue_ptr(int pos){
  q_ptr = queue;
  q_ptr += sizeof(*q_ptr)*pos;
}

/*************************************************/
/* Unsafe (No locking)                           */
/* Sets the q_avail flag so that the queue is    */
/*  unavailable.  This happens when the queue    */
/*  is being repopulated and needs exclusive     */
/*  access.                                      */
/*************************************************/
void _set_queue_unavailable(){
  q_avail = 0;
}

/*************************************************/
/* Unsafe (No locking)                           */
/* Sets the q_avail flag so that the queue is    */
/*  available.  This happens when the queue      */
/*  is being populated with data and ready to be */
/*  accessed.                                    */
/*************************************************/
void _set_queue_available(){
  q_avail = 1;
}

/*************************************************/
/* Unsafe (No locking)                           */
/* Returns the q_avail flag indicating whether   */
/*  the queue is available or not.               */
/*************************************************/
int _is_queue_available(){
  return q_avail;
}


