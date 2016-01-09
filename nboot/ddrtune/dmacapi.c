#define NPRINTF	1
//#define DBG_DMA

#ifdef DBG_DMA
#include "magus.h"
#include "debug.h"
#include "str.h"
#endif


#if (NPRINTF==1)
#else
#include <stdio.h>
#endif

#include "dmactype.h"
#include "dmacapi.h"

#if (NPRINTF==1)
#define dbg(s,...)
#else
#define dbg	printf
#endif

dmac_t	*_g_dmac;
unsigned long	_g_nch;
unsigned long	_g_fifo_sz;

#define DMAC_RST_DLY	0x10000
static int dma_use_num = 1;

int dmac_init (unsigned long base, dmac_cfg_t *cfg)
{
	int i;

	if (dma_use_num != 1)
	{
		dbg ("dmac_init: already initialised\n");
		return -1;
	}
	dma_use_num = 1;

	_g_dmac = (dmac_t *) base;

	if (_g_dmac->id != DMAC_ID)
	{
		dbg ("dmac_init: id mismatch\n");
		dbg ("dmac_init: id mismatch: %08lx, exp: %08lx\n",
			_g_dmac->id, DMAC_ID);
		return -1;
	}

	switch (GET_DMAC_NCH (_g_dmac->cap))
	{
		case 0: _g_nch = 8; break;
		case 1: _g_nch = 16; break;
		case 2: _g_nch = 24; break;
		case 3: _g_nch = 32; break;
		default: _g_nch = 8; break;
	}
	
	switch (GET_DMAC_FIFOSZ(_g_dmac->cap))
	{
		case 0: _g_fifo_sz = 16; break;
		case 1: _g_fifo_sz = 32; break;
		case 2: _g_fifo_sz = 64; break;
		case 3: _g_fifo_sz = 128; break;
		default: _g_fifo_sz = 16; break;	
	}
	
	cfg->id = _g_dmac->id;
	cfg->n_ch = _g_nch;
	cfg->fifo_sz = _g_fifo_sz;
	
	// reset the module
	
	_g_dmac->ctl = DMAC_CTL_RST | DMAC_CTL_EN;
	for (i = 0; i < DMAC_RST_DLY; i++)
	{
		if (_g_dmac->ctl & DMAC_CTL_RST)
			continue;

		break;
	}

	_g_dmac->ctl &= ~DMAC_CTL_EN;
	if (i == DMAC_RST_DLY)
	{
		dbg ("dmac_init: dmac reset failed\n");
		_g_dmac = (dmac_t *)0;
		return -1;
	}

	// configure the controller
	_g_dmac->ctl = DMAC_WTDLY (cfg->wait_dly) | 
			DMAC_BT (cfg->burst_type) | 
			(cfg->pri ? DMAC_CTL_PRI : 0) | 
			DMAC_CTL_EN;
			

	return 0;
}

int dmac_exit (void)
{
	if (!_g_dmac)
		return -1;

	dmac_rst ();

	_g_dmac->ctl &= ~DMAC_CTL_EN;
	_g_dmac = 0;

	return 0;
}

int dmac_rst (void)
{
	int i;
	unsigned long en;

	if (!_g_dmac)
		return -1;
	
	en = _g_dmac->ctl & DMAC_CTL_EN;

	for (i = 0; i < _g_nch; i++)
	{
		dmac_chan_rst (i);
	}

	_g_dmac->ctl |= (DMAC_CTL_RST | DMAC_CTL_EN);

	for (i = 0; i < DMAC_RST_DLY; i++)
	{
		if (_g_dmac->ctl & DMAC_CTL_RST)
			continue;
		break;
	}
	
	if (!en)
		_g_dmac->ctl &= ~DMAC_CTL_EN;
	
	if (i < DMAC_RST_DLY)
		return 0;

	dbg ("dmac_rst: reset failed\n");
	return -1;
}

int dmac_chan_rst (unsigned long id)
{
	int i;
	chan_t	*chan;
	
	if (!_g_dmac)
		return -1;

	if (id >= _g_nch)
		return -1;
	
	chan = (chan_t *) &_g_dmac->chan[id];

	chan->cfg |= DMAC_CFG_RST;
	for (i = 0; i < DMAC_RST_DLY; i++)
	{
		if (chan->cfg & DMAC_CFG_RST)
			continue;
		break;
	}

	if (i == DMAC_RST_DLY) 
	{
		dbg ("dmac_chan[%lu]: reset failed\n", id);
		return -1;
	}

	return 0;
}

