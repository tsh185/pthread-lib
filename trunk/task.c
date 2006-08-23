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
#include <strings.h>
#include <errno.h>
#include <libgen.h>
#include "task.h"

/* Global Variables */
TASK_T *tasks = NULL;
int capacity = 0;
int next_empty_index = NULL;
int size;

/* Private Functions */
void _copy_tm(struct tm *dest_time, struct tm *src_time);
void _copy_task(TASK_T *dest_task, TASK_T *src_task);
void _resize_tasks();
int _get_time_from_string(char *string);
int _get_task_by_id(int id);

/*****************************************************************************/
/*! @fn void init_task(int num_tasks);
    @brief Initializes the tasks
    @param int num_tasks The number of tasks that can be added initally

    This function must be called before any tasks are added.
*/
/*****************************************************************************/
void init_tasks(int num_tasks){
  const char *METHOD_NM = "init_tasks: ";

  if(num_tasks <= 0){
    return;
  }

  tasks = (TASK_T *)malloc(sizeof(TASK_T)*num_tasks);
  CHECK_MALLOC(tasks,METHOD_NM,"malloc of tasks failed!");

  memset(tasks, 0, sizeof(TASK_T)*capacity);
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
  init_tasks(INITIAL_NUM_TASK_TS);
}

/*****************************************************************************/
/*! @fn TASK_T *get_init_task()
    @brief Returns a initialized task (using malloc)

    Creates a dynamic piece of memory holding an initalized TASK_T.
*/
/*****************************************************************************/
TASK_T *get_init_task(){
  const char *METHOD_NM = "get_init_task: ";

  TASK_T *task = (TASK_T *)malloc(sizeof(TASK_T));
  CHECK_MALLOC(task, METHOD_NM, "malloc of task failed!");

  memset(task, 0, sizeof(TASK_T));

  return task; 
}

/*****************************************************************************/
/*! @fn void destroy_task(TASK_T *task)
    @brief Frees the allocated memory

    Does a simple FREE task.
*/
/*****************************************************************************/
void destroy_task(TASK_T *task){
  FREE(task);
}

/*****************************************************************************/
/*! @fn void destroy_all_tasks()
    @brief Fress the \a tasks variable that holds all the tasks
*/
/*****************************************************************************/
void destroy_all_tasks(){
  destroy_task(tasks);
}

