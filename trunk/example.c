#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include "multi_thread.h"

void *stop(){
  stop_threads();
}

void *print_report(){
  printf("Report 1\n");
}

void *do_something(){

  /* have this thread block all the signals */
  block_all_signals();

  while(!should_stop()){
    timed_wait(5);
  }
}

int main(){
  /* assign the stop function to the signal handler */
  FUNCTION_PTRS f_ptrs;
  f_ptrs.term_func_ptr = stop;
  f_ptrs.user1_func_ptr = print_report;

  /* create the signal handler */
  signal_handler_create(&f_ptrs);

  create_threads(do_something, NULL, 5);

  while(!should_stop()){
    timed_wait(2);
  }

  join_threads(NULL);

}
