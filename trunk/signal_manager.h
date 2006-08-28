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

/* Defines */
#define SIGNAL_HANDLER_DESTROY_SIGNAL SIGQUIT

/* Structs */
struct function_ptrs_struct {
  void *(*hup_func_ptr)();  /* 1    Exit    Hangup                           */
  void *(*int_func_ptr)();  /* 2    Exit    Interrupt                        */
  void *(*alarm_func_ptr)();/* 14   Exit    Alarm Clock                      */
  void *(*term_func_ptr)(); /* 15   Exit    Terminated                       */
  void *(*user1_func_ptr)();/* 16   Ignore  User Signal 1                    */
  void *(*user2_func_ptr)();/* 17   Ignore  User Signal 2                    */
  void *(*child_func_ptr)();/* 18   Ignore  Child status                     */
  void *(*cont_func_ptr)(); /* 25   Ignore  Continued                        */
  void *(*wait_func_ptr)(); /* 32   Ignore  All LWPs Blocked                 */
  void *(*lwp_func_ptr)();  /* 33   Ignore  Virtual Interprocessor Interrupt */
                            /*              for threads library              */
  void *(*aio_func_ptr)();  /* 34   Ignore  Asynchronous I/O                 */
};
typedef struct function_ptrs_struct FUNCTION_PTRS;


/* Function Prototypes */
int  signal_handler_create(void *function_ptrs);
int  block_all_signals();
int  block_most_signals();

#endif