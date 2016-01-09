/*======================================================================================
*
* piu.c 	- PIU control code
* Version 1.0
* Author:	Shao Wei
* Date:		Jan 5 2007
*
========================================================================================*/
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include "stdint.h"
#endif

#include "io.h"
#include "dbg.h"
#include "piur.h"
#include "piu.h"


/* function APIs */

void piu_wr32(piu_p t, uint8_t slot, uint8_t isCmd, uint32_t msg)
{
	volatile uint32_t	*ireg; 

	if (isCmd)
	{
		ireg = &(((piu_reg_p)t->base)->com0);
	}
	else
	{
		ireg = &(((piu_reg_p)t->base)->rep0);
	}
	io_wr32(*(ireg+slot), msg);

}


uint32_t piu_rd32(piu_p t, uint8_t slot, uint8_t isCmd)
{
	volatile uint32_t	*ireg;

	if (isCmd)
	{
		ireg = &(((piu_reg_p)t->base)->com0);
	}
	else
	{
		ireg = &(((piu_reg_p)t->base)->rep0);
	}
	return io_rd32(*(ireg+slot));
}



void piu_init(piu_p t) 
{
	piu_reg_p	reg;
	uint32_t	val = 0;

	reg = (piu_reg_p)t->base;
	val = io_rd32(reg->intmask);
	io_wr32(reg->intmask, val & ~PIU_HINTS);
	val = io_rd32(reg->status);
	io_wr32(reg->status, val & PIU_HINTS);
}

uint32_t piu_status(piu_p t)
{
	return io_rd32(((piu_reg_p)t->base)->status);
}


void piu_clr_status(piu_p t, uint32_t imask)
{
	piu_reg_p	reg;	
	uint32_t	stat;

	reg = (piu_reg_p)t->base;
	stat = io_rd32(reg->status);
	stat &= imask & PIU_HINTS;
	if (stat)
	{
		io_wr32(reg->status, stat);
	}
}

void piu_cfg(piu_p t, void (*evt)(void *, piu_evt_e), void *ctx)
{
	t->ctx = ctx;
	t->evt = evt;
}


void piu_int_ena(piu_p t, uint32_t imask)
{
	piu_reg_p	reg;	

	reg = (piu_reg_p)t->base;
	io_wr32(reg->intmask, io_rd32(reg->intmask) | (imask & PIU_HINTS));
//dbg("EN : intmask=%x\n", io_rd32(reg->intmask));
}


void piu_int_dis(piu_p t, uint32_t imask)
{
	piu_reg_p	reg;	

	reg = (piu_reg_p)t->base;
	io_wr32(reg->intmask, io_rd32(reg->intmask) & ~(imask & PIU_HINTS));
//dbg("DI : intmask=%x\n", io_rd32(reg->intmask));
}


int piu_isr(piu_p t)
{
	piu_reg_p	reg;
	uint32_t	stat;
uint32_t	estat, emask;

	reg = (piu_reg_p)t->base;
estat = io_rd32(reg->status);
emask = io_rd32(reg->intmask);
	stat = emask & estat;
//dbg("sslpiu.ko isr - intmask=%x stat=%x\n", io_rd32(reg->intmask), io_rd32(reg->status));
	stat &= PIU_HINTS;

//printk("%x\n", stat);
	if (!stat)
	{
		dbg("Error: No interrupt to piu %X %X\n", estat, emask);
		stat = io_rd32(reg->intmask) & io_rd32(reg->status);
		stat &= PIU_HINTS;
		if (!stat)
		{
			//dbg("Error: I give up\n");
			return 0;
		}
	}
	io_wr32(reg->status, stat);
	if (stat & PIU_WR_RPY0_HINT)
	{
//printk("*");
		t->evt(t->ctx, PIU_WR_RPY0_HINT);
	}
	if (stat & PIU_WR_RPY1_HINT)
	{
//printk("&");
		t->evt(t->ctx, PIU_WR_RPY1_HINT);
	}
	if (stat & PIU_WR_RPY2_HINT)
	{
//printk("$");
		t->evt(t->ctx, PIU_WR_RPY2_HINT);
	}
#if 0
	if (stat & PIU_RD_CMD2_HINT)
	{
		t->evt(t->ctx, PIU_RD_CMD2_HINT);
	}
	if (stat & PIU_RD_CMD1_HINT)
	{
		t->evt(t->ctx, PIU_RD_CMD1_HINT);
	}
	if (stat & PIU_RD_CMD0_HINT)
	{
		t->evt(t->ctx, PIU_RD_CMD0_HINT);
	}
#endif

	return 1;
}


int piu_clr_pending(piu_p t, piu_evt_e e)
{
	piu_reg_p	reg;	
	uint32_t	stat;
	
	reg = (piu_reg_p)t->base;
	stat = io_rd32(reg->status) & e;
	if (stat)
	{
		io_wr32(reg->status, stat);
		return 1;
	}
	else
	{
		return 0;
	}
}

//clean 071218
uint32_t piu_mask_chk(piu_p t, uint32_t imask)
{
	piu_reg_p	reg;	

	reg = (piu_reg_p)t->base;
	return io_rd32(reg->intmask) & (imask & PIU_HINTS);

}

