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

/*! @file signal_handler.h
    @brief The header file to signal_handler.c
*/

#ifndef __SIGNAL_MANAGER_H__
#define __SIGNAL_MANAGER_H__

/* Structs */
struct function_ptrs_struct {
  void *(*term_func_ptr)();
  void *(*user1_func_ptr)();
  void *(*user2_func_ptr)();
};
typedef struct function_ptrs_struct FUNCTION_PTRS;


/* Function Prototypes */
int signal_handler_create(void *function_ptrs);
int block_all_signals();
int  block_most_signals();

#endif