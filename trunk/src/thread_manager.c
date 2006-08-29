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
#include "util.h"
#include "thread_manager.h"

/* Globals */
pthread_t *global_manager_thread = NULL;
pthread_t *pool_to_manage = NULL;

int total_threads = 0;
int ping_signal = 18; /* SIGCHLD */

int thread_manager_running = FALSE;
pthread_mutex_t thread_manager_running_mutex;

/* Private Functions */
void *begin_managing();
int read_internal_config();


/******************************************************************************/
/******************************************************************************/
void create_thread_manager(pthread_t *thread_pool, int num_threads){
    const char *METHOD_NM = "create_thread_manager: ";
    int status = 0;

    if(thread_pool == NULL){
        printf("%s pool to manage is NULL!", METHOD_NM);
        return;
    }

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

/******************************************************************************/
/******************************************************************************/
void destroy_thread_manager(){
  void *ret_value = 0;

  thread_manager_running = FALSE;

  /* wait until the thread stops */
  int status = pthread_join(global_manager_thread, &ret_value);
  CHECK_STATUS(status, "pthread_join", "bad status");

  cleanup_tasks();

  FREE(global_manager_thread);

}

/******************************************************************************/
/* The default function the thread_manager executes.                          */
/******************************************************************************/
void *begin_managing(){
/*


*/
}

/******************************************************************************/
/******************************************************************************/
void cleanup_tasks(){

}

/******************************************************************************/
/******************************************************************************/
print_thread_status(pthread_t *thread_pool, int num_threads){
  int i;
  int rc = 0;

  int policy;
  struct sched_param sched;
  pthread_id_np_t pthread_id;
  int status;

  for(i=0; i<num_threads; i++){
    printf("Thread %d\n", i);
    rc = pthread_getschedparam(thread_pool[i], &policy, &sched);
    printf("Policy [%d]\n", policy);

    rc = pthread_getunique_np(thread_pool[i], &pthread_id);

    rc = pthread_is_multithreaded_np(thread_pool[i], (void *)(&status));

    printf("Status [%d]\n", status);
    /* int pthread_kill(pthread_t thread, int sig) */
  }
}


/******************************************************************************/
/* Returns dynamically allocated memory */
/******************************************************************************/
int *check_threads(pthread_t *threads, int num_threads){

  if(threads == NULL ||  num_threads == 0){
    return NULL;
  }

  int *status = (int *)malloc(sizeof(int)*num_threads);

  int i;
  /*! send signal \a ping_signal to each thread */
  for(i=0; i<num_threads; i++){
    status[i] = pthread_kill(threads[i], ping_signal);
  }

  return status;

}

/******************************************************************************/
/******************************************************************************/
void set_ping_signal(int signal){
  ping_signal = signal;
}

int read_internal_config(){

  char *sp = NULL;
  char *line = NULL;
  char *str = NULL;
  char *val = NULL;
  int rc = 0;

  config_file = fopen(INTERNAL_CONFIG_FILENAME, READ);

  if(config_file == NULL){
    printf("Configuration file [%s] is null!\n", INTERNAL_CONFIG_FILENAME);
    return;
  }

  while(!feof(config_file)){
    fgets(line,(FILE_LINE_SIZE-1),config_file);

    str=strtok_r(line,"=\0",&sp);
    val=strtok_r(NULL,"# \t\r\n\0", &sp);

    if(!val){ continue; }

    /* start cases here */
    if(strcasecmp(str,"blah")==0){

    }

    /* end cases here */

  }

  fclose(config_file);

}


void read_tasks_from_file(char *filename){

}

void define_task(TASK *task){


}
