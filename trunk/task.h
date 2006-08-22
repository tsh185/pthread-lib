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

#ifndef __TASK_H__
#define __TASK_H__

#define INITIAL_NUM_TASKS 5
#define NUM_TASKS_INCREMENT 5
#define TIME_LEN 8

/* Configuration file names */
#define TASK "task"

#define INTERVAL "interval" /* any time in HH24:MM:SS */

#define THREAD_ACTION "thread.action"
#define KILL "kill.thread"
#define REPLACE "replace"
#define KILL_PROGRAM "kill.program"

#define WORK_LOAD_INCREASE "work.load.increase"
#define WORK_LOAD_DECREASE "work.load.decrease"
#define ADD "add"
#define SUB "sub"
#define VALUE "value"

#define ACTION "action"
#define BOUNDS "bounds"
#define GREATER ">"
#define LESS "<"

#define INTERNAL_CONFIG_FILENAME "thread_manager.config"
#define READ "r"
#define FILE_LINE_SIZE 180

/* enums */
typedef enum {kill=1, replace, kill_program} thread_action;
typedef enum {increase=1,decrease} work_load_type;
typedef enum {add=1, sub} work_load_action;


/* structs */
struct work_load {
  work_load_type   type;
  work_load_action action;
  int  upper_bound;
  int  lower_bound;
  int  value;
};

struct task {
  int id;
  char interval[TIME_LEN + 1];
  int _interval;

  thread_action dead_thread_action;
  struct work_load load_increase;
  struct work_load load_decrease;
  FUNC_PTR function_ptr;
};

typedef struct task TASK;
typedef struct work_load WORK_LOAD;

/* Public Methods */
void init_tasks(int num_tasks);
void init_tasks_default();
BOOL add_task(TASK *task);
BOOL add_tasks_by_file(char *filename);
BOOL remove_task();
void set_id_byid(int id, int new_id);
void set_id(TASK *task , int new_id);
void set_interval_byid(int id, char *interval);
void set_interval_byid(TASK *task, char *interval);
void set_dead_thread_action_byid(int id, thread_action action);
void set_dead_thread_action(TASK *task, thread_action action);
void set_work_load_action_byid(int id, work_load_type type, work_load_action action);
void set_work_load_action(TASK *task, work_load_type type, work_load_action action);
void set_work_load_upper_bound_byid(int id, work_load_type type, int bound);
void set_work_load_upper_bound(TASK *task, work_load_type type, int bound);
void set_work_load_lower_bound_byid(int id, work_load_type type, int bound);
void set_work_load_lower_bound(TASK *task, work_load_type type, int bound);
void set_function_ptr_byid(int id, FUNC_PTR ptr);
void set_function_ptr_byid(TASK *task, FUNC_PTR ptr);


#endif
