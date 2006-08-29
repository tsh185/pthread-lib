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

/*! @file multi_thread.c
    @brief A set of classes and wrappers to make using the pthread library easy
*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include <gdsl.h>
#include "multi_thread.h"
#include "util.h"

/* Global Variables */
gdsl_hash_t thread_pool_hash = NULL;
int thread_pool_hash_capacity = INITIAL_HASH_TABLE_SIZE;
int thread_pool_id = 0;

/* Mutexes */
pthread_mutex_t thread_pool_hash_mutex;
pthread_mutex_t global_stop_mutex;

int *status_array = NULL; /* used by the thread manager */
int sizeof_status_array = 0;

/* Global Thread status */
int global_stop = FALSE; /* used to stop all threads in all pools */

/* Private functions */
void _create_local_mutexes();
void _destroy_all_mutexes();

void  _lock_hash();
void  _unlock_hash();
void  _lock_global_stop();
void  _unlock_global_stop();

gdsl_element_t _thread_pool_alloc(void *e);
void _thread_pool_free(gdsl_element_t e);
const char* _thread_pool_key(gdsl_element_t e);
void _thread_pool_print(gdsl_element_t e);

/*****************************************************************************/
/*! @fn void create_thread_pool(void *(*func_ptr)(), void *parameter, int num_threads)
    @brief Creates \a num_threads threads executing \a func_ptr.

    @param void *(void *) func_ptr ptr to the function to be executed by each thread
    @param void *parameter generic parameter, each thread will have it's own local copy.
    @param int num_threads the number of threads to create
    @return int Id Id of the \a THREAD_POOL 

    Creates the threads and passes them the function to be executed and the
    generic parameter as a void ptr. The array of threads are stored in a 
    hash table
*/
/*****************************************************************************/
int create_thread_pool(void *(*func_ptr)(), void *parameter, int num_threads){
  int status;
  int thread_num;
  int id = 0;
  const char *METHOD_NM = "create_thread_pool: ";
  pthread_t *threads = NULL;
  THREAD_POOL pool;

  if(func_ptr == NULL || num_threads <= 0){
    return -1;
  }

  /* init structures */
  memset(&pool,0,sizeof(pool));

  id = thread_pool_id++;
  /* if first time, create hash table*/
  if(thread_pool_hash == NULL){
    thread_pool_hash = gdsl_hash_alloc(THREAD_POOL_HASH_TABLE_NAME, _thread_pool_alloc,
		                       _thread_pool_free, _thread_pool_key, NULL, 
				       thread_pool_hash_capacity);

    CHECK_MALLOC(thread_pool_hash, METHOD_NM, "Failed creating hash table for thread pool");

    _create_local_mutexes();
    global_stop = FALSE;
  }

  /* Create memory for threads */
  threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
  CHECK_MALLOC(threads,METHOD_NM,"Failed on malloc of pool");

  /* Create memory for the parameter */
  pool.parameter.id = id;
  pool.parameter.user_parameter = parameter;

  /* Initalize all threads and pass the parameter */
  /* Note: the user supplied parameter is wrapped in the private parameter */
  int i;
  for(i = 0; i < num_threads; i++){
    status = pthread_create(&threads[i], NULL, func_ptr, (void *)&(pool.parameter));
    CHECK_STATUS(status, "pthread_create", "thread bad status");
  }

  /* setup THREAD_POOL object */
  pool.id = id;
  /* convert int id to char id */
  INT_TO_CHAR(pool.id,pool.char_id);
  pool.capacity = num_threads;
  pool.size = num_threads;
  pool.pool = threads;
  pool.use_global_stop = FALSE;
  pool.stop = FALSE;

  gdsl_element_t elem = gdsl_hash_insert(thread_pool_hash, (void *)&pool);
  if(elem == NULL){
    LOG_ERROR("%s unable to insert into Thread Pool Hash Table", METHOD_NM);
    /* destroy threads */
    FREE(threads);
    return -1;
  }

  return pool.id;
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
void join_threads(int pool_id){ /* may need to put a function to free the parameter ptr */
  int status;
  void *ret_value = 0;
  const char *METHOD_NM = "join_threads: ";

  char key[CHAR_ID_LEN];
  memset(key,0,sizeof(key));

  /* convert to char */
  sprintf(key, "%i", pool_id);
  gdsl_element_t e = gdsl_hash_remove(thread_pool_hash, key);

  if(e == NULL){
    LOG_ERROR("%s unable to find Thread Pool with id [%s]\n",METHOD_NM, key);
    return;
  }

  THREAD_POOL *p = (THREAD_POOL *)e;

  int i;
  for(i = 0; i < p->size; i++ ) {
        status = pthread_join(p->pool[i], &ret_value);
        CHECK_STATUS(status, "pthread_join", "bad status");
  }

  printf("%s Threads Stopped Successfully\n",METHOD_NM);

  /* free all memory */
  FREE(p->pool);
  FREE(p);

} /* end join_threads */

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
    LOG_ERROR(METHOD_NM, "gettimeofday failed in timed_wait");
    return rc;
  }

  /* Lock Mutex */
  int status = pthread_mutex_lock( &timed_wait_mutex);
  CHECK_STATUS(status, "pthread_mutex_lock", "bad status (timed_wait_mutex)");

  /* Get time in seconds */
  ts.tv_sec  = tp.tv_sec;
  ts.tv_nsec = tp.tv_usec * 1000;
  ts.tv_sec += wait_secs;

  /* Perform Wait */
  rc = pthread_cond_timedwait(&timed_wait_cond, &timed_wait_mutex, &ts);

  /* Unlock Mutex */
  status = pthread_mutex_unlock(&timed_wait_mutex);
  CHECK_STATUS(status, "pthread_mutex_unlock", "bad status (timed_wait_mutex)")

  /* Destroy local mutex and cond */
  status = pthread_mutex_destroy(&timed_wait_mutex);
  CHECK_STATUS(status,"pthread_mutex_destroy", "bad status (timed_wait_mutex)");

  status = pthread_cond_destroy(&timed_wait_cond);
  CHECK_STATUS(status,"pthread_cond_destroy", "bad status (timed_wait_cond)");

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
    LOG_ERROR(METHOD_NM, "gettimeofday failed");
    return rc;
  }

  /* Lock Mutex */
  int status = pthread_mutex_lock( &timed_wait_mutex);
  CHECK_STATUS(status, "pthread_mutex_lock", "bad status (timed_wait_mutex)");

  /* Get time in milliseconds */
  /* convert milli to nano */
  wait_secs *= 1000000;
  ts.tv_sec  = tp.tv_sec;
  ts.tv_nsec = tp.tv_usec + wait_secs;

  /* Perform Wait */
  rc = pthread_cond_timedwait(&timed_wait_cond, &timed_wait_mutex, &ts);

  /* Unlock Mutex */
  status = pthread_mutex_unlock(&timed_wait_mutex);
  CHECK_STATUS(status, "pthread_mutex_unlock", "bad status (timed_wait_mutex)");

  /* Destroy local mutex and cond */
  status = pthread_mutex_destroy(&timed_wait_mutex);
  CHECK_STATUS(status,"pthread_mutex_destroy", "bad status (timed_wait_mutex)");

  status = pthread_cond_destroy(&timed_wait_cond);
  CHECK_STATUS(status,"pthread_cond_destroy", "bad status (timed_wait_cond)");

  return rc;

}/* end timed_wait */


