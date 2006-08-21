#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "gen_queue.h"

struct widget {
       int id;
       char *word;
};

void test_queue_add(){
  GEN_Q *queue = create_queue(4);
  
  struct widget widget1;
  widget1.id = 1;
  widget1.word = malloc(sizeof(char)*10);
  strcpy(widget1.word, "Nick");
  
  struct widget widget2;
  widget2.id = 2;
  widget2.word = malloc(sizeof(char)*10);
  strcpy(widget2.word,"Powers");
  
  
  
  add(queue, &widget1);
  add(queue, &widget2);
  add(queue, &widget1);
  
  print_actuals(queue);
  
  void *e1 = get_next(queue);
  void *e2 = get_next(queue);
  void *e3 = get_next(queue);
  
  struct widget *wid1 = (struct widget *)e1;
  struct widget *wid2 = (struct widget *)e2;
  struct widget *wid3 = (struct widget *)e3;
  
    printf("1 widget id = %d\n", wid1->id);
    printf("1 widget word = %s\n", wid1->word);
    printf("2 widget id = %d\n", wid2->id);
    printf("2 widget word = %s\n", wid2->word);
    printf("3 widget id = %d\n", wid3->id);
    printf("3 widget word = %s\n", wid3->word);

  

}

void test_queue_add_list(){
     GEN_Q *queue = create_queue(5);
     struct widget *wid = (struct widget *)malloc(sizeof(struct widget)*2);
     memset(wid, 0, sizeof(struct widget)*2);
     
     wid[0].id = 1;
     wid[0].word = malloc(sizeof(char)*10);
     strcpy(wid[0].word,"Nick");
     
     wid[1].id = 2;
     wid[1].word = malloc(sizeof(char)*10);
     strcpy(wid[1].word,"Powers");
     
     add_list(queue, wid, 2, sizeof(struct widget)*2);
     
     void *elem1 = get_next(queue);
     void *elem2 = get_next(queue);
     
     struct widget *wid1 = (struct widget *)elem1;
     struct widget *wid2 = (struct widget *)elem2;
     
     printf("1 widget id = %d\n", wid1->id);
     printf("1 widget word = %s\n", wid1->word);
     
     if(wid2 != NULL){
     printf("2 widget id = %d\n", wid2->id);
     printf("2 widget word = %s\n", wid2->word);
     }
     
     
          
}
int main(int argc, char **argv) {
test_queue_add_list();
  system("PAUSE");
    
  return 0;
}
