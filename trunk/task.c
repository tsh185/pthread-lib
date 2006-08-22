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
#include "util.h"
#include "task.h"


TASK *tasks = NULL;
int capacity = 0;
TASK *next_empty_index = NULL;
int size;


/*****************************************************************************/
/*! @fn void init_task(int num_tasks);
    @brief Initializes the tasks
    @param int num_tasks The number of tasks that can be added initally

    This function must be called before any tasks are added.
*/
/*****************************************************************************/
void init_tasks(int num_tasks){
  const char *METHOD_NM = "init_task: ";

  if(num_tasks <= 0){
    return;
  }

  tasks = (TASK *)malloc(sizeof(TASK)*num_tasks);
  CHECK_MALLOC(tasks,METHOD_NM,"malloc of tasks failed!");

  memset(tasks, 0, sizeof(TASK)*capacity);
  capacity = num_tasks;
  next_empty_index = 0;
  size = 0;
}

/*****************************************************************************/
/*! @fn void init_task_default()
    @brief Initializes the tasks with the default number of tasks
    
    Calls init_tasks with a default parameter.
*/
/*****************************************************************************/
void init_tasks_default(){
  init_task(INITIAL_NUM_TASKS);
}

/*****************************************************************************/
/*! @fn BOOL add_task(TASK *task)
    @brief Copies the task from source into the next available spot
*/
/*****************************************************************************/
BOOL add_task(TASK *task){
  if(task == NULL){
    return FALSE;
  }

  if(size >= capacity){
    _resize_tasks();  
  }

  _copy_task(&tasks[next_empty_index], task);
  next_empty_index++;
  size++;

  return TRUE;
}

/*****************************************************************************/
/*****************************************************************************/
BOOL add_tasks_by_file(char *filename){
  fopen(filename,READ);



  fclose(filename);

}

/*****************************************************************************/
/*! @fn BOOL remove_task()
    @brief Removes the last task from \a tasks
*/
/*****************************************************************************/
BOOL remove_task(){
  if(size <= 0){
    return FALSE;
  }

  memset(tasks[--next_empty_index],0,sizeof(TASK));

  return TRUE;
}

void sort_tasks_by_time(){
  const char *METHOD_NM = "sort_tasks_by_time: ";

  BOOL rc = quick_sort_tasks(&tasks[0], capacity);

  if(!rc){
    LOG_ERROR(METHOD_NM, "Reached max level in quick sort algorithm");
  }
}

/*****************************************************************************/
/*! @fn void set_id(int id, int new_id)
    @brief Sets the id of the task
    @param id Id of the task to be changed
    @param new_id New id of the task
*/
/*****************************************************************************/
void set_id_byid(int id, int new_id){
  int index = _get_task_by_id(id);
  tasks[index].id = new_id;
}

/*****************************************************************************/
/*! @fn void set_id(TASK *task, int new_id)
    @brief Sets the id of the task
    @param TASK *task Task to be updated with a new id
    @param new_id New id of the task
*/
/*****************************************************************************/
void set_id(TASK *task , int new_id){
  task->id = new_id;
}

/*****************************************************************************/
/*! @fn void set_interval_byid(int id, char *interval)
    @brief Sets the \a interval and \a _interval of the task
    @param int id Id of the task
    @param char *interval The interval in HH:MM:SS form
*/
/*****************************************************************************/
void set_interval_byid(int id, char *interval){
  if(interval != NULL){
    return;
  }

  int index = _get_task_by_id(id);
  strncpy(tasks[index].interval, interval, TIME_LEN);

  tasks[index]._interval = _get_time_from_string(interval);
}

/*****************************************************************************/
/*! @fn void set_interval(TASK *task, char *interval)
    @brief Sets the \a interval and \a _interval of the task
    @param TASK *task The task to be changed
    @param char *interval The interval in HH:MM:SS form
*/
/*****************************************************************************/
void set_interval_byid(TASK *task, char *interval){
  if(interval != NULL){
    return;
  }

  strncpy(task->interval, interval, TIME_LEN);

  tasks->_interval = _get_time_from_string(interval);
}