/* Mutex Operations */
void _lock_global_stop() {
  int status = pthread_mutex_lock(&global_stop_mutex);
  CHECK_STATUS(status, "pthread_mutex_lock", "bad status (global_stop_mutex)");
}

void _unlock_global_stop() {
  int status = pthread_mutex_unlock(&global_stop_mutex);
  CHECK_STATUS(status, "pthread_mutex_unlock", "bad status (global_stop_mutex)")
}

void _lock_thread_pool_hash(){
  int status = pthread_mutex_lock(&thread_pool_hash_mutex);
  CHECK_STATUS(status, "pthread_mutex_lock", "bad status (thread_pool_hash_mutex)");
}

void _unlock_thread_pool_hash(){
  int status = pthread_mutex_unlock(&thread_pool_hash_mutex);
  CHECK_STATUS(status, "pthread_mutex_lock", "bad status (thread_pool_hash_mutex)");
}

/******************************************************************************/
/******************************************************************************/
void _create_local_mutexes(){
  int status;

  status = pthread_mutex_init(&thread_pool_hash_mutex, NULL);
  CHECK_STATUS(status, "pthread_mutex_init", "thread_pool_hash_mutex bad status");

  status = pthread_mutex_init(&global_stop_mutex, NULL);
  CHECK_STATUS(status, "pthread_mutex_init", "global_stop_mutex bad status");
}

