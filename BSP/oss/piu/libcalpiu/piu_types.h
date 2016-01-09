/*======================================================================================
*
* piu_types.h - PIU type definition
*
* Version 1.0
* Author:	Shao Wei
* Date:		Jan 5 2007
*
========================================================================================*/
#ifndef _PIU_TYPES_H
#define _PIU_TYPES_H


#define PIU_INTERNAL_MEM	64

#define PIU_IOCTL_IRQENA	0
#define PIU_IOCTL_IRQDIS	1
#define PIU_IOCTL_STATUS	2
#define PIU_IOCTL_CLR_STATUS	3

#define PIU_IOCTL_TX		4
#define PIU_IOCTL_RX		5

#define PIU_IOCTL_ENA		6
#define PIU_IOCTL_DIS		7

#define PIU_IOCTL_RX_MSG	8
#define PIU_IOCTL_INIT		9
#define PIU_IOCTL_CLOSE		10

#define PIU_IOCTL_NONBLOCK_ENA	11
#define PIU_IOCTL_NONBLOCK_DIS	12
#define	PIU_IOCTL_RTSIGNO	13

/* struct */
//#pragma pack(1)
typedef struct 
{
	uint32_t	type;
	uint32_t	len;
	uint8_t		p[PIU_INTERNAL_MEM];
}
piu_msg_t, *piu_msg_p;
//#pragma pack()

struct piu_protl
{
	uint32_t	msg;
	piu_msg_t	msg_body;
};



#endif
