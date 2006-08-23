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

/*! @file test_task.c
    @brief A set of tests for task.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include <time.h>
#include "util.h"
#include "task.h"

#define NUM_TESTS 3
void *do_something(){
  printf("This if the function assigned to a task\n");
}

BOOL test_0(){
  const char* m = "test_0: ";
  BOOL rc = TRUE;

  init_tasks(1);
  TASK_T *task1 = get_init_task();
  set_id(&task1[0],1);
  set_interval(&task1[0],"00:00:30");
  set_dead_thread_action(&task1[0], kill_program);
  set_work_load_action(&task1[0], increase, add);
  set_work_load_upper_bound(&task1[0], increase, 3);
  set_work_load_lower_bound(&task1[0], increase, -3);
  set_work_load_value(&task1[0], increase, 1);
  set_function_ptr(&task1[0], &do_something);

  struct tm curr_time;
  memset(&curr_time,0,sizeof(struct tm));
  time_t t = time(NULL);
  localtime_r(&t, &curr_time);

  set_schedule(&task1[0], &curr_time, FALSE);
  rc = add_task(&task1[0]);

  if(!rc){printf("%s add of task[0] failed\n",m);}

  int size = get_size();
  if(size <=0 || size > 1){
    printf("%s size does not match. size [%d] \n",m, size);
    rc = FALSE; 
  }

  int capacity = get_capacity();
  if(capacity <= 0 || capacity > 1){
    printf("%s capacity does not match. capacity [%d]\n", m, capacity);
    rc = FALSE;
  }

  /* destroy locally created task */
  destroy_task(task1);
  /* destroy queue of tasks */
  destroy_all_tasks();

  return rc;
}

BOOL test_1(){
  const char* m = "test_1: ";
  BOOL rc = TRUE;

  init_tasks(5);

  int size = get_size();
  if(size != 0){
    printf("%s size does not match. size [%d]\n",m,size);
    rc = FALSE;
  }

  int capacity = get_capacity();
  if(capacity != 5){
    printf("%s capacity does not match. capacity [%d]\n",m,capacity);
    rc = FALSE;
  }

  destroy_all_tasks();

  return rc;
}

BOOL test_2(){
  const char* m = "test_2: ";
  BOOL rc = TRUE;

  init_tasks(10);
  TASK_T *task1 = get_init_task();
  TASK_T *task2 = get_init_task();
  TASK_T *task3 = get_init_task();
  TASK_T *task4 = get_init_task();

  add_task(task1);
  add_task(task2);
  add_task(task3);
  add_task(task4);

  int size = get_size();
  if(size != 4){
    printf("%s size does not match. size [%d]\n",m,size);
    rc = FALSE;
  }

  remove_task();
  remove_task();

  if(size != 2){
    printf("%s size does not match after removal. size [%d]\n",m,size);
    rc = FALSE;
  }

  destroy_task(task1);
  destroy_task(task2);
  destroy_task(task3);
  destroy_task(task4);
  destroy_all_tasks();

  return rc;
}

int main(){
  int results[NUM_TESTS];
  memset(&results,0,sizeof(int)*NUM_TESTS);

  results[0] = test_0();
  CHECK_TEST(0, results[0]);

  results[1] = test_1();
  CHECK_TEST(1, results[1]);

  results[2] = test_1();
  CHECK_TEST(2, results[2]);

  int i;
  for(i=0; i<NUM_TESTS; i++){
    if(results[i] == FALSE){
      return 1;
    }
  }
  printf("All Tests Were Successful!\n");
  return 0;
}
