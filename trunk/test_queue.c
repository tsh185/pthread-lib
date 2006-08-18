#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include "gen_queue.h"


void test_queue(){
  GEN_Q *queue = create_queue(4);
  print_actuals(queue);

  printf("Going to print queue 1st time\n");
  print_queue(queue);

  printf("Adding elements\n");
  int result = add(queue,"Nick");
  if(!result){printf("Not able to add to queue\n");}

  result = add(queue,"Powers");
  if(!result){printf("Not able to add to queue\n");}


  printf("Going to print queue second time\n");
  print_actuals(queue);


  result = add(queue,"My 3");
  if(!result){printf("Not able to add to queue\n");}
  print_actuals(queue);

  result = add(queue,"My 4");
  if(!result){printf("Not able to add to queue\n");}
  print_actuals(queue);

  get_next(queue);

  result = add(queue,"My 5");
  if(!result){printf("Not able to add to queue\n");}
  print_actuals(queue);

  print_queue(queue);
}

int main(int argc, char **argv) {
  test_queue();
}
