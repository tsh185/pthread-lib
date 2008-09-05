/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include <stdio.h>
#include <stdlib.h> 
#include <stdio.h>
#include <assert.h>
#include "../ptl_queue.h"
#include "../ptl_linked_queue.h"
#include "../ptl_linked_queue_test.h"
#include "../ptl_util.h"

int *new_int(int);

int run()
{
	printf("Start ptl_linked_queue_test\n");
	
	ptl_q_t q = (ptl_q_t)malloc(sizeof(struct ptl_q));
	ptl_lq_init_queue (q);
	int i = 0;
	int *i_ptr = NULL;
	for(i=0; i<10; i++){
		i_ptr = new_int(i*2);
		ptl_lq_add (q, i_ptr);
	}
	
	printf("Number of elements %ld\n", q->size);
	
//	void *e = NULL;
//	while((e = ptl_lq_get (q)) != NULL){
//		int *v_int = (int *)e;
//		printf("Got element: %d\n", *v_int);
//		FREE(e);
//	}
	
	ptl_lq_clear (q);
	
	printf("Number of elements %ld\n", q->size);
	
	FREE(q);
	
	
	printf("Done ptl_linked_queue_test\n");
	return (0);
}

int *new_int(int in_i){
	int *i = (int *)malloc(sizeof(int));
	*i = in_i;
	return i;
}
