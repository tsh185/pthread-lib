#ifndef __GEN_QUEUE_H__
#define __GEN_QUEUE_H__

#include "util.h"

struct q_element {
  int id;
  BOOL used;
  void *value;
};

struct gen_queue {
  int capacity;
  int size;
  int tail_id;
  int curr_id;
  struct q_element *queue_elements;
};

typedef struct gen_queue GEN_Q;
typedef struct q_element Q_ELEM;

GEN_Q *create_queue(int capacity);
BOOL add(GEN_Q *queue, void *element);
void *get_next(GEN_Q *queue);
void print_queue(GEN_Q *queue);
void print_actuals(GEN_Q *queue);

#endif
