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
 
/* See header file for documentation. */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include "ptl_signal_manager.h"
#include "ptl_util.h"



/* Private Functions */
void *_ptl_signal_handler_function(void *functions);


/* Creates a single thread to handle all interrupt signals */
ptl_signal_manager_t ptl_signal_handler_create(ptl_smgr_funcs_t func_ptrs){
	if(func_ptrs == NULL){ return NULL; }

	ptl_signal_manager_t signal_manager = (ptl_signal_manager_t)calloc(1, sizeof(struct ptl_signal_manager));
	assert(signal_manager);
	
	signal_manager->func_ptrs = func_ptrs;
	signal_manager->running = 1;
	
	// _signal_handler_function is a private function declared below
	int rc = pthread_create(&(signal_manager->smgr_thread), NULL, _ptl_signal_handler_function, (void *)signal_manager);
	assert(rc);
	
	//signal_manager->smgr_thread = smgr_thread;

	
	return signal_manager;
}


/* Stops the thread manager by setting a flag and sending a signal. */
void stop_signal_manager(ptl_signal_manager_t sig_mgr){

	/* set flag to stop */
	sig_mgr->running = 0;

	/* send signal to thread to break it out of wait */
	pthread_kill(sig_mgr->smgr_thread, PTL_SIGNAL_HANDLER_DESTROY_SIGNAL);
	
	destroy_signal_manager(sig_mgr);
}


/* Synchronizes the signal handler and frees the memory. */
int destroy_signal_manager(ptl_signal_manager_t sig_mgr){
	void *ret_value = 0;

	/* wait for the thread to stop */
	int status = pthread_join(sig_mgr->smgr_thread, &ret_value);
	
	// free memory
	FREE(sig_mgr);
	
	return (status == 0);
}


/**
 * The function the signal handler executes when a signal is sent to this 
 * process. This function executes one of the function pointers assigned to it
 * during it's creation. The signal handler stays in this function for it's
 * entire life until it is told to break out of the loop.
 *
 * @param signal manager that contains a set of functions to execute when a signal is sent
 */
void *_ptl_signal_handler_function(void *sig_mgr){
	assert(sig_mgr);
	
	sigset_t signals;
	int rc = 0;
	int sig_caught;

	ptl_signal_manager_t signal_manager = (ptl_signal_manager_t)sig_mgr;
	ptl_smgr_funcs_t func_ptrs = signal_manager->func_ptrs;

	sigfillset(&signals);
	while(signal_manager->running){ 

		rc = sigwait(&signals, &sig_caught);

		printf("Caught signal [%d]...\n", sig_caught);
	 
		switch(sig_caught){
		case SIGTERM:
			signal_manager->running = 0; // stop the signal manager
				
			if(func_ptrs->term_func_ptr != NULL){
				func_ptrs->term_func_ptr();
			}
		break;
		case SIGHUP:
			if(func_ptrs->hup_func_ptr != NULL){
				func_ptrs->hup_func_ptr();
			}
		break;
		case SIGINT:
			if(func_ptrs->int_func_ptr != NULL){
				func_ptrs->int_func_ptr();
			}
		break;
		case SIGQUIT:
			if(func_ptrs->term_func_ptr != NULL){
				func_ptrs->term_func_ptr();
			}
		break;
		case SIGABRT:
			if(func_ptrs->abort_func_ptr != NULL){
				func_ptrs->abort_func_ptr();
			}	
		break;
		case SIGUSR1:
			if(func_ptrs->user1_func_ptr != NULL){
				func_ptrs->user1_func_ptr();
			}
		break;
		case SIGUSR2:
			if(func_ptrs->user2_func_ptr != NULL){
				func_ptrs->user2_func_ptr();
			}
		break;
		case SIGALRM:
			if(func_ptrs->alarm_func_ptr != NULL){
				func_ptrs->alarm_func_ptr();
			}
		break;
		case SIGCHLD:
			if(func_ptrs->child_func_ptr != NULL){
				func_ptrs->child_func_ptr();
			}
		break;
		case SIGCONT:
			if(func_ptrs->cont_func_ptr != NULL){
				func_ptrs->cont_func_ptr();
			}
		break;
		default:
			printf("Caught Unsupported Signal [%d], Igoring.\n", sig_caught);
		break;
		} /* end switch */
	} /* end while(still_running) */

	
	// we should get here if the thread is told to stop
	pthread_exit(NULL);
	
	return NULL;
}


/* Blocks all signal for the thread calling this function */
int block_all_signals(){

	sigset_t signals;
	sigfillset(&signals);

	int sigmask_rc = pthread_sigmask(SIG_BLOCK, &signals, NULL);
	return (sigmask_rc == 0);
}
