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
#include "util.h"

/* Global Variables */
TASK_T *tasks = NULL;
int capacity = 0;
int next_empty_index = NULL;
int size;

/* Private Functions */
void _copy_task(TASK_T *dest_task, TASK_T *src_task);
void _copy_tm(struct tm *dest_time, struct tm *src_time);
void _resize_tasks();
long _get_time_from_string(char *string);
int  _get_task_by_id(int id);
void _copy_work_to_task(WORK_LOAD *work, TASK_T *task, work_load_type type );
void _copy_sched_to_task(SCHEDULE *shed, TASK_T *task);
int  _validate_schedule(SCHEDULE *sched);
int  _validate_interval(char *interval);
int  _validate_thread_action(thread_action ta);
int  _validate_work_load(WORK_LOAD *work);
void print_task(TASK_T *task);
void print_tm(struct tm *t);

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
/*! @fn int add_task(TASK_T *task)
    @brief Copies the task from source into the next available spot
*/
/*****************************************************************************/
int add_task(TASK_T *task){
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
int add_tasks_by_file(char *filename){
  const char *METHOD_NM = "add_tasks_by_file: ";
  int rc = 0;
  scew_tree* tree = NULL;
  scew_parser* parser = NULL;
  char buf[512];

  memset(buf,0,sizeof(buf));

  if(filename == NULL){
      return FALSE;
  }

  parser = scew_parser_create();
  scew_parser_ignore_whitespaces(parser, 1);
  rc = scew_parser_load_file(parser, filename)
  if (!rc){
    scew_error code = scew_error_code();
    LOG_ERROR(METHOD_NM,"Unable to load file");
    return FALSE;
  }

  tree = scew_parser_tree(parser);

  parse_xml(scew_tree_root(tree), 0);

  scew_tree_free(tree);
  scew_parser_free(parser);

}

/*****************************************************************************/
/*****************************************************************************/
void parse_xml(scew_element* element, unsigned int indent){
  const char *METHOD_NM = "parse_xml(): ";

  if (element == NULL){
      return;
  }

  scew_element* child = NULL;

  XML_Char *name = scew_element_name(element);
  XML_Char *value = NULL;
  XML_Char *att_name = NULL;
  XML_Char *att_value = NULL;


  if(!strcasecmp(element,TOP_LEVEL_NAME) == 0){
    LOG_ERROR("%s This is not a properly formatted configuration file.\n",METHOD_NM);
    LOG_ERROR("%s Exptected [%s], got [%s]\n",METHOD_NM, TOP_LEVEL_NAME, name);
    return;
  }


  TASK_T task;
  int task_id = 0;
  memset(&task,0,sizeof(task));
  hasTask = FALSE;

  while ((child = scew_element_next(element, child)) != NULL){

    name = scew_element_name(child);
    value = scew_element_contents(child);

      if(strcasecmp(name,TASK) == 0){

        /* add task before we create a new task */
        if(hasTask){
          add_task(&task);
          memset(&task,0,sizeof(task));
        }

        hasTask = TRUE;
        task_id++;

      } else if(strcasecmp(name, INTERVAL) == 0){
          strncpy(task.interval, value, TIME_LEN);
          task._interval =
            _validate_interval(task.interval);

      } else if(strcasecmp(name, THREAD_ACTION) == 0){
        while ((attribute = scew_attribute_next(element, attribute)) != NULL){
          att_name = scew_attribute_name(attribute);
          att_value = scew_attribute_value(attribute);

          if(att_name != NULL){
            if(strcasecmp(att_name, ACTION) == 0){
              if(strcasecmp(value, KILL){
                task.dead_thread_action = kill_it;
              } else if(strcasecmp(value, REPLACE) == 0){
                task.dead_thread_action = replace;
              } else if(strcasecmp(value, KILL_PROGRAM) == 0){
                task.dead_thread_action = kill_program;
              } else {
                LOG_ERROR("%s unexpected attribute [%s] under tag [%s].\n",METHOD_NM,value,att_name);
              }
            }/* end if attribute == ACTION */
        }/* end if att_name != NULL */

      } /*else if(strcasecmp(str, ACTION) == 0){

      } else if(strcasecmp(val, KILL) == 0){

      } else if(strcasecmp(val, REPLACE) == 0){

      } else if(strcasecmp(val, KILL_PROGRAM) == 0){

      } else if(strcasecmp(str, WORK_LOAD_CHAR) == 0){

      } else if(strcasecmp(str, TYPE) == 0){

      } else if(strcasecmp(val, INCREASE) == 0){

      } else if(strcasecmp(val, DECREASE) == 0){

      } else if(strcasecmp(str, UPPER_BOUND) == 0){

      } else if(strcasecmp(str,LOWER_BOUND) == 0){

      } else if(strcasecmp(str, ACTION) == 0){

      } else if(strcasecmp(val, ADD) == 0){

      } else if(strcasecmp(val, SUB) == 0){

      } else if(strcasecmp(str, VALUE) == 0){

      } else if(strcasecmp(str, SCHEDULE_CHAR) == 0){

      } else if(strcasecmp(str,START) == 0){

      } else if(strcasecmp(str, REPEAT) == 0){

      } */
        else {
        LOG_ERROR("%s Unrecognized element [%s]\n",METHOD_NM, name);
      }

  } /* end while */


}
/*****************************************************************************/
/*! @fn int remove_task()
    @brief Removes the last task from \a tasks
*/
/*****************************************************************************/
int remove_task(){
  if(size <= 0){
    return FALSE;
  }

  memset(&tasks[--next_empty_index],0,sizeof(TASK_T));
  size--;

  return TRUE;
}

void sort_tasks_by_time(){
  const char *METHOD_NM = "sort_tasks_by_time: ";

  int rc = quick_sort_tasks(&tasks[0], capacity);

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
/*! @fn void set_schedule_byid(int id, struct tm *time, int repeat)
    @brief Sets the schedule of the task
    @param int id Id of the task to be changed
    @param struct tm *time Time that the task will start
    @param int repeat Will task repeat?

*/
/*****************************************************************************/
void set_schedule_byid(int id, struct tm *time, int repeat){

  int index = _get_task_by_id(id);
  _copy_tm(&(tasks[index].schedule.start), time);
  tasks[index].schedule.repeat = repeat;
}

/*****************************************************************************/
/*! @fn void set_schedule(TASK_T *task, struct tm *time, int repeat)
    @brief Sets the schedule of the task
    @param TASK_T *task Task to be changed
    @param struct tm *time Time the task will start
    @param int repeat Will task repeat?

*/
/*****************************************************************************/
void set_schedule(TASK_T *task, struct tm *time, int repeat){
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
    This is a private function.
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

    A full memory copy from src to dest.
    This is a private function.
*/
/*****************************************************************************/
void _copy_tm(struct tm *dest_time, struct tm *src_time){
  memset(dest_time,0,sizeof(struct tm));
  memcpy(dest_time,src_time,sizeof(struct tm));
}

/*****************************************************************************/
/*! @fn void _resize_tasks()
    @brief Resizes the array of tasks

    This is a private function.
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

    This is a private function.
 */
/*****************************************************************************/
long _get_time_from_string(char *string){
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

  return (long)(hour*120)+(min*60)+(sec);
}

/*****************************************************************************/
/*! @fn int _get_task_by_id(int id)
    @brief Looks for given id in the \a tasks arraa
    @param int id Id to look for

    This is a private function.
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
/*! @fn void _copy_work_to_task(WORK_LOAD *work, TASK_T *task, work_load_type type )
    @breif Copies a work struct to the task struct
    @param WORK_LOAD *work Work load to be copied
    @param TASK_T *task Task for the work load to be cpied
    @param work_load_type type Increase or decrease

    This is a private function.

*/
/*****************************************************************************/
void _copy_work_to_task(WORK_LOAD *work, TASK_T *task, work_load_type type ){
  if(work == NULL || task == NULL){
    return;
  }

  if(type == increase){
    memcpy(&(task->load_increase), work, sizeof(WORK_LOAD));
  }
  else if(type == decrease){
    memcpy(&(task->load_decrease), work, sizeof(WORK_LOAD));
  }

  return;
}

/*****************************************************************************/
/*! @fn void _copy_sched_to_task(SCHEDULE *shed, TASK *task)
    @brief Copy the schedule to the task
    @param SCHEDULE *sched Schedule to be copied
    @param TASK_T *task Task the schedule will be copied to.

    This is a private function.
*/
/*****************************************************************************/
void _copy_sched_to_task(SCHEDULE *sched, TASK_T *task){
  if(sched == NULL || task == NULL){
    return;
  }

  memcpy(&(task->schedule), sched, sizeof(SCHEDULE));

  return;
}

/*****************************************************************************/
/*! @fn int _validate_schedule(TASK_T *task, SCHEDULE *sched)
    @brief Validates the schedule element and assings the start_char to start
    @param SCHEDULE *sched Schedule to be validated

    This is a private function.
*/
/*****************************************************************************/
int _validate_schedule(SCHEDULE *sched){
  char *sp = NULL;
  char *date = NULL;
  char *t = NULL;
  const char *delim = " ";
  const char *delim2 = "-";
  const char *delim3 = ":";
  char *string = NULL;
  char *year = NULL;
  char *month = NULL;
  char *day = NULL;
  char *hour = NULL;
  char *min = NULL;
  char *sec = NULL;
  int rc = TRUE;

  if(sched == NULL){
    return FALSE;
  }

  if(sched->start_char[0] == '\0'){
    return FALSE;
  }

  string = sched->start_char;

  /* split the date and time up. Should be seperated by a space */
  date = strtok_r(string, delim, &sp);
  t = strtok_r(NULL, delim, &sp);

  /* if date or time is null, this is not valid */
  if(date == NULL || t == NULL){
    return FALSE;
  }



  /* get the date */
  year = strtok_r(date, delim2, &sp);
  if(is_digit(year)) { sched->start.tm_year = atoi(year); } else { rc = FALSE; }

  month = strtok_r(NULL, delim2, &sp);
  if(is_digit(month)) { sched->start.tm_mon = atoi(month); } else { rc = FALSE; }

  day = strtok_r(NULL, delim2, &sp);
  if(is_digit(day)) { sched->start.tm_mday = atoi(day); } else { rc = FALSE; }

  /* get the time */
  hour = strtok_r(t, delim3, &sp);
  if(is_digit(hour)) { sched->start.tm_hour = atoi(hour); } else { rc = FALSE; }

  min = strtok_r(NULL, delim3, &sp);
  if(is_digit(min)) { sched->start.tm_min = atoi(min); } else { rc = FALSE; }

  sec = strtok_r(NULL, delim3, &sp);
  if(is_digit(sec)) { sched->start.tm_sec = atoi(sec); } else { rc = FALSE; }

  /* validate repeat */
  if(sched->repeat != FALSE || sched->repeat != TRUE){
    rc = FALSE;
  }

  return rc;
}

/*****************************************************************************/
/*! @fn int _validate_interval(TASK_T *task, char *interval)
    @brief Validats the interval given
    @param TASK_T *task Task to be assigned the interval in long form
    @param char *interval String of chars to be validated

    Returns true if valid, false otherwise.
    If task is not null, then \a _interval
    is assigned that value in seconds.  Otherwise, task is not used.

    This is a private function.

*/
/*****************************************************************************/
int _validate_interval(char *interval){
  if(interval == NULL){
    return FALSE;
  }

  long t =_get_time_from_string(interval);

  if(t < 0){
    return FALSE;
  }

  return TRUE;
}

/*****************************************************************************/
/*! @fn int _validate_thread_action(thread_action ta)
    @brief Validates the thread action
    @param thread_action ta Thread action to be validated

    Returns true if valid, false otherwise.
    This is a private functions.
*/
/*****************************************************************************/
int _validate_thread_action(thread_action ta){
  if(ta == kill_it || ta == replace || ta == kill_program){
    return TRUE;
  }

  return FALSE;
}

/*****************************************************************************/
/*! @fn int _validate_work_load(WORK_LOAD *work)
    @brief Validates the work load
    @param WORK *work Work load to be Validated.

    Returns true if valid, false otherwise.
    This is a private function.
*/
/*****************************************************************************/
int _validate_work_load(WORK_LOAD *work){
  int rc = TRUE;

  if(work == NULL){
    return FALSE;
  }

  if(work->value <= 0 || work->upper_bound <= 0 || work->lower_bound <= 0){
    rc = FALSE;
  }

  return rc;
}

void print_task(TASK_T *task){
  if(task == NULL){
    return;
  }

  const char *METHOD_NM = "print_task: ";

  printf("==========================================================\n");
  printf("id [%d]\n",task->id);
  printf("interval [%s]\n",task->interval);
  printf("_interval [%d]\n",task->_interval);
  printf("dead_thread_action [%d]\n", task->dead_thread_action);

  printf("load_increase\n");
  printf("\ttype [%d]\n",task->load_increase.type);
  printf("\taction [%d]\n",task->load_increase.action);
  printf("\tupper_bound [%d]\n", task->load_increase.upper_bound);
  printf("\tlower_bound [%d]\n", task->load_increase.lower_bound);
  printf("\tvalue [%d]\n", task->load_increase.value);

  printf("load_decrease\n");
  printf("\ttype [%d]\n",task->load_decrease.type);
  printf("\taction [%d]\n",task->load_decrease.action);
  printf("\tupper_bound [%d]\n", task->load_decrease.upper_bound);
  printf("\tlower_bound [%d]\n", task->load_decrease.lower_bound);
  printf("\tvalue [%d]\n", task->load_decrease.value);

  printf("Function Pointer null? [%d]\n",(task->function_ptr == NULL));

  printf("schedule\n");
  printf("\tstart_char [%s]\n", task->schedule.start_char);
  printf("\tstart\n");
  print_tm(&(task->schedule.start));
  printf("\trepeat [%d]\n",task->schedule.repeat);
  printf("==========================================================\n");
}

void print_tm(struct tm *t){
  printf("\t\tsec [%d]\n",t->tm_sec);
  printf("\t\tmin [%d]\n",t->tm_min);
  printf("\t\thour [%d]\n", t->tm_hour);
  printf("\t\tday [%d]\n",t->tm_mday);
  printf("\t\tmonth [%d]\n",t->tm_mon);
  printf("\t\tyear [%d]\n",t->tm_year);
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

