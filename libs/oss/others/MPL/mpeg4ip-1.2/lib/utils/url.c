/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is MPEG4IP.
 *
 * The Initial Developer of the Original Code is Cisco Systems Inc.
 * Portions created by Cisco Systems Inc. are
 * Copyright (C) Cisco Systems Inc. 2004.  All Rights Reserved.
 *
 * Contributor(s):
 *              Bill May        wmay@cisco.com
 */


#include "mpeg4ip.h"
#include "mpeg4ip_utils.h"
static size_t strcount (const char *string, const char *vals)
{
  size_t count = 0;
  if (string == NULL) return 0;

  while (*string != '\0') {
    if (strchr(vals, *string) != NULL) count++;
    string++;
  }
  return count;
}

char *convert_url (const char *to_convert)
{
  static const char *spaces = " \t";
  char *ret, *p;
  size_t count;

  if (to_convert == NULL) return NULL;

  count = strcount(to_convert, spaces);
  count *= 3; // replace each space with %20
  count += strcount(to_convert, "%") * 2;

  count += strlen(to_convert) + 1;
  ret = (char *)malloc(count);
  p = ret;
  while (*to_convert != '\0') {
    if (isspace(*to_convert)) {
      *p++ = '%';
      *p++ = '2';
      *p++ = '0';
    } else if (*to_convert == '%') {
      // 2005.10.21 : if url already "%20", then don't %% again...thanks...HAHA
      if ( (*(to_convert + 1) != '\0') && 
      	   (*(to_convert + 2) != '\0') &&
		   ( ((*(to_convert + 1) >= '0') && (*(to_convert + 1) <= '9')) || 
		     ((*(to_convert + 1) >= 'A') && (*(to_convert + 1) <= 'F')) || 
			 ((*(to_convert + 1) >= 'a') && (*(to_convert + 1) <= 'f')) ) &&
		   ( ((*(to_convert + 2) >= '0') && (*(to_convert + 2) <= '9')) || 
		     ((*(to_convert + 2) >= 'A') && (*(to_convert + 2) <= 'F')) || 
			 ((*(to_convert + 2) >= 'a') && (*(to_convert + 2) <= 'f')) )
	 )
      {
      // do nothing
        *p++ = '%';
      }
      else
      {
        *p++ = '%';
        *p++ = '%';
      }
    } else
      *p++ = *to_convert;
    to_convert++;
  }
  //*p++ = *to_convert; // final \0
  *p = '\0'; // final \0
  return ret;
}

  
