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
 
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "ptl_array_list.h"
#include "ptl_util.h"

/* Private Functions */
void _check_capacity(ptl_array_list_t *array_list, int size);
void _shift_elements(ptl_array_list_t *array_list, int index, int num_places);

/* creates the array list with an inital size of 10. */
int ptl_al_create_array_list(ptl_array_list_t *array_list){
	return ptl_al_create_array_list_size(array_list, 10);
}


/* creates the array list with an inital size of 'size'. */
int ptl_al_create_array_list_size(ptl_array_list_t *array_list, int size){
	if(array_list == NULL || size <= 0) { return 0; }
	
	// initialize 
	array_list->capacity = size;
	array_list->size = 0;
	array_list->array = NULL;
	
	int malloc_size = size * (sizeof(void *)); // size of ptr * capacity (size)
	array_list->array = (void *)calloc(array_list->capacity, sizeof(void *));
	assert(array_list->array);
	array_list->malloc_size = malloc_size; // store the 'real' size of this array
	
	return (array_list->array != NULL);
}


/* frees all memory allocated in create functions. */
int ptl_al_destroy_array_list(ptl_array_list_t *array_list){
	if(array_list == NULL) { return 0; }
	   
	array_list->capacity = 0;
	array_list->size = 0;
	array_list->malloc_size = 0;
	FREE(array_list->array);
	
	return 1;
}


/* if list if null or size is equal to zero. */
int ptl_al_is_empty(ptl_array_list_t *array_list){
	return (array_list == NULL || array_list->size <= 0);	
}


/* appends the specified element to the end of this list. */
int ptl_al_add(ptl_array_list_t *array_list, void *value){
	if(array_list == NULL || value == NULL) { return 0; }
	
	_check_capacity(array_list, 0);
	// array_list->array[array_list->size] = value;
	//((array_list->array) + (array_list->size)) = value; // this ptr points to the ptr given
	
	void *ptr = array_list->array; // use temp variable
	ptr += array_list->size; // move to our desired position
	ptr = value; // assign the value
	
	array_list->size++; // increase the size
	
	return 1;
}


/* adds, if an element is here, shifts all values to the right. */
int ptl_al_add_index(ptl_array_list_t *array_list, void *value, int index){
	if(array_list == NULL || 
	   index >= array_list->size || 
	   index < 0){ 
		   return 0; 
	   }
	
	_check_capacity(array_list, 1);
	
	// copy all elements to the right
	_shift_elements(array_list, index, 1);
	
	// use a temporary array to shift
	//void *temp_array = (void *)calloc(array_list->malloc_size);
	//assert(temp_array);
	
	//int copy_size = (sizeof(void *))+(index-(array_list->size));
	// copy index to end of array ('index' ... N)
	//memcpy(temp_array, (array_list->array)+index, copy_size);
	// copy from temp back to original array ('index'+1 ... N)
	//memcpy((array_list->array)+index+1, temp_array, copy_size);
	
	// insert the element
	//(array_list->array)+index = value;
	array_list->array[index] = value;
	
	// free out temporary memory
	FREE(temp_array);
	
	return 1;
}


/* gets, and does NOT remove, the element at the 'index'. */
void *ptl_al_get(ptl_array_list_t *array_list, int index){
	if(array_list == NULL || 
	   index < 0 || 
	   index >= array_list->size) {
		   return 0; 
	   }
	
	return array_list->array[index];
	
}


/* set the value at the 'index' position. */
int ptl_al_set(ptl_array_list_t *array_list, void *value, int index){
	if(array_list == NULL || 
	   index < 0 || 
	   index >= array_list->size) {
		   return 0; 
   }
	
	array_list->array[index] = value;
	
	return 1;
}


/* remove the element at index, shift elements left */
void *ptl_al_remove_index(ptl_array_list_t *array_list, int index){
	if(array_list == NULL || 
	   index < 0 || 
	   index >= array_list->size) {
		   return NULL; 
   }
	
	void *element = array_list->array[index];
	// element = (array_list->array)+index;
	array_list->array[index] = NULL;
	
	// shift elements to the left by 1
	_shift_elements(array_list, index, -1);
	
	return element;
}


/* removes the element 'value' if found in the array */
void *ptl_al_remove(ptl_array_list_t *array_list, void* value){
	if(array_list == NULL || value == NULL) {
		return NULL; 
	}
	
	void* element = NULL;
	// search for the element to remove
	int i = 0;
	for(i=0; i<array_list->size; i++){
		if(array_list->array[i] == value){
			break;
		}
	}
	
	// did we find the value?
	if(i < array_list->size){ // found the element
		element = ptl_al_remove_index(array_list, i); // call existing function
	}
	
	return element;
	
}


/* sets all ptrs to null (does not free any memory */
void ptl_al_clear(ptl_array_list_t *array_list){
	if(array_list == NULL) { return; }
	
	// set all ptrs to NULL
	memset(array_list->array, 0, array_list->malloc_size);
}


/* go through array, return true if the element is in the array */
int ptl_al_contains(ptl_array_list_t *array_list, void* value){
	if(array_list == NULL || value == NULL) {
		   return NULL; 
   }
	
	int i = 0;
	for(i=0; i<array_list->size; i++){
		if(array_list->array[i] == value){
			return 1;
		}
	}
	
	return 0;
}


/* go through array, return the index of the element if the element is in the array */
int ptl_al_index_of(ptl_array_list_t *array_list, void* value){
		if(array_list == NULL || 
	   index < 0 || 
	   index >= array_list->size) {
		   return NULL; 
   }
	
	int i = 0;
	for(i=0; i<array_list->size; i++){
		if(array_list->array[i] == value){
			return i;
		}
	}
	
	return -1;
}


/* check if array needs to be expanded, if so, it expands it */
void _check_capacity(ptl_array_list_t *array_list, int size){
	// don't check array_list, assume it's not null
	if( (array_list->size + size) >= array_list->capacity){ // make the array list bigger
		
		// calculate new capacity
		int new_capacity = (array_list->capacity * 3)/2 + 1;
		
		// allocate new memory
		int malloc_size = new_capacity * (sizeof(void *)); 
		void *new_array = (void *)calloc(malloc_size);
		assert(new_array);
		
		// copy elements to the new array using the 'real' size
		memcpy(new_array, array_list->array, array_list->malloc_size);
		void *old_array = array_list->array;
		
		// save new capcity and malloc size
		array_list->capacity = new_capacity;
		array_list->malloc_size = malloc_size;
		array_list->array = new_array;
		// size stays the same
		
		// free old memory
		FREE(old_array);
		
	}
}


/* shifts the elements of the array left or right */
void _shift_elements(ptl_array_list_t *array_list, int index, int num_places){
	// use a temporary array to shift
	void *temp_array = (void *)calloc(array_list->malloc_size);
	assert(temp_array);
	
	int copy_size = 0;
	void* start = 0;
	
	if(num_places > 0){ // shift right
		// index ... size
		copy_size = (sizeof(void *))*(index-(array_list->size)); 
		start = (array_list->array)+index;
		
	} else { // (num_places is negative) - shift left
		int pos_num_places = num_places * -1;
		// index+1 ... size elements
		copy_size = (sizeof(void *)) * 
				((index+pos_num_places)-(array_list->size)); 
		
		start = (array_list->array)+index+pos_num_places;
	}
	// copy part to shift into temp array
	memcpy(temp_array, start, copy_size);
	
	// copy from temp back to original array
	memcpy((array_list->array)+index+num_places, temp_array, copy_size);
	
	// free out temporary memory
	FREE(temp_array);
}
