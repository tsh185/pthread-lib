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

/*! This public-domain C implementation by Darel R. Finley.

  - Returns true if sort was successful, or false if the nested
    pivots went too deep, in which case your array will have
    been re-ordered, but probably not sorted correctly.

  - This function assumes it is called with valid parameters.

  - Example calls:
    quickSort(&myArray[0],5); // sorts elements 0, 1, 2, 3, and 4
    quickSort(&myArray[3],5); // sorts elements 3, 4, 5, 6, and 7

  - This function was modified by Nick Powers on 2006/08/22.
*/
BOOL quick_sort_tasks(TASK *arr, int elements) {

  int  piv, beg[Q_SORT_MAX_LEVELS], end[Q_SORT_MAX_LEVELS], i=0, L, R;

  beg[0]=0; end[0]=elements;
  while (i>=0) {
    L=beg[i];
    R=end[i]-1;

    if (L<R) {
      piv=arr[L]->_interval; 
      if (i==Q_SORT_MAX_LEVELS-1) return FALSE;
      while (L<R) {
        while (arr[R]->_interval >=piv && L<R) R--; if (L<R) arr[L++]=arr[R];
        while (arr[L]->_interval <=piv && L<R) L++; if (L<R) arr[R--]=arr[L]; 
      }/* end while */
      arr[L]->_interval=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L; 
    }
    else {
      i--;
    }/* end else */
  }/* end while */
  return TRUE;
}
