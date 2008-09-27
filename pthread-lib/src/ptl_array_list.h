/*
 * This file is part of the pthread-lib Library.
 * Copyright (C) 2008-2009 Nick Powers.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in theptl_al_create_array_list() hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */
 

/**
 * This "class" is an implementation of an ArrayList (see Javadoc). It is an
 * array of pointers that can point to any memory. It starts with an inital
 * 'size' and grows (and may shrink) as more elements are put in the list.
 */


#ifndef __PTL_ARRAY_LIST_H__
#define __PTL_ARRAY_LIST_H__

/* Structures */
struct ptl_array_list {
	int capacity; 		/**< largest current size */
	int size;	  		/**< current size */
	int malloc_size; 	/**< size used in the malloc */
	void **array;  		/**< array of pointers - elements in the array */
};

/* Type Definitions */
typedef struct ptl_array_list *ptl_array_list_t;


/* Public Functions */

/**
 * Creates an array list with a starting size of 10.
 * To finish using this data structure, be sure to call the 'destroy' function.
 *
 * @return a fully initialized array list of '10' length
 */
ptl_array_list_t ptl_al_create_array_list();

/**
 * Creates an array list of 'size' length. 
 * To finish using this data structure, be sure to call the 'destroy' function.
 *
 * @param starting size of this array list
 * @return a fully initialized array list of 'size' length
 */
ptl_array_list_t ptl_al_create_array_list_size(int size);

/**
 * Destroy an array list that was created using a 'create' function.
 *
 * @param array list to be freed
 * @return 1 if successful, 0 otherwise
 */
int ptl_al_destroy_array_list(ptl_array_list_t array_list);

/**
 * Destroy an array list that was created using a 'create' function.
 * This function flavor will execute the free function passed in as a parameter
 * on each non-null element in the list.
 *
 * @param array list to be freed
 * @param free function to be executed on each non-null element in the list 
 * @return 1 if successful, 0 otherwise
 */
int ptl_al_destroy_array_list_freefunc(ptl_array_list_t array_list, void (*free_func)(void *));

/**
 * Check if the list if empty.
 *
 * @param array list to be checked if empty
 * @return 1 if empty, 0 otherwise
 */
int ptl_al_is_empty(ptl_array_list_t array_list);

/**
 * Adds a 'value' to the list at the 'end' of the list. It inserts in the
 * (current size - 1) index. If current size is 4, then the element will 
 * be put in index 3 position. If it is found that the element at the end of
 * the list is occupied, then each position after that is tried until the end
 * of the list. If there is no room from 'size' to 'capacity', then the array
 * list is expanded and the element is put at previous capacity + 1.
 *
 * @param array list to add a value
 * @param value to be added to the list
 * @return 1 if successful, 0 otherwise
 */
int ptl_al_add(ptl_array_list_t array_list, void *value);

/**
 * Add the element at 'index'. If the position 'index' is occupied, then
 * all elements past this 'index' are shifted to the right. If this position
 * is unoccupied, then the value is simply inserted.
 *
 * @param array list to add the value
 * @param value to be added to the list
 * @param index position in which to add the 'value'
 * @return 1 if successful, 0 otherwise
 */
int ptl_al_add_index(ptl_array_list_t array_list, void *value, int index);

/**
 * Gets and returns the value stored at index 'index'. If the index requested
 * is past capacity, then NULL is returned.
 *
 * @param array list to get the value at position 'index'
 * @param index used to find the value in the list
 * @return the element at position 'index', even if it's null
 */
void *ptl_al_get(ptl_array_list_t array_list, int index);

/**
 * Sets the 'value' at index 'index. This value will be set at position 'index'
 * even if the element at 'index' is occupied. If 'index' is past 'capacity',
 * then the array list grows and the element is set at 'index'.
 *
 * @param array list to set an element
 * @param value to be set at 'index'
 * @param 'index' to put element 'value'
 * @return 1 if successful, 0 otherwise
 */
int ptl_al_set(ptl_array_list_t array_list, void *value, int index);

/**
 * Removes the element at position 'index' and returns it. The pointer at
 * position 'index' then become null. If the removal of an index greater than
 * capacity is requested, then NULL is returned.
 *
 * @param array list to remove an element
 * @param 'index' that an element will be removed
 * @return the element at 'index', if it exists
 */
void *ptl_al_remove_index(ptl_array_list_t array_list, int index);

/**
 * Remove the element that matches 'value'. Goes through and tries to find an
 * element that equals 'value' (Keep in mind, this will be testing pointer
 * addresses and doesn't compare the element it which it points).
 *
 * @param array list to be searched for 'value'
 * @param 'value' to be searched for in the list
 * @return the 'value' if found, NULL otherwise
 */
void *ptl_al_remove(ptl_array_list_t array_list, void* value);

/**
 * Clears the list of all element. Sets all pointers in the list to NULL.
 * Keep in mind, this does not free any memory. If an element in this list
 * points to memory that is not referenced elsewhere, then this may cause
 * a memory leak.
 *
 * @param array list to be cleared
 */
void ptl_al_clear(ptl_array_list_t array_list);

/**
 * Iterates through the list looking for 'value'. If found, then 1 is returned.
 * If we hit the end of the list and have not found the value, 0 is returned.
 *
 * @param array list to search for 'value'
 * @param 'value' to find
 * @return 1 if this list contains the 'value, 0 otherwise
 */
int ptl_al_contains(ptl_array_list_t array_list, void* value);

/**
 * Finds the index of the 'value'. Searches through the list looking for
 * 'value'. When found, the index of where it was found is returned. If no
 * element was found, then it returns -1.
 *
 * @param array list to search
 * @param 'value' to find
 * @return the index of the 'value' if found, -1 otherwise
 */
int ptl_al_index_of(ptl_array_list_t array_list, void* value);

#endif
