
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "gen_queue.h"
#include "util.h"

#define DEFAULT_Q_SIZE 100
#define THREAD_SAFE
#define DEBUG

int nulls_ok = 0;

/******************************************************************************/
/******************************************************************************/
GEN_Q *create_queue(int capacity){
  const char* METHOD_NM = "create_queue";
  GEN_Q *queue = (GEN_Q *)malloc(sizeof(GEN_Q));;

  if(capacity <= 0){
    capacity = DEFAULT_Q_SIZE;
  }

  queue->capacity = capacity;
  queue->queue_elements = (Q_ELEM *)malloc(sizeof(Q_ELEM)*capacity);
  memset(queue->queue_elements, 0, sizeof(Q_ELEM)*capacity);

  if(queue->queue_elements == NULL){
    printf("%s malloc failed", METHOD_NM);
    return;
  }

  queue->size = 0;
  queue->tail_id = 0;
  queue->curr_id = 0;

  return queue;

}

/******************************************************************************/
/* Adds an element to the generic queue.                                      */
/* queue - queue that is being used                                           */
/* element - element to be added to the queue                                 */
/* Notes: Increments the tail and sets used to TRUE.                          */
/******************************************************************************/
BOOL add(GEN_Q *queue, void *element){
  if(queue == NULL || element == NULL){
    return FALSE;
  }

  /* if at capacity, can't add more */
  if(queue->size >= queue->capacity){
    return FALSE;
  }

  /* check if we are the end of the circular queue */
  if(queue->tail_id >= queue->capacity){
    queue->tail_id = 1;
  } else {
    queue->tail_id++;
  }

  queue->size++;
  queue->queue_elements[queue->tail_id - 1].id = queue->tail_id;
  queue->queue_elements[queue->tail_id - 1].used = FALSE;
  queue->queue_elements[queue->tail_id - 1].value = element;

  return TRUE;
}
/******************************************************************************/
/******************************************************************************/
BOOL add_list(GEN_Q *queue, void *elements, int size_of_single_element){
  if(queue == NULL || elements == NULL){
    return FALSE;
  }
  
  BOOL success = TRUE;
  BOOL rc = FALSE;
  int size = size_of_single_element;
  int total_size = sizeof(*elements);
  int num_elements = size/total_size;
  
  int i;
  for(i=0; i<num_elements; i++){
    rc = add(queue, (void *)&elements[size]);
    if(!rc){
      success = FALSE;
    }  
    size+=size;
  }

  return success;
}

/******************************************************************************/
/* get_next returns the next element and iterates the curr pointer            */
/* This is a circular queue                                                   */
/******************************************************************************/

/* having problems with this function */
void *get_next(GEN_Q *queue){
  BOOL used = FALSE;
  int next_id = 0;

  if(queue == NULL){
    return NULL;
  }

#ifdef DEBUG
  print_element(&(queue->queue_elements[queue->curr_id]));
#endif

  next_id = queue->curr_id + 1;

  /* reset for circular queue */
  if(next_id > queue->capacity){
    next_id = 1;
  }

  if(queue->curr_id == queue->tail_id || next_id == queue->tail_id){
    return NULL;
  }

  used = queue->queue_elements[next_id - 1].used;
  if(used){
    return NULL;
  }

  queue->size--;
  queue->queue_elements[next_id - 1].used = TRUE;
  queue->curr_id = next_id;

  return queue->queue_elements[queue->curr_id - 1].value;
}

/******************************************************************************/
/******************************************************************************/
void *get_element(int index){
  return NULL;
}

/******************************************************************************/
/******************************************************************************/
void print_queue(GEN_Q *queue){
  if(queue == NULL){
    return;
  }

  void *element = NULL;
  char *char_elem = NULL;

  int size = queue->size;
  Q_ELEM *all_elements = queue->queue_elements;

  int i = 0;
  for (i=0; i<size; i++){
    element = all_elements[i].value;
    char_elem = (char *)element;
    printf("Element %d [%s]\n",i,char_elem);
  }
  printf("\n");

}


void print_actuals(GEN_Q *queue){
  printf(" capacity [%d]\n", queue->capacity);
  printf(" size [%d]\n", queue->size);
  printf(" tail_id [%d]\n", queue->tail_id);
  printf(" curr_id [%d]\n\n", queue->curr_id);
}

void print_element(Q_ELEM *elem){
  if(elem != NULL){
    printf("Element Id [%d]\n", elem->id);
    printf("Element Used [%d]\n",elem->used);
    printf("Element Value [%s]\n",elem->value);
  }
}
