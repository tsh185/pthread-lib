#ifndef __THREAD_MANAGER__
#define __THREAD_MANAGER__


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


/* Public Methods */

void create_thread_manager(pthread_t *thread_pool, int num_threads);
int *check_threads(pthread_t *threads, int num_threads);
void read_config_file(char *filename);

print_thread_status(pthread_t *thread_pool, int num_threads);
void set_ping_signal(int signal);

#endif
