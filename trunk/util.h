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

#ifndef __UTIL_H__
#define __UTIL_H__

#define FALSE 0
#define TRUE !FALSE 

typedef int BOOL;
typedef void (*FUNC_PTR)();

#define READ "r"
#define FILE_LINE_SIZE 180

#define Q_SORT_MAX_LEVELS 1000

/* Macro for easy free */
#define FREE(x) {if(x) {free(x); x = NULL;}}
/* Macro to check status of thread operations */
#define CHECK_STATUS(status, api, msg){if( status != 0 ) {int _errno = errno;if( api )printf( "%s ", api );printf( "failure" );if( msg )printf( ": %s", msg );printf( " (status = %d, errno = %d)\n", status, _errno );exit( 1 ); }}
/* Macro to log an error */
#define LOG_ERROR(method, msg) {printf("ERROR - %s %s\n",method,msg);  }
/* Macro for easy malloc check */
#define CHECK_MALLOC(var,method,msg){if(!var){LOG_ERROR(method,msg); exit(ERROR_CODE_MALLOC);}}

#define CHECK_TEST(num,result){if(!result){printf("Test %d failed\n", num);} }

#define GET_LINE(file,line,line_size,str,val,delim,delim2,sp){ fgets(line,(line_size),file); str=strtok_r(line,delim,&sp); if(!str){continue;} val=strtok_r(NULL, delim2, sp); }
/* strcat that concatenates all the chars it can */
#define STR_CAT(dest,src,max) { int _s = strlen(dest)+strlen(src); if(_s < max){ strcat(dest,src);} else {strncat(dest,src,(max-_s));} }


/* General Error Codes */
#define ERROR_CODE_MALLOC 1


/* Public Functions */
int is_digit(char *str);

#endif
