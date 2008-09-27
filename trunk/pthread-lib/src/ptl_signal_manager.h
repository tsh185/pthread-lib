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
 * This "class" is a thread dedicated to handling signals. When multithreading,
 * one does not know which thread will catch a signal. Using these functions
 * one is able to block all the signals from the threads (think lineman and
 * quaterback)and handle all the signals sent to the running process. 
 * It is made flexible by allowing one to execute user-defined functions for 
 * a wide array of signals.
 */

#ifndef __PTL_SIGNAL_MANAGER_H__
#define __PTL_SIGNAL_MANAGER_H__

/* Defines */
#define PTL_SIGNAL_HANDLER_DESTROY_SIGNAL SIGTERM

/* Structures */
struct ptl_smgr_funcs {
	void *(*hup_func_ptr)();  /**< 1    Hangup (POSIX).                       */
	void *(*int_func_ptr)();  /**< 2    Interrupt (ANSI)                      */
	void *(*quit_func_ptr)(); /**< 3    Quit (POSIX)                          */
	void *(*abort_func_ptr)();/**< 6   Abort (ANSI)                           */
	void *(*user1_func_ptr)();/**< 10   User-defined signal 1 (POSIX)         */
	void *(*user2_func_ptr)();/**< 12   User-defined signal 2 (POSIX)         */
	void *(*alarm_func_ptr)();/**< 14   Alarm clock (POSIX)                   */
	void *(*term_func_ptr)(); /**< 15   Termination (ANSI)                    */
	void *(*child_func_ptr)();/**< 17   Child status has changed (POSIX)      */
	void *(*cont_func_ptr)(); /**< 28   Continue (POSIX)                      */
};

struct ptl_signal_manager {
	struct ptl_smgr_funcs* func_ptrs;
	pthread_t smgr_thread;
	int running;
};

/* Type Definitions */
typedef struct ptl_smgr_funcs* ptl_smgr_funcs_t;
typedef struct ptl_signal_manager* ptl_signal_manager_t;


/* Public Functions */

/**
 * Creates a single thread to handle all interrupt signals.
 * The function pointer that correlated with each signal will be executed
 * when that signal is encountered.  If a ptr is null, nothing happens.
 *
 *
 * @param func_ptrs set of functions to be executed for various signals
 * @return a new signal manager
 */
ptl_signal_manager_t ptl_signal_handler_create(ptl_smgr_funcs_t func_ptrs);

/**
 * Stops the thread manager by setting a flag and sending a signal.
 * This function must send a signal to the manager to break it out of
 * sigwait. The signal is not assignable in the set of function pointers to
 * avoid undesired behavior.
 * @note A better way to design it would be to make the signal handler stop
 *       when the program stops and use destroy_signal_handler only.
 *
 * @param sig_mgr the created signal manager
 */
void stop_signal_manager(ptl_signal_manager_t sig_mgr);

/**
 * Synchronizes the signal handler and frees the memory.
 * Do not use this function after calling stop_signal_handler.
 *
 * @param sig_mgr the created signal manager
 */
int destroy_signal_manager(ptl_signal_manager_t sig_mgr);

/**
 * Blocks all signal for the thread calling this function.
 * This function must be called per thread for all threads to block all
 * signals.
 *
 * @return 1 if successful, 0 otherwise
 */
int block_all_signals();

#endif
