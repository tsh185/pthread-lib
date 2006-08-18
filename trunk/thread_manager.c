#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include "multi_thread.h"
#include "util.h"


pthread_t *pool_to_manage = NULL;
struct timespec wait_time;


/******************************************************************************/
/******************************************************************************/
void create_thread_manager(pthread_t *thread_pool){
    const char *METHOD_NM = "create_thread_manager: ";
    int thread_id;

    if(thread_pool == NULL){
        printf("%s pool to manage is NULL!", METHOD_NM);
        return;
    }

    pool_to_manage = thread_pool;
    rc = pthread_create(&thread_id, NULL, begin_managing, (void *)arg);



}

/******************************************************************************/
/******************************************************************************/
void check_threads(){
}

/******************************************************************************/
/* The default function the thread_manager executes.                          */
/******************************************************************************/
void *begin_managing(){

}