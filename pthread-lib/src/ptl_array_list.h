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
 
#ifndef __PTL_ARRAY_LIST_H__
#define __PTL_ARRAY_LIST_H__

struct ptl_array_list {
	int capacity; 		/**< largest current size */
	int size;	  		/**< current size */
	int malloc_size; 	/**< size used in the malloc */
	void *array;  		/**< elements in the array */
};

typedef struct ptl_array_list ptl_array_list_t;


int ptl_al_create_array_list(ptl_array_list_t *array_list);
int ptl_al_create_array_list_size(ptl_array_list_t *array_list, int size);
int ptl_al_destroy_array_list(ptl_array_list_t *array_list);
int ptl_al_is_empty(ptl_array_list_t *array_list);
int ptl_al_add(ptl_array_list_t *array_list, void *value);
int ptl_al_add_index(ptl_array_list_t *array_list, void *value, int index);
void *ptl_al_get(ptl_array_list_t *array_list, int index);
int ptl_al_set(ptl_array_list_t *array_list, void *value, int index);
void *ptl_al_remove_index(ptl_array_list_t *array_list, int index);
void *ptl_al_remove(ptl_array_list_t *array_list, void* value);
void ptl_al_clear(ptl_array_list_t *array_list);
int ptl_al_contains(ptl_array_list_t *array_list, void* value);
int ptl_al_index_of(ptl_array_list_t *array_list, void* value);


#endif