/******************************************************************************/
/******************************************************************************/
void _destroy_all_mutexes(){
  int status;

  status = pthread_mutex_destroy(&global_stop_mutex);
  CHECK_STATUS(status,"pthread_mutex_destroy", "bad status (global_stop_mutex)");

  status = pthread_mutex_destroy(&thread_pool_hash_mutex);
  CHECK_STATUS(status,"pthread_mutex_destroy", "bad status (thread_pool_hash_mutex)");
}

/******************************************************************************/
/*********************  Safe/Public Operations  *******************************/
/******************************************************************************/


/* Thread Operations */
/*************************************************/
/*! @fn void stop_all_threads()
    @brief Stops all threads in all pools
 */
/*************************************************/
void stop_all_pools() {
  _lock_global_stop();
  /* set each stop variable in each thread pool to true */
  global_stop = TRUE;
  _unlock_global_stop();
}

/******************************************************************************/
/*! @fn int should_stop()
    @brief Returns the should_stop variable.

    This function can be used in a while loop to determine if the threads
    should stop.

    This function has a mutex and, therefore, is thread safe.
*/
/******************************************************************************/
int should_stop_all() {
  int should_stop = 0;

  _lock_global_stop();
  should_stop = global_stop;
  _unlock_global_stop();

  return should_stop;
}

/******************************************************************************/
/*! @fun void set_stop(int stop)
*/
/******************************************************************************/
int should_stop_pool(int id){
  int stop;

  char key[CHAR_ID_LEN];
  memset(key,0,sizeof(key));

  /* convert to char */
  sprintf(key, "%i", id);
  gdsl_element_t e = gdsl_hash_search(thread_pool_hash, key);

  if(e == NULL){
    LOG_ERROR("Unable to find pool to see if we should_stop for id [%s]\n", key);
    return FALSE;
  }

  THREAD_POOL *p = (THREAD_POOL *)e;

  LOCK(&(p->mutexes.stop_mutex));
  stop = p->stop;
  UNLOCK(&(p->mutexes.stop_mutex));

  return stop;
}

/******************************************************************************/
/******************************************************************************/
void stop_pool(int id){
  char key[CHAR_ID_LEN];
  memset(key,0,sizeof(key));

  INT_TO_CHAR(id,key);
  gdsl_element_t e = gdsl_hash_search(thread_pool_hash, key);

  if(e == NULL){
    LOG_ERROR("Unable to find pool to stop the threads for id [%d]\n",key);
    return;
  }

  THREAD_POOL *p = (THREAD_POOL *)e;

  LOCK(&(p->mutexes.stop_mutex));
  p->stop = TRUE;
  UNLOCK(&(p->mutexes.stop_mutex));
}

