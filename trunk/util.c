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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "util.h"

/*****************************************************************************/
/*! 
*/
/*****************************************************************************/
int is_digit(char *str){
  int i = 0;
  int len = 0;
  int result = 1;

  if(!str){
    return !result;
  }

  len = strlen(str);

  for(i=0; i < len; i++){
    if(!isdigit(str[i])){
      result = 0;
      break;
    }
  }

  return result;
}