/*****************************************************************************/
/*! @fn void set_dead_thread_action_byid(int id, thread_action action)
    @brief Sets the dead_thread_action of the task
    @param int id Id of the task to be changed
    @param thread_action action Action to be changed (it's an enum)

    Options for thread_action are:
    - kill
    - replace
    - kill_program
*/
/*****************************************************************************/
void set_dead_thread_action_byid(int id, thread_action action){
  int index = _get_task_by_id(id);
  tasks[index].dead_thread_action = action;
}

/*****************************************************************************/
/*! @fn void set_dead_thread_action(TASK *task, thread_action action)
    @brief Sets the dead_thread_action of the task
    @param TASK *task Task to be changed
    @param thread_action action Action to be changed (it's an enum)

    Options for thread_action are:
    - kill
    - replace
    - kill_program
*/
/*****************************************************************************/
void set_dead_thread_action(TASK *task, thread_action action){
  task->dead_thread_action = action;
}

/*****************************************************************************/
/*! @fn void set_work_load_action_byid(int id, work_load_type type, work_load_action action)
    @brief Sets the action of a work load
    @param int id Id of the task to be changed
    @param work_load_type type Increase or decrease
    @param work_load_action action Action to be assinged to this work_load

    work_load_types are:
    - increase
    - decrease

    work_load_actions are:
    - add
    - sub
*/
/*****************************************************************************/
void set_work_load_action_byid(int id, work_load_type type, work_load_action action){
  int index = _get_task_by_id(id);
  if(type == increase){
    tasks[index].load_increase.action = action;
  } else if(type == decrease){
    tasks[index].load_decrease.action = action;
  }
}


/*****************************************************************************/
/*! @fn void set_work_load_action(TASK *task, work_load_type type, work_load_action action)
    @brief Sets the action of a work load
    @param TASK *task Task to be changed
    @param work_load_type type Increase or decrease
    @param work_load_action action Action to be assinged to this work_load

    work_load_types are:
    - increase
    - decrease

    work_load_actions are:
    - add
    - sub
*/
/*****************************************************************************/
void set_work_load_action(TASK *task, work_load_type type, work_load_action action){
  if(type == increase){
    task->load_increase.action = action;
  } else if(type == decrease){
    task->load_decrease.action = action;
  }
}

/*****************************************************************************/
/*! @fn void set_work_load_upper_bound_byid(int id, work_load_type type, int bound
    @brief Sets the upper bound of the work_load with an int
    @param int id Id of the task to be changed
    @param work_load_type type Increase or decrease
    @param int bound Bound to assingn the task

    work_load_types are:
    - increase
    - decrease

    valid bounds are any positive integer
*/
/*****************************************************************************/
void set_work_load_upper_bound_byid(int id, work_load_type type, int bound){
  if(bound <= 0){
    return;
  }

  int index = _get_task_by_id(id);
  if(type == increase){
    tasks[index].load_increase.upper_bound = bound;
  } else if(type == decrease){
    tasks[index].load_decrease.upper_bound = bound;
  }
}


/*****************************************************************************/
/*! @fn void set_work_load_upper_bound(TASK *task, work_load_type type, int bound
    @brief Sets the upper bound of the work_load with an int
    @param TASK *task Task to be changed
    @param work_load_type type Increase or decrease
    @param int bound Bound to assingn the task

    work_load_types are:
    - increase
    - decrease

    valid bounds are any positive integer
*/
/*****************************************************************************/
void set_work_load_upper_bound(TASK *task, work_load_type type, int bound){
  if(bound <= 0){
    return;
  }

  if(type == increase){
    task->load_increase.upper_bound = bound;
  } else if(type == decrease){
    tasks->load_decrease.upper_bound = bound;
  }
}

/*****************************************************************************/
/*! @fn void set_work_load_lower_bound_byid(int id, work_load_type type, int bound)
    @brief Sets the lower bound of the work_load with an int
    @param int id Id of the task to be changed
    @param work_load_type type Increase or decrease
    @param int bound Bound to assingn the task

    work_load_types are:
    - increase
    - decrease

    valid bounds are any negative integer
*/
/*****************************************************************************/
void set_work_load_lower_bound_byid(int id, work_load_type type, int bound){
  if(bound > 0){
    return;
  }

  int index = _get_task_by_id(id);
  if(type == increase){
    tasks[index].load_increase.lower_bound = bound;
  } else if(type == decrease){
    tasks[index].load_decrease.lower_bound = bound;
  }
}

