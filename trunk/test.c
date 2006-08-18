#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include "multi_thread.h"


void *print_me(){
	printf("PRINT ME!\n");
}

int main(){
	create_threads((void *)print_me, NULL, 4);
	join_threads(NULL);

}
