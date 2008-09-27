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
 
#ifndef __PTL_UTIL_H__
#define __PTL_UTIL_H__

#include <time.h>
#include <sys/time.h>

/* Macros */

/* Macro for easy free */
#define FREE(x) {if(x) {free(x); x = NULL;}}
/* get a line from a file */
#define GET_LINE(file,line,line_size,str,val,delim,delim2,sp,val_ptr,str_ptr){ fgets(line,(line_size),file); str=strtok_r(line,delim,sp); if(!str){continue;} val=strtok_r(NULL, delim2, sp); str_ptr=strip_whitespace(str); val_ptr=strip_whitespace(val); }
/* strcat that concatenates all the chars it can */
#define STR_CAT(dest,src,max) { if((strlen(dest)+strlen(src)) < max){ strcat(dest,src);} else {strncat(dest,src,(max-(strlen(dest)+strlen(src))));} }

#define COPY(dest,src,size){ memset(dest,0,size); memcpy(dest,src,size); }

#define INT_TO_CHAR(num,c){ sprintf(c,"%i", num);  }

# define TIMEVAL_TO_TIMESPEC(tv, ts) {                                   \
        (ts)->tv_sec = (tv)->tv_sec;                                    \
        (ts)->tv_nsec = (tv)->tv_usec * 1000;                           \
}
# define TIMESPEC_TO_TIMEVAL(tv, ts) {                                   \
        (tv)->tv_sec = (ts)->tv_sec;                                    \
        (tv)->tv_usec = (ts)->tv_nsec / 1000;                           \
}


/* Public Functions */

/**
 * Wait wait_usec microseconds. A local mutex and cond is created to call the
 * pthread_cond_timedwait function.
 *
 * @param wait_usec number of microseconds to sleep
 * @return the result of calling pthread_cond_timedwait()
 */
int ptl_timed_wait(long wait_usec);


/**
 * Gets the time 'usec' microseconds later.
 * 
 * @param ts the result will be stored in this struct
 * @param usec number of microseconds later to get the time
 */
void ptl_get_future_time(struct timespec *ts, long usec);

#endif
