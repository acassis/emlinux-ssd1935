/*======================================================================================
*
* piu.h - PIU Control Code Header File
*
* Version 1.0
* Author:	Shao Wei
* Date:		Jan 5 2007
*
========================================================================================*/
#ifndef _PIU_H
#define _PIU_H

/* macros */

//#define PIU_MAX_SLOTS		3


/* bits in intmask register */

#define PIU_WR_COM2_CINT_BIT	8
#define PIU_WR_COM1_CINT_BIT	7
#define PIU_WR_COM0_CINT_BIT	6

#define PIU_WR_RPY2_HINT_BIT	5
#define PIU_WR_RPY1_HINT_BIT	4
#define PIU_WR_RPY0_HINT_BIT	3
#define PIU_RD_CMD2_HINT_BIT	2
#define PIU_RD_CMD1_HINT_BIT	1
#define PIU_RD_CMD0_HINT_BIT	0


/* enum */
typedef enum
{
	PIU_ERR_NONE = 0,
	PIU_ERR_PARM = -1,
	PIU_ERR_CFG = -2
}
piu_err_e;

typedef enum
{
#if 0
	PIU_RD_CMD2_HINT = (1 << PIU_RD_CMD2_HINT_BIT),
	PIU_RD_CMD1_HINT = (1 << PIU_RD_CMD1_HINT_BIT),
	PIU_RD_CMD0_HINT = (1 << PIU_RD_CMD0_HINT_BIT),
#endif
	PIU_WR_RPY2_HINT = (1 << PIU_WR_RPY2_HINT_BIT),
	PIU_WR_RPY1_HINT = (1 << PIU_WR_RPY1_HINT_BIT),
	PIU_WR_RPY0_HINT = (1 << PIU_WR_RPY0_HINT_BIT)
}
piu_evt_e;


/** mcu PIU context */
typedef struct
{
/* public */
	void	*base;					/**< logical register base address */
	
/* private */
	void	*ctx;					/**< context pointers for event callback functions */
	void	(*evt)(void *ctx, piu_evt_e e);	/**<  event callback functions */

}
piu_t, *piu_p;


/* APIs */

void piu_wr32(piu_p t, uint8_t slot, uint8_t isCmd, uint32_t msg);
/**<
Write message to piu register
@param[in]	t		context
@param[in]	slot		slot index, 0~PIU_MAX_SLOT-1
@param[in]	isCmd		1, write to COM[slot] register; 0, write to REP[slot] register
@param[in]	msg		message written to piu register
*/

uint32_t piu_rd32(piu_p t, uint8_t slot, uint8_t isCmd);
/**<
Read message from piu register
@param[in]	t		context
@param[in]	slot		slot index, 0~PIU_MAX_SLOT-1
@param[in]	isCmd		1, read from COM[slot] register; 0, read from REP[slot] register
@return				message read from piu register
*/

void piu_init(piu_p t);
/**<
Initialize piu module, disable and clear all hint bits
@param[in]	t		context
*/

void piu_cfg(piu_p t, void (*evt)(void *, piu_evt_e), void *ctx);
/**<
Configure PIU interrupt callback function
@param[in]	t		context
@param[in]	evt		interrupt callback function
@param[in]	ctx		interrupt callback context
*/

uint32_t piu_status(piu_p t);
/**<
Read PIU status register
@param[in]	t		context
@return				status value
*/

void piu_clr_status(piu_p t, uint32_t imask);
/**<
clear PIU status register by imask value
@param[in]	t		context
@param[in]	imask		bitwise or-ed status to be cleared
*/

void piu_int_ena(piu_p t, uint32_t imask);
/**<
Enable certain PIU interrupts
@param[in]	t		context
@param[in]	imask		bitwise or-ed interrupt to be enabled
*/

void piu_int_dis(piu_p t, uint32_t imask);
/**<
Disable certain PIU interrupts
@param[in]	t		context
@param[in]	imask		bitwise or-ed interrupt to be disabled
*/

int piu_isr(piu_p t);
/**<
Host piu interrupt service routine
@param[in]	t		context
@return				0, interrupt is not processed, otherwise, processed
*/


int piu_clr_pending(piu_p t, piu_evt_e e);
/**<
clear any event status belonging to slot
@param[in]	t		context
@param[in]	e		status of event
@return				1, if the specified status is 1 and therefore is cleared
				0, if the specified status is 0
*/

//clean 071218
uint32_t piu_mask_chk(piu_p t, uint32_t imask);

#endif