/*****************************************************************************/
/*! @fn BOOL add_task(TASK_T *task)
    @brief Copies the task from source into the next available spot
*/
/*****************************************************************************/
BOOL add_task(TASK_T *task){
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
/*! @fn char *add_tasks_by_file(char *filename)
    @brief Adds tasks to the thread manager's queue via a file.
    @param char *filename Fully qualified filename to open.

    See the documentation on how to create a task configuration file.
*/
/*****************************************************************************/
char *add_tasks_by_file(char *filename){
  char line[FILE_LINE_SIZE + 1];
  char *str = NULL;
  char *val = NULL;
  char *sp = NULL;
  const char *delim = "=\0";
  const char *delim2 = "# \t\r\n\0";
  int num_blank_lines = NUM_BLANK_LINES; 
  int i,j,k,l;
  char errors[NUM_ERROR_CHARS+1];
  char buf[BUFSIZ];
  int num_tasks = 0;
  int error_reading_task = FALSE;

  memset(errors, 0 , NUM_ERROR_CHARS+1);
  memset(buf, 0 , BUFSIZ);

  sprintf(errors, "Errors while parsing file %s:\n", filename);

  if(filename == NULL){
    STR_CAT(errors,"filename passed in is NULL.\n", sizeof(errors));
    strcat(errors,"filename passed in is NULL.\n");
    return errors;
  }

  FILE *task_file = NULL;
  task_file = fopen(filename, READ);

  if(task_file == NULL){
    STR_CAT(errors,"file can not be opened.\n", sizeof(errors));
    return errors;
  }

  while(!feof(config_file)){

    GET_LINE(task_file, line, FILE_LINE_SIZE, str, val, delim, delim2, &sp);
    if(strcasecmp(str,TASK) == 0){
      num_tasks++;
      TASK_T task;
      memset(&task, 0, sizeof(task));

      for(i=0; i<NUM_TASK_DEFINITIONS; i++){
        /* Interval */
        if(strcasecmp(str, INTERVAL) == 0){
          task.interval = val;  

          _validate_interval(task.interval);
        }


        /* thread action: action */
        else if(strcasecmp(str, THREAD_ACTION) == 0){
          int itr = num_blank_lines;
          /* Specific fields */
          for(j=0; j<NUM_THREAD_ACTION_DEFINTIONS; j++){
            if(strcasecmp(str, ACTION) == 0){
              /* actions: kill_it, replace, kill_program */
              if(strcasecmp(val, KILL) == 0){
                task.dead_thread_action = kill_it;
              }
              else if(strcasecmp(val, REPLACE) == 0){
                task.dead_thread_action = replace;
              }
              else if(strcasecmp(val, KILL_PROGRAM) == 0){
                task.dead_thread_action = kill_program; 
              } else {
                sprintf(buf,"Expected action value next to action tag. Got [%s] instead.\n", val);
                STR_CAT(errors,buf,sizeof(errors)); 
                memset(buf,0,sizeof(buf));
              }
            }
            else {
              sprintf(buf,"Expected action tag under thread_action tag. Got [%s] instead.\n",val);
              STR_CAT(errors,buf,sizeof(errors)); 
              memset(buf,0,sizeof(buf));
            }
          }/* end for (j) loop */

        _validate_thread_action(task.dead_thread_action);

        }/* end if THREAD_ACTION */


        /* work load: type, bounds, action, value */
        else if(strcasecmp(str, WORK_LOAD) == 0){

          WORK_LOAD work;
          for(k=0; k<NUM_WORK_LOAD_DEFINITIONS; k++){
            /* types: increase, decrease */
            if(strcasecmp(str, TYPE) == 0){
              if(strcasecmp(val, INCREASE) == 0){
                work.type=increase;
              }
              else if(strcasecmp(val, DECREASE) == 0){
                work.type=decrease;
              } 
              else {
                sprintf(buf,"Expected a value next to the type tag. Got [%s] instead\n", val);  
                STR_CAT(errors,buf,sizeof(errors));
                memset(buf,0,sizeof(buf));
              }

            }
            /* bounds: any integer */
            else if(strcasecmp(str, BOUNDS) == 0){
              if(is_digit(str)){
                work.bounds = taskatoi(val);
              } else {
                sprintf(buf,"Expected an integer next to the bounds tag. Got [%s] instead\n", val);
                STR_CAT(errors,buf,sizeof(errors));
                memset(buf,0,sizeof(buf));
              }
            }
            /* work load action: add, sub */ 
            else if(strcasecmp(str, WORK_LOAD_ACTION) == 0){
              if(strcasecmp(val, ADD) == 0 ||
                 strcasecmp(val, ADD_LONG) == 0){
                work.action = add;
              } 
              else if(strcasecmp(val, SUB) == 0 ||
                      strcasecmp(val, SUB_LONG) == 0){
                work.action = sub;
              } 
              else {
                sprintf(buf,"Expected a value next to the type tag. Got [%s] instead\n",val);
                STR_CAT(errors,buf,sizeof(errors));
                memset(buf,0,sizof(buf));
              }
            }
            /* value: any integer */
            else if(strcasecmp(str, VALUE) == 0){
              if(is_digit(val)){
                work.value = atoi(val); 
              } 
              else {
                sprintf(buf,"Expected an integer next to the value tag. Got [%s] instead,\n",val);
                STR_CAT(errors,buf,sizeof(errors));
                memset(buf,0,sizeof(buf));
              }
            } else {
              sprintf(buf,"Expected a tag under work_load tag. Got [%s] instead.\n",str);
              STR_CAT(errors,buf,sizeof(errors));
              memset(buf,0,sizeof(buf));
            }
          }/* end for work_load loop */

        _validate_work_load(&work);
        _copy_work_to_task(&task, &work);

        }/* end if work_load tag */


        /* schedule: start, repeat */
        else if(strcasecmp(str, SCHEDULE) == 0){
          SCHEDULE sched;

          for(l=0; l<NUM_SCHEDULE_DEFINITIONS; l++){
            if(strcasecmp(str,START) == 0){
              sched.start_char = val;     
            }
            else if(strcasecmp(str, REPEAT) == 0){
              if(strcasecmp(val,"true") == 0){
                sched.repeat = TRUE;
              }
              else if(strcasecmp(val,"false") == 0){
                sched.repeat = FALSE;
              }
              else {
              sprintf(buf,"Expected a value of true or false next to repeat tag. Got [%s] instead.\n",val);
              STR_CAT(errors,buf,sizeof(errors));
              memset(buf,0,sizeof(buf));
              }
            } 
            else {
              sprintf(buf,"Expected a tag under schedule tag. Got [%s] instead.\n",str);
              STR_CAT(errors,buf,sizeof(errors));
              memset(buf,0,sizeof(buf));
            }

          }/* end for schedule */

          _validate_schedule(&sched);
          _copy_sched_to_task(&task, &sched);

        }/* end if schedule tag */ 
        else {
          sprintf(buf,"tag [%s] is unrecognized under the task tag\n", str);
          STR_CAT(errors, buf, sizeof(errors-1));
          memset(buf,0,sizeof(buf));
        }

      }/* end for loop - task tag */
    }/* end if task tag */

    /* add the task to the queue of tasks */
    if(!error_reading_task){
      add_task(&task);
#ifdef DEBUG
     printf("Added task number %d\n",num_tasks);
#endif
    } else {
      sprintf(buf,"Not able to add task number %d. See errors for more information\n",num_tasks);
      STR_CAT(errors,buf,sizeof(errors));
      memset(buf,0,sizeof(buf));
    }
  }/* end while not end of file */


  if(errors[1] != '\0'){
     sprintf(buf, "Errors while parsing file %s\n", filename);
     STR_CAT(errors, buf, sizeof(errors));
     memset(buf,0,sizeof(buf));
  }

  /* close the file */
  if(task_file){
    fclose(task_file);
  }

  return TRUE;
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

  memset(&tasks[--next_empty_index],0,sizeof(TASK_T));

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
/*! @fn void set_id(TASK_T *task, int new_id)
    @brief Sets the id of the task
    @param TASK_T *task Task to be updated with a new id
    @param new_id New id of the task
*/
/*****************************************************************************/
void set_id(TASK_T *task , int new_id){
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
/*! @fn void set_interval(TASK_T *task, char *interval)
    @brief Sets the \a interval and \a _interval of the task
    @param TASK_T *task The task to be changed
    @param char *interval The interval in HH:MM:SS form
*/
/*****************************************************************************/
void set_interval(TASK_T *task, char *interval){
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
/*! @fn void set_dead_thread_action(TASK_T *task, thread_action action)
    @brief Sets the dead_thread_action of the task
    @param TASK_T *task Task to be changed
    @param thread_action action Action to be changed (it's an enum)

    Options for thread_action are:
    - kill
    - replace
    - kill_program
*/
/*****************************************************************************/
void set_dead_thread_action(TASK_T *task, thread_action action){
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
/*! @fn void set_work_load_action(TASK_T *task, work_load_type type, work_load_action action)
    @brief Sets the action of a work load
    @param TASK_T *task Task to be changed
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
void set_work_load_action(TASK_T *task, work_load_type type, work_load_action action){
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
/*! @fn void set_work_load_upper_bound(TASK_T *task, work_load_type type, int bound
    @brief Sets the upper bound of the work_load with an int
    @param TASK_T *task Task to be changed
    @param work_load_type type Increase or decrease
    @param int bound Bound to assingn the task

    work_load_types are:
    - increase
    - decrease

    valid bounds are any positive integer
*/
/*****************************************************************************/
void set_work_load_upper_bound(TASK_T *task, work_load_type type, int bound){
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
/*! @fn void set_work_load_lower_bound(TASK_T *task, work_load_type type, int bound)
    @brief Sets the lower bound of the work_load with an int
    @param TASK_T *task Task to be changed
    @param work_load_type type Increase or decrease
    @param int bound Bound to assingn the task

    work_load_types are:
    - increase
    - decrease

    valid bounds are any negative integer
*/
/*****************************************************************************/
void set_work_load_lower_bound(TASK_T *task, work_load_type type, int bound){
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
/*! @fn void set_work_load_value_byid(int id, work_load_type, int value)
    @brief Sets the value part of the work_load
    @param int id Id of the task to be changed
    @param work_load_type type Increase or decrease
    @param int value Value to increase or decrease the threads

    Value must be a positive integer.

*/
/*****************************************************************************/
void set_work_load_value_byid(int id, work_load_type type, int value){
  if(value <= 0){
    return;
  }

  int index = _get_task_by_id(id);
  if(type == increase){
    tasks[index].load_increase.value = value;
  } else if(type == decrease){
    tasks[index].load_decrease.value = value;
  }
}

/*****************************************************************************/
/*! @fn void set_work_load_value(TASK_T *task, work_load_type type, int value)
    @brief Sets the value part of the work_load
    @param TASK_T *task Task to be changed
    @param work_load_type type Increase or decrease
    @param int value Value to increase or decrease the threads

    Value must be a positive integer.

*/
/*****************************************************************************/
void set_work_load_value(TASK_T *task, work_load_type type, int value){
  if(value <= 0){
    return;
  }

  if(type == increase){
    task->load_increase.value = value;
  } else if(type == decrease){
    task->load_decrease.value = value;
  }
}

/*****************************************************************************/
/*! @fn void set_function_ptr_byid(int id, FUNC_PTR ptr)
    @brief Sets the function pointer to execute when this task is triggered
    @param int id Id of the task to be changed
    @param FUNC_PTR ptr Pointer to a function
*/
/*****************************************************************************/
void set_function_ptr_byid(int id, void *(*ptr)() ){
  if(ptr == NULL){
    return;
  }

  int index = _get_task_by_id(id);
  tasks[index].function_ptr = ptr;
}

/*****************************************************************************/
/*! @fn void set_function_ptr(TASK_T *task, FUNC_PTR ptr)
    @brief Sets the function pointer to execute when this task is triggered
    @param TASK_T *task Task to be changed
    @param FUNC_PTR ptr Pointer to a function
*/
/*****************************************************************************/
void set_function_ptr(TASK_T *task, void *(*ptr)() ){
  if(ptr == NULL){
    return;
  }

  task->function_ptr = ptr;
}

/*****************************************************************************/
/*! @fn void set_schedule_byid(int id, struct tm *time, BOOL repeat)
    @brief Sets the schedule of the task
    @param int id Id of the task to be changed
    @param struct tm *time Time that the task will start
    @param BOOL repeat Will task repeat?

*/
/*****************************************************************************/
void set_schedule_byid(int id, struct tm *time, BOOL repeat){

  int index = _get_task_by_id(id);
  _copy_tm(&(tasks[index].schedule.start), time);
  tasks[index].schedule.repeat = repeat;
}

/*****************************************************************************/
/*! @fn void set_schedule(TASK_T *task, struct tm *time, BOOL repeat)
    @brief Sets the schedule of the task
    @param TASK_T *task Task to be changed
    @param struct tm *time Time the task will start
    @param BOOL repeat Will task repeat?

*/
/*****************************************************************************/
void set_schedule(TASK_T *task, struct tm *time, BOOL repeat){
  _copy_tm(&(task->schedule.start),time);
  task->schedule.repeat = repeat;
}

/*****************************************************************************/
/*! @fn int get_size()
    @brief Returns the size of the \a tasks
*/
/*****************************************************************************/
int get_size(){
  return size;
}

/*****************************************************************************/
/*! @fn int get_capacity()
    @brief Returns the capacity of the \a tasks
*/
/*****************************************************************************/
int get_capacity(){
  return capacity;
}

/*****************************************************************************/
/*                        Private Functions                                  */
/*****************************************************************************/

/*****************************************************************************/
/*! @fn  _copy_task(TASK_T *dest_task, TASK_T *src_task)
    @brief Initailizes dest_task, then copies src_dest to dest_task
    @param TASK_T *dest_task Destination task
    @param TASK_T *src_task  Source task

    A full memory copy from src to dest.
*/
/*****************************************************************************/
void _copy_task(TASK_T *dest_task, TASK_T *src_task){
  memset(dest_task,0,sizeof(TASK_T));
  memcpy(dest_task,src_task,sizeof(TASK_T));
}

/*****************************************************************************/
/*! @fn void _copy_tm(struct tm *dest_time, struct tm *src_time)
    @brief Initializes dest_time, then copies src_time to dest_time
    @param struct tm *dest_time Destination time
    @param struct tm *src_time  Source time

    A full memory copy from src to dest
*/
/*****************************************************************************/
void _copy_tm(struct tm *dest_time, struct tm *src_time){
  memset(dest_time,0,sizeof(struct tm));
  memcpy(dest_time,src_time,sizeof(struct tm));
}

/*****************************************************************************/
/*! @fn void _resize_tasks()
    @brief Resizes the array of tasks
*/
/*****************************************************************************/
void _resize_tasks(){

  int new_size = capacity + NUM_TASK_TS_INCREMENT;
  int i;
  TASK_T *t = (TASK_T *)malloc(sizeof(TASK_T)*new_size);

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
  const char *delim = ":";


  if(string == NULL || strlen(string) > TIME_LEN){
    return -1;
  }

  str = strtok_r(string, delim, &sp);
  if(str) hour = atoi(str);

  str = strtok_r(NULL, delim, &sp);
  if(str) min = atoi(str);

  str = strtok_r(NULL, delim, &sp);
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
    if(tasks[i].id == id){
      return i;
    }
  }

  return -1;
}

/*****************************************************************************/
/*!

*/
/*****************************************************************************/
int _handle_config_element(char *tag, char *value, char *source, char **sp){
  if(!tag){
    return;
  }

  /* found a task element */
  if(strcasecmp(str,TASK) == 0){

  }

}

/*****************************************************************************/
/*! @fn int quick_sort_tasks(TASK_T *arr, int elements)
    @brief Quick sort function for tasks
    @param TASK_T arr An array of tasks
    @param int elements Number of elements to be sorted

  This code was taken from the Internet and was not created by myself.

  This public-domain C implementation by Darel R. Finley.

  - Returns true if sort was successful, or false if the nested
    pivots went too deep, in which case your array will have
    been re-ordered, but probably not sorted correctly.

  - This function assumes it is called with valid parameters.

  - Example calls:
    quickSort(&myArray[0],5); // sorts elements 0, 1, 2, 3, and 4
    quickSort(&myArray[3],5); // sorts elements 3, 4, 5, 6, and 7

  - This function was modified by Nick Powers on 2006/08/22.
*/
/*****************************************************************************/
int quick_sort_tasks(TASK_T *arr, int elements) {

  int  piv, beg[Q_SORT_MAX_LEVELS], end[Q_SORT_MAX_LEVELS], i=0, L, R;

  beg[0]=0; end[0]=elements;
  while (i>=0) {
    L=beg[i];
    R=end[i]-1;

    if (L<R) {
      piv=arr[L]._interval;
      if (i==Q_SORT_MAX_LEVELS-1) return FALSE;
      while (L<R) {
        while (arr[R]._interval >=piv && L<R) R--; if (L<R) arr[L++]=arr[R];
        while (arr[L]._interval <=piv && L<R) L++; if (L<R) arr[R--]=arr[L];
      }/* end while */
      arr[L]._interval=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L;
    }
    else {
      i--;
    }/* end else */
  }/* end while */
  return TRUE;
}

