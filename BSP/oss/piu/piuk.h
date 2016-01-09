/*======================================================================================
*
* piuk.h - PIU driver API
*
* Version 1.0
* Author:	Shao Wei
* Date:		Jan 5 2007
*
========================================================================================*/
#ifndef _SSL_PIU_H
#define _SSL_PIU_H

#include "piu_types.h"

/* APIs */

int piu_tx(uint32_t msg, piu_msg_p body);
int piu_rx(uint32_t *msg, piu_msg_p body);

#endif
