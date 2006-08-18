#ifndef __MULTI_THREAD_H__
#define __MULTI_THREAD_H__

/* Error Codes */
#define ERROR_CODE_MALLOC 1
#define ERROR_CODE_HAVE_WORK_MALLOC 2
#define ERROR_CODE_WRONG_HAVE_WORK_SIZE 3
#define ERROR_CODE_THREAD_CREATE_THREADS 4
#define ERROR_CODE_THREAD_CREATE_PARAMETER 5


/* Thread Operations */
void create_threads(void *(*func_ptr)(void *), void *parameter, int num_threads);
void join_threads(int *t_status);
void check_status(int status, char *api, char *msg);
int timed_wait(int wait_secs);
int timed_wait_milli(int wait_secs);
void destroy_all_mutexes();

void stop_threads();
int should_stop();


/* Queue Operations */
int get_queue_size();
void set_queue_size(int size);
void* get_queue_ptr();
void increment_queue_ptr(int pos);
void* next_queue_element();
int is_queue_available();
void set_queue_unavailable();
void set_queue_available();

struct function_ptrs_struct {
  void *(*term_func_ptr)();
  void *(*user1_func_ptr)();
  void *(*user2_func_ptr)();
};
typedef struct function_ptrs_struct FUNCTION_PTRS;

#endif
