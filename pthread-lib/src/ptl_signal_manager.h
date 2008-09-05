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
 

#ifndef __PTL_SIGNAL_MANAGER_H__
#define __PTL_SIGNAL_MANAGER_H__

/* Defines */
#define SIGNAL_HANDLER_DESTROY_SIGNAL SIGQUIT

/* Structs */
struct ptl_sh_funcs {
  void *(*hup_func_ptr)();  /**< 1    Exit    Hangup                           */
  void *(*int_func_ptr)();  /**< 2    Exit    Interrupt                        */
  void *(*alarm_func_ptr)();/**< 14   Exit    Alarm Clock                      */
  void *(*term_func_ptr)(); /**< 15   Exit    Terminated                       */
  void *(*user1_func_ptr)();/**< 16   Ignore  User Signal 1                    */
  void *(*user2_func_ptr)();/**< 17   Ignore  User Signal 2                    */
  void *(*child_func_ptr)();/**< 18   Ignore  Child status                     */
  void *(*cont_func_ptr)(); /**< 25   Ignore  Continued                        */
  void *(*wait_func_ptr)(); /**< 32   Ignore  All LWPs Blocked                 */
  void *(*lwp_func_ptr)();  /**< 33   Ignore  Virtual Interprocessor Interrupt */
                            /*                for threads library              */
  void *(*aio_func_ptr)();  /**< 34   Ignore  Asynchronous I/O                 */
};

/* Typedefs */
typedef struct ptl_sh_funcs* ptl_sh_funcs_t;


int ptl_signal_handler_create(pthread_t sig_mgr, ptl_sh_funcs_t func_ptrs);
void stop_signal_manager(pthread_t sig_mgr);
int destroy_signal_manager(pthread_t sig_mgr);

#endif