/*****************************************************************************/
/*! @fn void set_work_load_lower_bound(TASK *task, work_load_type type, int bound)
    @brief Sets the lower bound of the work_load with an int
    @param TASK *task Task to be changed
    @param work_load_type type Increase or decrease
    @param int bound Bound to assingn the task

    work_load_types are:
    - increase
    - decrease

    valid bounds are any negative integer
*/
/*****************************************************************************/
void set_work_load_lower_bound(TASK *task, work_load_type type, int bound){
  if(bound > 0){
    return;
  }

  if(type == increase){
    task->load_increase.lower_bound = bound;
  } else if(type == decrease){
    task->load_decrease.lower_bound = bound;
  }
}

/*****************************************************************************/
/*! @fn void set_function_ptr_byid(int id, FUNC_PTR ptr)
    @brief Sets the function pointer to execute when this task is triggered
    @param int id Id of the task to be changed
    @param FUNC_PTR ptr Pointer to a function
*/
/*****************************************************************************/
void set_function_ptr_byid(int id, FUNC_PTR ptr){
  if(ptr == NULL){
    return;
  }

  int index = _get_task_by_id(id);
  tasks[index].function_ptr = ptr;
}

/*****************************************************************************/
/*! @fn void set_function_ptr(TASK *task, FUNC_PTR ptr)
    @brief Sets the function pointer to execute when this task is triggered
    @param TASK *task Task to be changed
    @param FUNC_PTR ptr Pointer to a function
*/
/*****************************************************************************/
void set_function_ptr_byid(TASK *task, FUNC_PTR ptr){
  if(ptr == NULL){
    return;
  }

  task->function_ptr = ptr;
}

/*****************************************************************************/
/*                        Private Functions                                  */
/*****************************************************************************/

/*****************************************************************************/
/*! @fn  _copy_task(TASK *dest_task, TASK *src_task)
    @brief Initailizes dest_task, then copies src_dest to dest_task
    @param TASK *dest_task destination task
    @param TASK *src_task source task

    A full memory copy from src to dest.
*/
/*****************************************************************************/
void _copy_task(TASK *dest_task, TASK *src_task){
  memset(dest_task,0,sizeof(TASK));
  memcpy(dest_task,src_task,sizeof(TASK));
}

/*****************************************************************************/
/*! @fn void _resize_tasks()
    @brief Resizes the array of tasks
*/
/*****************************************************************************/
void _resize_tasks(){

  int new_size = capacity + NUM_TASKS_INCREMENT;
  int i;
  TASK *t = (TASK *)malloc(sizeof(TASK)*new_size);

  /* copy tasks in original */
  for(i=0; i<capacity; i++){
    _copy_task(&t[i],&tasks[i]); 
  }

  FREE(tasks);
  tasks = t;

  capacity = new_size;

}

/*****************************************************************************/
/*! @fn int get_time_from_string(char *string)
    @brief Parses a string of the form HH:MM:SS and returns it in mins past midnight
    @param char *string Date to be parsed
 */
/*****************************************************************************/
int _get_time_from_string(char *string){
  int hour = 0;
  int min = 0;
  int sec = 0;
  char *sp = NULL;
  char *str = NULL;

  if(string == NULL || strlen(string) > TIME_LEN){
    return -1;
  }

  str=strtok_r(string,":",&sp);
  if(str) hour = atoi(str);

  str=strtok_r(NULL,":", &sp);
  if(str) min = atoi(str);

  str=strtok_r(NULL,":", &sp);
  if(str) sec = atoi(str);

  return ((hour*60)+min);
}

/*****************************************************************************/
/*! @fn int _get_task_by_id(int id)
    @brief Looks for given id in the \a tasks arraa
    @param int id Id to look for
*/
/*****************************************************************************/
int _get_task_by_id(int id){
  int i;
  for(i=0; i<size; i++){
    if(tasks[i]->id == id){
      return i;
    }
  }

  return -1;
}