int dmac_chan_en (unsigned long id)
{
	if (!_g_dmac)
		return -1;

	if (id >= _g_nch)
		return -1;
	
	_g_dmac->chan[id].cfg |= DMAC_CFG_EN;

	return 0;
}

int dmac_chan_dis (unsigned long id)
{
	int i;
	chan_t	*chan;

	if (!_g_dmac)
		return -1;

	if (id >= _g_nch)
		return -1;
	
	chan = (chan_t *) &_g_dmac->chan[id];
	if (!(chan->cfg & DMAC_CFG_EN))
	{
		dbg ("dmac_chan[%lu]: not enabled\n", id);
		return 0;
	}

	chan->cfg |= DMAC_CFG_DIS;
	for (i = 0; i < DMAC_RST_DLY; i++) 
	{
		if (chan->cfg & DMAC_CFG_DIS)
			continue;
		break;
	}

	if (i == DMAC_RST_DLY)
	{
		dbg ("dmac_chan[%lu]: disable failed\n", id);
		return -1;
	}

	return 0;
}

int dmac_chan_cfg (unsigned long id, dmac_ch_cfg_t *cfg)
{
	chan_t	*chan;
#ifdef DBG_DMA
	char	p[30];
#endif

	if (!_g_dmac)
		return -1;

	if (id >= _g_nch)
		return -1;
	
	if (!cfg)
		return -1;

	chan = &_g_dmac->chan[id];
	if (chan->cfg & DMAC_CFG_EN)
	{
		dbg ("dmac_chan[%lu]: addr=%08lx, cannot config an enabled channel\n", 
			id, (unsigned long) chan);
		return -1;
	}

#if 0
 	// commented because of lack of module support in Sasi's magus.lib
	if (cfg->src_type == DMAC_TYP_FIFO) 
	{
		if ((cfg->transfer_cnt % cfg->src_asz) == 0)
		{
			dbg ("dmac_chan[%lu]: transfer_cnt(%lu) not multiple of src access size(%lu)\n",
				id, cfg->transfer_cnt, cfg->src_asz);
			return -1;
		}
	}
	if (cfg->dst_type == DMAC_TYP_FIFO)
	{
		if ((cfg->transfer_cnt % cfg->dst_asz) == 0)
		{
			dbg ("dmac_chan[%lu]: transfer_cnt(%lu) not multiple of dst access size(%lu)\n",
				id, cfg->transfer_cnt, cfg->dst_asz);
			return -1;
		}
	}
#endif
	chan->cfg = 	DMAC_BL (cfg->burst_len) | 
			DMAC_STYP (cfg->src_type) |
			DMAC_SASZ (cfg->src_asz) | 
			DMAC_SASZ (cfg->src_req) | 
			DMAC_STYP (cfg->dst_type) |
			DMAC_SASZ (cfg->dst_asz) | 
			DMAC_SASZ (cfg->dst_req) | 
			(cfg->flush_en ? DMAC_CFG_FLSHEN : 0);

	chan->saddr = cfg->src_addr;
	chan->daddr = cfg->dst_addr;
	chan->tc = cfg->transfer_cnt;

#ifdef DBG_DMA
	print ("cfg->transfer_cnt:"); print (long2str(p,cfg->transfer_cnt)); print ("\n");
#endif
	dbg ("dmac_chan[%lu]: cfg: %08lx, saddr: %08lx, daddr: %08lx, tc: %08lx\n", 
		id, chan->cfg, chan->saddr, chan->daddr, chan->tc);

	return 0;
}

int dmac_chan_status (unsigned long id, unsigned long *status)
{
	if (!_g_dmac)
		return -1;

	if (id >= _g_nch)
		return -1;
	
	if (!status)
		return -1;
	
	if (_g_dmac->chan[id].isr & DMAC_INT_BSY)
		*status = DMAC_STAT_BSY;
	else if (_g_dmac->chan[id].isr & DMAC_INT_DONE)
		*status = DMAC_STAT_DONE;
	else
		*status = 0;

	return 0;
}


/* end */


