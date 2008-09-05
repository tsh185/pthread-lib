/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Nick Powers 2008 <>
 * 
 * main.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * main.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h> 
#include <stdio.h>
#include <assert.h>
#include "ptl_queue.h"
#include "ptl_linked_queue.h"
#include "ptl_linked_queue_test.h"

void *print_me();
int *new_int(int in_i);

int main(int argc, char **argv)
{
	void *print_me();
	void* q = (void *)malloc(5*sizeof(void*));
	void* ptr = q;
	int *  i = new_int(4);
	
	ptr = (void *)i;
	
	int *j = new_int(9);
	
	ptr ++;
	
	ptr = (void*)j;
	ptr = NULL;
	
	int k=0;
	void *p = q;
	for(k=0; k<2; k++){
		int * my_int = (int *)p; 
		printf("pos %d, value %d\n", k,*my_int);
	}
	void *print_me();
	//printf("Start\n");
	//printf("Number of args: %d\n", argc);
	
	//int a = 0;
	//printf("true %d\n", (a == 0));
	//printf("false %d\n", (a != 0));
	
	//run();
	
	
	//printf("Done\n");
	return (0);
}

void *print_me(){
	printf("printme\n");
	return NULL;
}

int *new_int(int in_i){
	int *i = (int *)malloc(sizeof(int));
	*i = in_i;
	return i;
}

