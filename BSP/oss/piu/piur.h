/*======================================================================================
*
* piur.h - PIU Register Header File
* Version 1.0
* Author:	Shao Wei
* Date:		Jan 5 2007
*
========================================================================================*/
#ifndef _PIU_REG_H
#define _PIU_REG_H

//#include "stdint.h"


/* hardware map of each channel */
#pragma pack(4)

typedef struct
{
	/* semaphore */
	uint32_t	sem0s;		/**< semaphore set register 0 */
	uint32_t	sem1s;		/**< semaphore set register 1 */
	uint32_t	sem2s;		/**< semaphore set register 2 */
	uint32_t	sem0c;		/**< semaphore clear register 0 */
	uint32_t	sem1c;		/**< semaphore clear register 1 */
	uint32_t	sem2c;		/**< semaphore clear register 2 */	
	uint32_t	mcu_mask0;	/**< semaphore mcu interrupt mask register 0 */
	uint32_t	mcu_mask1;	/**< semaphore mcu interrupt mask register 1 */
	uint32_t	mcu_mask2;	/**< semaphore mcu interrupt mask register 2 */
	uint32_t	cx_mask0;	/**< semaphore dsp interrupt mask register 0 */
	uint32_t	cx_mask1;	/**< semaphore dsp interrupt mask register 1 */
	uint32_t	cx_mask2;	/**< semaphore dsp interrupt mask register 2 */
	/* command / reply */
	uint32_t	com0;		/**< command register 0 */
	uint32_t	com1;		/**< command register 1 */
	uint32_t	com2;		/**< command register 2 */
	uint32_t	rep0;		/**< reply register 0 */
	uint32_t	rep1;		/**< reply register 1 */
	uint32_t	rep2;		/**< reply register 2 */
	uint32_t	intmask;	/**< com/rep interrupt mask register */
	uint32_t	status;		/**< com/rep interrupt status */
	/* snoop address accessed by external MCU */
	uint32_t	snp_base0;	/**< snoop address base register 0 */
	uint32_t	snp_base1;	/**< snoop address base register 1 */
	uint32_t	snp_msk0;	/**< snoop address mask register 0 */
	uint32_t	snp_msk1;	/**< snoop address mask register 1 */
	uint32_t	snp_en;		/**< snoop address interrupt enable register */
	uint32_t	snp_status;	/**< snoop address interrupt status register */	
}
volatile piu_reg_t, *piu_reg_p;

#pragma pack()

#define PIU_HINTS		0x3F



/* value of bit in intmask register */

//#define PIU_WR_RPY2_HINT	1 << 5		/**< reply reg 2 write interrupt to host */
//#define PIU_WR_RPY1_HINT	1 << 4		/**< reply reg 1 write interrupt to host */
//#define PIU_WR_RPY0_HINT	1 << 3		/**< reply reg 0 write interrupt to host */
//#define PIU_RD_CMD2_HINT	1 << 2		/**< command reg 2 read interrupt to host */
//#define PIU_RD_CMD1_HINT	1 << 1		/**< command reg 1 read interrupt to host */
//#define PIU_RD_CMD0_HINT	1		/**< command reg 0 read interrupt to host */



#endif

