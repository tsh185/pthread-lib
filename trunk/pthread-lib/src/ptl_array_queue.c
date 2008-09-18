/*
 * This file is part of the pthread-lib Library.
 * Copyright (C) 2008-2009 Nick Powers.
 *
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

 /*
  * For a "class" description, see the header file. 
  */

#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <assert.h>
#include "ptl_queue.h"
#include "ptl_util.h"
#include "ptl_array_queue.h"



void ptl_lq_destroy_queue(ptl_q_t q){
	return; 
}

void ptl_lq_init_queue (ptl_q_t q){
	return;
}

int ptl_lq_add(ptl_q_t q, void *value){
	return 0;
}

int ptl_lq_add_wait(ptl_q_t q, void *value, long timeout){
	return 0;
}

void ptl_lq_clear(ptl_q_t q){
	return;
}

void ptl_lq_clear_freefunc(ptl_q_t q, void (*free_func)(void *)){
	return;
}


void* ptl_lq_peek(ptl_q_t q){
	return NULL;
}


void* ptl_lq_get(ptl_q_t q){
	return NULL;
}

void* ptl_lq_get_wait(ptl_q_t q, long timeout){
	return NULL;
}
