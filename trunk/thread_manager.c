#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include "multi_thread.h"
#include "util.h"


pthread_t *pool_to_manage = NULL;
int total_threads;
int ping_signal = 18; /* SIGCHLD */

struct timespec wait_time;

/* Private Functions */
void *begin_managing();
int read_internal_config();


/******************************************************************************/
/******************************************************************************/
void create_thread_manager(pthread_t *thread_pool, int num_threads){
    const char *METHOD_NM = "create_thread_manager: ";
    int thread_id;

    if(thread_pool == NULL){
        printf("%s pool to manage is NULL!", METHOD_NM);
        return;
    }

    read_internal_config();

    pool_to_manage = thread_pool;
    rc = pthread_create(&thread_id, NULL, begin_managing, (void *)arg);


}

/******************************************************************************/
/* The default function the thread_manager executes.                          */
/******************************************************************************/
void *begin_managing(){

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

void read_config_file(char *filename){


}
