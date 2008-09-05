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
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "ptl_util.h"

/**
 * A set of functions used to support the functionality found in this library.
 * It also houses useful wrappers useful to the programmer using this library.
 */

/* See header file for documentation */

/* Wait wait_usec microseconds. */
int ptl_timed_wait(long wait_usec){
  int timed_wait_result;
  struct timespec   ts;

  pthread_mutex_t timed_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t  timed_wait_cond = PTHREAD_COND_INITIALIZER;

  ptl_get_future_time(&ts, wait_usec);

  pthread_mutex_lock(&timed_wait_mutex); // lock
  /* Perform Wait - the mutex is assumed to be locked */
  timed_wait_result = pthread_cond_timedwait(&timed_wait_cond, &timed_wait_mutex, &ts);

  pthread_mutex_unlock(&timed_wait_mutex); // unlock
	
  /* Destroy local mutex and cond */
  pthread_mutex_destroy(&timed_wait_mutex);
  pthread_cond_destroy(&timed_wait_cond);

  return timed_wait_result;
}

/* Gets the time 'usec' microseconds later. */
void ptl_get_future_time(struct timespec *ts, long usec){
  struct timeval    tp;
  memset(&tp,0,sizeof(tp));
	
  gettimeofday(&tp, NULL);
	
  tp.tv_usec += usec; // add time
  TIMEVAL_TO_TIMESPEC(&tp, ts); // defined locally
}
