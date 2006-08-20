#ifndef __UTIL_H__
#define __UTIL_H__

/* Macro for easy free */
#define FREE(x) {if(x) {free(x); x = NULL;}}

#define TRUE 1
#define FALSE !TRUE

typedef int BOOL;

#define CHECK_STATUS(x,y){if(x != 0){if(y){printf("%s",y);} printf(" (status = %d)\n", status); } }
#define CHECK_STATUS_UNIX(status, api, msg){if( status != 0 ) {int _errno = errno;if( api )printf( "%s ", api );printf( "failure" );if( msg )printf( ": %s", msg );printf( " (status = %d, errno = %d)\n", status, _errno );exit( 1 ); }}

#endif
