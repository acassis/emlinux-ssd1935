/*======================================================================================

* piulib.h - PIU library
* Version 1.1
* Author:	Shao Wei
* Date:		15 Oct 2007
*
========================================================================================*/
#ifndef _PIULIB_H
#define _PIULIB_H

//#include "piu_types.h"


#define PIU_CMD	1
#define PIU_REP	2


#define PIU_MAX_QUEUES		8			// number to q_ids, to be changed

typedef enum
{
	PIU_Q1 = 1,
	PIU_Q2 = 2,
	PIU_Q3 = 3,
	PIU_Q4 = 4,
	PIU_Q5 = 5,
	PIU_Q6 = 6,
	PIU_Q7 = 7,
	PIU_Q8 = 8
}
piu_queue_e;


// same as defined in piu driver

typedef void (*piu_callback_t)(void *, piu_msg_p msg_body);

/* APIs */
int piulib_init(void);
void piulib_exit(void);
void piu_register(piu_callback_t call, piu_queue_e q_id, void *ctx);

int piu_tx(uint32_t msg, piu_msg_p body);
void piulib_enable(void);
void piulib_disable(void);
int8_t piulib_wait_piu_complete();
int piulib_get_piu_thread_id();


#define piu_app2msg(app_body, piu_body, msg_type, app_body_t) \
	do {	\
	piu_body->type = msg_type;	\
	piu_body->len = sizeof(app_body_t);	\
	memcpy(piu_body->p, app_body, piu_body->len);	\
	} while (0);

#define piu_msg2app(app_body, piu_body, app_body_t)	memcpy(app_body, piu_body->p, sizeof(app_body_t)

#endif
