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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "gen_queue.h"
#include "util.h"

#define DEFAULT_Q_SIZE 100
#define THREAD_SAFE

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
BOOL add_list(GEN_Q *queue, void *elements, size_t size_of_array, size_t element_size){
  if(queue == NULL || elements == NULL){
    return FALSE;
  }
  
  BOOL success = TRUE;
  BOOL rc = FALSE;
  size_t size = 1;
  size_t total_size = size_of_array;
  int num_elements = total_size/size;
  
  char *char_elements = (char *)elements;
  
  int i;
  for(i=0; i<num_elements; i++){
    rc = add(queue, char_elements + i);
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

  next_id = queue->curr_id + 1;

  /* reset for circular queue */
  if(next_id > queue->capacity){
    next_id = 1;
  }

  if(queue->curr_id == queue->tail_id){
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

/******************************************************************************/
/******************************************************************************/
void print_queue_type(GEN_Q *queue, char *type){
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
    
    int elem = (int)element;
    
    printf("Element %d [%d]\n",i,elem);
  }
  printf("\n");

}

/******************************************************************************/
/******************************************************************************/
void print_actuals(GEN_Q *queue){
  printf(" capacity [%d]\n", queue->capacity);
  printf(" size [%d]\n", queue->size);
  printf(" tail_id [%d]\n", queue->tail_id);
  printf(" curr_id [%d]\n\n", queue->curr_id);
}

/******************************************************************************/
/******************************************************************************/
void print_element(Q_ELEM *elem){
  if(elem != NULL){
    printf("Element Id [%d]\n", elem->id);
    printf("Element Used [%d]\n",elem->used);
    printf("Element Value [%s]\n",elem->value);
  }
}
