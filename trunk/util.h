#ifndef __UTIL_H__
#define __UTIL_H__

/* Macro for easy free */
#define FREE(x) {if(x) {free(x); x = NULL;}}

#define TRUE 1
#define FALSE !TRUE

typedef int BOOL;
#endif
