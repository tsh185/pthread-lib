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
#include "multi_thread.h"


void *print_me(void *e){
  PARAMETER *p = (PARAMETER *)e;
  int id = p->id;

  printf("My id [%d]\n",id);

  while(!should_stop_all()){
    timed_wait(1);
    printf("I'm Running 2!\n");
  }

  printf("done with print_me\n");
}

void *print_me_pool(void *e){
  PARAMETER *p = (PARAMETER *)e;
  int id = p->id;

  printf("My id [%d]\n",id);

  while(!should_stop_all()){
	  printf("Running!\n");
	  timed_wait(1);
  }

  printf("done with print_me_pool\n");
}


int main(){
	int id = create_thread_pool((void *)print_me_pool,NULL, 5);
	int id2 = create_thread_pool((void *)print_me,NULL, 10);

	timed_wait(10);
	stop_all_pools();
	join_threads(id);
	join_threads(id2);

        printf("DONE\n");	
}