/******************************************************************************/
/*! @fn int create_status_array()
    @brief Creates the array in which the threads update to indicate status

    This status is the pid of the thread.  If this status is unable to
    be updated, this means the thread is dead. If the malloc fails
    this thread will exit.

    This function has a mutex and, therefore, is thread safe.
*/
/******************************************************************************/
/*
void create_status_array(){
  const char *METHOD_NM = "create_status_array: ";
  int rc = 0;

  _lock_status_array();

  FREE(status_array);
  sizeof_status_array = 10;
  status_array = (int *)malloc(sizeof(int)*sizeof_status_array);

  if(status_array == NULL){
    LOG_ERROR(METHOD_NM, "Unable to malloc memory for status_array");
    rc = ERROR_CODE_MALLOC;
  }

  _unlock_status_array();

  if(rc){ exit(rc);}
}
  */

/******************************************************************************/
/*!  @fn void init_status_array()
     @brief Initializes the \a status_array variable

     If the pool_size has changed, recreate the \a status_array. Otherwise,
     just reset all ints to 0. This function has a mutex lock and, therefore,
     is thread safe.
*/
/******************************************************************************/
/*
void init_status_array(){
  if(10 != sizeof_status_array){
    create_status_array();
  } else {
    _lock_status_array();

    memset(status_array, 0, sizeof(int)*sizeof_status_array);

    _unlock_status_array();
  }
}
*/

/******************************************************************************/
/*! @fn int *get_status_array()
    @brief Returns the \a status_array variable

    This function has a mutex lock around it and, therefore, is thread safe.
*/
/******************************************************************************/
/*
int *get_status_array(){
  int *sa = NULL;

  _lock_status_array();
  sa = status_array;
  _unlock_status_array();

  return sa;
}
*/
/******************************************************************************/
/*! @fn BOOL set_status_element(int index, int status)
    @brief Sets a single int of the \a status_array variable
    @param int index the index of the element that will be set
    @param int status the status that will be set in the index-th element

    This function is used via each thread to report it's status.  Each thread
    should set the element to a unique number.

*/
/******************************************************************************/
/*
BOOL set_status_element(int index, int status){
  BOOL rc = FALSE;

  _lock_status_array();

  if(index < sizeof_status_array){
    status_array[index] = status;
    rc = TRUE;
  }

  _unlock_status_array();

  return rc;
}
*/
/******************************************************************************/
/********************  UnSafe/Private Operations  *****************************/
/******************************************************************************/
gdsl_element_t _thread_pool_alloc(void *e){
  const char *METHOD_NM = "_thread_pool_alloc: ";

  THREAD_POOL *p = (THREAD_POOL *)e;

  THREAD_POOL *pool = (THREAD_POOL *)malloc(sizeof(THREAD_POOL));
  CHECK_MALLOC(pool, METHOD_NM, "malloc failed on pool"); 
  memset(pool,0,sizeof(THREAD_POOL));

  memcpy(pool, p, sizeof(THREAD_POOL));

  return(gdsl_element_t)pool;
}

/******************************************************************************/
/******************************************************************************/
void _thread_pool_free(gdsl_element_t e){
  if(e == NULL){
    return;
  }

  THREAD_POOL *p = (THREAD_POOL *)e;
  FREE(p->pool);
  FREE(p);
}

/******************************************************************************/
/******************************************************************************/
const char * _thread_pool_key(gdsl_element_t e){
  THREAD_POOL *p = (THREAD_POOL *)e;
  return p->char_id;
}

/******************************************************************************/
/******************************************************************************/
void _thread_pool_print(gdsl_element_t e){
  if(e == NULL){
    return;
  }

  THREAD_POOL *p = (THREAD_POOL *)e;

  printf("Id [%d]\n",p->id);
  printf("Capacity [%d]\n",p->capacity);
  printf("Size [%d]\n", p->size);
  printf("Pool [%d]\n", p->pool);
  printf("Use Global Stop? [%d]\n", p->use_global_stop);
  printf("Stop? [%d]\n",p->stop);
}
