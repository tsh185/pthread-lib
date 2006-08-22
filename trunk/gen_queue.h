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
void print_queue_type(GEN_Q *queue, char*);
void print_actuals(GEN_Q *queue);
void print_element(Q_ELEM *elem);

#endif
