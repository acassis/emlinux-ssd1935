/*
File		sdhc.c
Author		Ambat Sasi Nair
Copyright	Solomon Systech Ltd

Secure Digital Host Controller support as per SD spec Part E1
*/

#include "os.h"

#include "sd.h"
#include "sdhcr.h"
#include "sdhc.h"


#define SDHC_INHIBIT_TOUT	100000	/* in for loops */

//#define SDHC_CMD12		/* don't use auto CMD12 */
//#define SDHC_DIV2			/* use fixed DIV2 as with TO1 */
#define SDHC_TOUT			/* calculate timeout instead of using longest */

static void	sdhc_pio(sdhc_t *, sd_cmd_p);
static int	sdhc_rst(sdhcr_t *, uint8_t);
#ifdef SDHC_TOUT
static int	sdhc_tout(sdhc_t *, int ms);
#endif

#define SDHC_IRQ_ALL	(SDHC_IRQ_ERR | SDHC_IRQ_REM | SDHC_IRQ_INS | \
			SDHC_IRQ_RD | SDHC_IRQ_WR | SDHC_IRQ_DMA | \
			SDHC_IRQ_DAT | SDHC_IRQ_CMD)

#define SDHC_CTL_REG(r)	((volatile uint32_t *)((uint32_t)(r) - 0x100))

//#define GPIO_DBG
#ifdef GPIO_DBG
#include <asm/gpio.h>
#include <linux/delay.h>
#define GPIO_DBGP	GPIO_NUM(5,5)
#undef SDHC_DIV2
#endif

int sdhc_init(sdhc_t *t)
{
	sdhcr_t	*r;
#if SDHC_IO == 32
#warning "sdhc access in 32 bit mode"
	uint32_t	cap;
#endif
	uint32_t	clk, pwr, ocr;
	int			rt;
#ifndef SDHC_DIV2
	volatile uint32_t	*ctlr;
#endif

	if (!t || !(r = (sdhcr_t *)t->r))
	{
		dbg("sdhc: init err - sdhc_t not initialized\n");
		return SDHC_RET_PARM;
	}

	/* reset */
	rt = sdhc_rst(r, SDHC_RST_ALL);
	if (rt)
	{
		return rt;
	}

	/* get base clk in 100KHz units */
#if SDHC_IO == 32
	cap = io_rd32(r->toutcaps);
	clk = (cap >> 8) & 0x3F;
#else
	clk = io_rd8(r->clkcaps) & 0x3F;
#endif
	if (clk)
	{
		t->baseclk = clk * 10;
	}

	/* get tout clk in KHz units */
#if SDHC_IO == 32
	rt = cap & 0xff;
#else
	rt = io_rd8(r->toutcaps);
#endif
	clk = rt & SDHC_TOUTCAPS_MASK;
	if (clk)
	{
		if (rt & SDHC_TOUTCAPS_MHZ)
		{
			clk *= 1000;
		}
		t->toutclk = clk;
	}

	if (!t->baseclk || !t->toutclk)
	{
		dbg("sdhc: init err - baseclk=%d00KHz or toutclk=%dKHz\n", 
			t->baseclk, t->toutclk);
		return SDHC_RET_PARM;
	}

	/* make OCR format out of supported voltages */
	ocr = 0;
#if SDHC_IO == 32
	pwr = cap >> 24;
#else
	pwr = io_rd8(r->pwrcaps);
#endif
	if (pwr & SDHC_PWRCAPS_V18)
	{
		ocr |= 3 << SD_OCR_V(17);		/* 1.7-1.9V */
	}
	if (pwr & SDHC_PWRCAPS_V30)
	{
		ocr |= 3 << SD_OCR_V(29);		/* 2.9-3.1V */
	}
	if (pwr & SDHC_PWRCAPS_V33)
	{
		ocr |= 3 << SD_OCR_V(32);		/* 3.2-3.4V */
	}
	t->ocr = ocr;

	/* set other caps */
#if SDHC_IO == 32
	pwr = (cap >> 16) & 0xff;
#else
	pwr = io_rd8(r->fncaps);
#endif
	t->fdma = (pwr & SDHC_FNCAPS_DMA) ? 1 : 0;
	t->fhspeed = (pwr & SDHC_FNCAPS_HSPEED) ? 1 : 0;
	t->fsuspend = (pwr & SDHC_FNCAPS_SUSPEND) ? 1 : 0;
	t->maxblksz = pwr & SDHC_FNCAPS_BMASK;
	t->fins = !!(io_rd32(r->sta) & SDHC_STA_INS);

	/* SSL proprietary - get version */
#ifndef SDHC_DIV2
	ctlr = SDHC_CTL_REG(r);
	clk = io_rd32(*ctlr);
	clk |= 8;
	io_wr32(*ctlr, clk);
	/* bit reserved in TO1 */
	if (io_rd32(*ctlr) & 8)
	{
		/* unspec len for remainder, BURST16, div2 */
		io_wr32(*ctlr, (clk & 0xFFFF0000) | 0x71);	
		t->ver = 1;
	}
	else
	{
		t->ver = 0;
	}
#endif
dbg("sdhc: init info - io=%X ver=%d dma=%d hspd=%d susp=%d blk=%d\n",
(uint32_t)r, t->ver, t->fdma, t->fhspeed, t->fsuspend, 1 << (t->maxblksz + 9));

	/* enable interrupts */
	clk = SDHC_IRQ_ALL;
#if SDHC_IO == 32
	clk |= SDHC_ERR_ALL_MASK << 16;
	io_wr32(r->istaen, clk);
	io_wr32(r->isigen, clk);
#else
	io_wr16(r->istaen, clk);
	io_wr16(r->isigen, clk);
	io_wr16(r->estaen, SDHC_ERR_ALL_MASK);
	io_wr16(r->esigen, SDHC_ERR_ALL_MASK);
#endif

#ifdef GPIO_DBG
gpio_direction_output(GPIO_DBGP, 0);
#endif

	return SDHC_RET_NONE;
}


void sdhc_exit(sdhc_t *t)
{
	sdhc_rst((sdhcr_t *)t->r, SDHC_RST_ALL);
}


static int _sdhc_div(int clk, int base, int *rclk)
{
	int	div;

	if (clk >= base)
	{
		div = 0;
	}
	else 
	{
		for (div = 1; div < 8; div++)
		{
			if ((base >> div) <= clk)
			{
				break;
			}
		}
		base >>= div;
		div = 1 << (div - 1);
	}
	if (rclk)
	{
		*rclk = base;
	}
	return div;
}


int sdhc_set_clk(sdhc_t *t, int clk)
/**<
set/stop clock
@param[in] t		context
@param[in] clk		in 100KHz units, 0 to stop
@remark Spec pg.68 4.2.1, 4.2.2
*/
{
	sdhcr_t		*r = (sdhcr_t *)t->r;
	int			div;
	uint32_t	baseclk, rclk;

	/* check busy if applicable */
	for (div = 0; div < SDHC_INHIBIT_TOUT; div++)
	{
		rclk = io_rd32(r->sta);
		rclk &= (SDHC_STA_CMD_INHIBIT | SDHC_STA_DAT_INHIBIT);
		if (!rclk)
		{
			break;
		}
	}
	if (div >= SDHC_INHIBIT_TOUT)
	{
		dbg("sdhc: clk err - line busy %X\n", rclk);
		return SDHC_RET_BUSY;
	}
if (div)
dbg("sdhc: cmd warn - line busy %X\n", rclk);
	/* stop clock */
#if SDHC_IO == 32
	io_wr32(r->clk, io_rd32(r->clk) & ~SDHC_CLK_ENA);
#else
	io_wr8(r->clk, io_rd8(r->clk) & ~SDHC_CLK_ENA);
#endif
	if (!clk)
	{
//dbg("sdhc: clk info - off\n");
		return SDHC_RET_NONE;
	}

	/* calc divisor */
	baseclk = t->baseclk;
#ifndef SDHC_DIV2
	if (t->ver)
	{
		volatile uint32_t	*ctlr;
		int					div3, rclk3, sel;

		sel = 0;
		div = _sdhc_div(clk, baseclk, &rclk);
		div3 = _sdhc_div(clk, baseclk / 3, &rclk3);
		if (rclk3 > rclk)
		{
			div = div3;
			rclk = rclk3;
			sel = 2;		/* select div3 */
		}
		ctlr = SDHC_CTL_REG(r);
		io_wr32(*ctlr, (io_rd32(*ctlr) & ~3) | sel);
	}
	else
#endif
	{
		/* fixed internal div by 2 in TO1 */
		div = _sdhc_div(clk, baseclk / 2, &rclk);
	}

	/* set divisor & start clk */
//dbg("sdhc: clk info - base=%d clk=%d div=%02X rclk=%d\n", 
//baseclk, clk, div, rclk);
#if SDHC_IO == 32
	io_wr32(r->clk, (io_rd32(r->clk) & 0xFFFF0000) | SDHC_CLK_INT | (div << 8));
#else
	io_wr8(r->frqdiv, div);
	io_wr8(r->clk, SDHC_CLK_INT);
#endif

	/* wait for clk to stabilize */
	for (div = 0; div < 10000; div++)
	{
#if SDHC_IO == 32
		if (io_rd32(r->clk) & SDHC_CLK_STABLE)
#else
		if (io_rd8(r->clk) & SDHC_CLK_STABLE)
#endif
		{
			break;
		}
	}
	if (div >= 10000)
	{
		dbg("sdhc: clk err - stabilize timeout\n");
		return SDHC_RET_TOUT;
	}

	/* output clk */
#if SDHC_IO == 32
	io_wr32(r->clk, io_rd32(r->clk) | SDHC_CLK_ENA);
#else
	io_wr8(r->clk, io_rd8(r->clk) | SDHC_CLK_ENA);
#endif
	return SDHC_RET_NONE;
}


int  sdhc_set_pwr(sdhc_t *t, int pwr)
/**<
set/stop power
@param[in] t	context
@param[in] pwr	power bitmask
*/
{
	sdhcr_t	*r = (sdhcr_t *)t->r;
	int	spwr;

	if (!pwr)
	{
//dbg("sdhc: pwr info - off\n");
#if SDHC_IO == 32
		io_wr32(r->host, io_rd32(r->host) & ~0xFF00);
#else
		io_wr8(r->pwr, 0);
#endif
		return SDHC_RET_NONE;
	}

#if SDHC_IO == 32
	spwr = io_rd32(r->toutcaps) >> 24;
#else
	spwr = io_rd8(r->pwrcaps);
#endif
	if (pwr == -1)
	{
		/* set max */
		pwr = (spwr & SDHC_PWRCAPS_V33) ? SDHC_PWR_V33 :
				(spwr & SDHC_PWRCAPS_V30) ? SDHC_PWR_V30 : SDHC_PWR_V18;
	}
	else
	{
		if (pwr & (3 << SD_OCR_V(17)))
		{
			if (!(spwr & SDHC_PWRCAPS_V18))
			{
				goto lp_err;
			}
			pwr = SDHC_PWR_V18 | SDHC_PWR_ENA;
		}
		else if (pwr & (3 << SD_OCR_V(29)))
		{
			if (!(spwr & SDHC_PWRCAPS_V30))
			{
				goto lp_err;
			}
			pwr = SDHC_PWR_V30 | SDHC_PWR_ENA;
		}
		else if (pwr & (3 << SD_OCR_V(32)))
		{
			if (!(spwr & SDHC_PWRCAPS_V33))
			{
				goto lp_err;
			}
			pwr = SDHC_PWR_V33 | SDHC_PWR_ENA;
		}
		else
		{
			goto lp_err;
		}
	}

//dbg("sdhc: pwr info - pwr = %02X ", pwr);
#if SDHC_IO == 32
	spwr = io_rd32(r->host) & ~0xFF00;
	io_wr32(r->host, spwr);
	io_wr32(r->host, spwr | (pwr << 8));
//dbg("%02X\n", (io_rd32(r->host) >> 8) & 0xFF);
#else
	io_wr8(r->pwr, 0);
	io_wr8(r->pwr, pwr);
//dbg("%02X\n", io_rd8(r->pwr));
#endif
	return SDHC_RET_NONE;

lp_err:
	dbg("sdhc: pwr err - no supported power range %X\n", pwr);
	return SDHC_RET_PARM;
}


void  sdhc_set_led(sdhc_t *t, int on)
/**<
turn on/off led
@param[in] t	context
@param[in] on	turn on/off
*/
{
	sdhcr_t	*r = (sdhcr_t *)t->r;
	uint32_t	v;

#if SDHC_IO == 32
	v = io_rd32(r->host);
#else
	v = io_rd8(r->host);
#endif
	if (!on != !(v & SDHC_HOST_LED))
	{
#if SDHC_IO == 32
		io_wr32(r->host, v ^ SDHC_HOST_LED);
#else
		io_wr8(r->host, v ^ SDHC_HOST_LED);
#endif
	}
}


void  sdhc_set_width(sdhc_t *t, int wide)
/**<
set bus width
@param[in] t	context
@param[in] wide	4 or 1 bit
*/
{
	sdhcr_t	*r = (sdhcr_t *)t->r;
	uint32_t		v;

//dbg("sdhc: width %d\n", wide);
#if SDHC_IO == 32
	v = io_rd32(r->host);
#else
	v = io_rd8(r->host);
#endif
	if (!(v & SDHC_HOST_WIDE) != !wide)
	{
		uint32_t	sta;

#if SDHC_IO == 32
		sta = io_rd32(r->istaen);
		io_wr32(r->istaen, 0);
#else
		sta = io_rd16(r->istaen);
		io_wr16(r->istaen, 0);
#endif

		v ^= SDHC_HOST_WIDE;
#if SDHC_IO == 32
		io_wr32(r->host, v);
#else
		io_wr8(r->host, v);
#endif

#if SDHC_IO == 32
		io_wr32(r->istaen, sta);
#else
		io_wr16(r->istaen, sta);
#endif
	}
}


#define SDHC_TOUT_CMD	SYS_MS2LOOP(1)


int  sdhc_cmd(sdhc_t *t, sd_cmd_t *c)
/*
*/
{
	sdhcr_t	*r = (sdhcr_t *)t->r;
	int			i;
	uint16_t	v;
#if SDHC_IO == 32
	uint16_t	mode = 0;
#endif
	uint16_t	cmd;
	uint8_t		f;

	if (!t->fins)
	{
		c->rt = SDHC_RET_REM;
		return SDHC_RET_REM;
	}

	f = c->flag;
	cmd = (c->cmd << 8);
	switch (f & SDCMD_F_RMASK)
	{
		case SDCMD_F_R1:
			cmd |= SDHC_CMD_RSP_48 | SDHC_CMD_IDX | SDHC_CMD_CRC;
			break;

		case SDCMD_F_R1B:
			cmd |= SDHC_CMD_RSP_BUSY | SDHC_CMD_IDX | SDHC_CMD_CRC 
					| SDHC_CMD_DAT;
			break;

		case SDCMD_F_R2:
			cmd |= SDHC_CMD_RSP_136 | SDHC_CMD_CRC;
			break;

		case SDCMD_F_R3:
			cmd |= SDHC_CMD_RSP_48;
			break;
	}
	if ((cmd >> 8) == 12)
	{
		cmd |= SDHC_CMD_TYP_ABORT;
	}
	else if ((cmd >> 8) == 52)
	{
		uint32_t	arg = c->arg;

		if ((arg & ((1 << 31) | (7 << 28))) == (1 << 31))
		{
			/* write to CCCR */
			switch (arg & (0x1FFFF << 9))
			{
				case (6 << 9):
					/* some I/O function is aborted */
					if (arg & 7)
					{
						cmd |= SDHC_CMD_TYP_ABORT;
					}
					break;

				case (12 << 9):
					/* some I/O function is suspended */
					if (arg & 2)
					{
						cmd |= SDHC_CMD_TYP_SUSPEND;
					}
					break;

				case (13 << 9):
					/* some I/O function is resumed */
					if (arg & 7)
					{
						cmd |= SDHC_CMD_TYP_RESUME;
					}
					break;
			}
		}
	}

	/* check if line is busy */
	if ((cmd & SDHC_CMD_TYP_MASK) != SDHC_CMD_TYP_ABORT && 
		((cmd & SDHC_CMD_RSP_MASK) == SDHC_CMD_RSP_BUSY ||
			(f & SDCMD_F_DAT)))
	{
		v = SDHC_STA_CMD_INHIBIT | SDHC_STA_DAT_INHIBIT;
	}
	else
	{
		v = SDHC_STA_CMD_INHIBIT;
	}
	for (i = 0; i < SDHC_INHIBIT_TOUT; i++)
	{
		if (!(v & io_rd32(r->sta)))
		{
			break;
		}
	}
	if (i >= SDHC_INHIBIT_TOUT)
	{
		/* data line busy timeout */
		dbg("sdhc: cmd err - line busy %X\n", v);
		c->rt = SDHC_RET_BUSY;
		return SDHC_RET_BUSY;
	}
if (i)
dbg("sdhc: cmd warn - line busy %X\n", v);

	/* prepare data transfer */
	if (f & SDCMD_F_DAT)
	{
		sd_dat_p	d;

		cmd |= SDHC_CMD_DAT;
		d = c->dat;
		d->actual = 0;
		if (!d->count || !d->blk)
		{
			dbg("sdhc: cmd err - data expected\n");
			c->rt = SDHC_RET_PARM;
			return SDHC_RET_PARM;
		}

		if (!(f & SDCMD_F_WR)) 
		{
			v = SDHC_MODE_RD;
		}

		if (f & SDCMD_F_MULTI)
		{
			v |= SDHC_MODE_MULTI | SDHC_MODE_COUNT;
#if SDHC_IO == 32
			/* write count & block later */
#else
			io_wr16(r->count, d->count);
#endif
#ifndef SDHC_CMD12
			if (f & SDCMD_F_STOP)
			{
				v |= SDHC_MODE_AUTOSTOP;
			}
#endif
		}

		/* transfer data to FIFO/DMA */
		if (f & SDCMD_F_DMA)
		{
			v |= SDHC_MODE_DMA;
//dbg("sdhc: cmd info - dma=%08X\n", (uint32_t)d->buf);
			io_wr32(r->dma, (uint32_t)d->buf);
		}

		/* set block size & transfer mode registers 
			use 512K setting to reduce dma boundary interrupt frequency
		*/
#if SDHC_IO == 32
		io_wr32(r->blk, SDHC_BLK_ALIGN_512K | d->blk | (d->count << 16));
		mode = v;
#else
		io_wr16(r->blk, SDHC_BLK_ALIGN_512K | d->blk);
		io_wr16(r->mode, v);
#endif

//dbg("sdhc: cmd info - cmd=%04X arg=%08X mode=%04X blk=%d count=%d\n", 
//cmd, c->arg, v, d->blk, d->count);
	}
//else dbg("sdhc: cmd info - cmd=%04X arg=%08X mode=%04X\n", cmd, c->arg, v);

#ifdef SDHC_TOUT
	/* set data/busy timeout register */
	if ((cmd & SDHC_CMD_DAT) || 
		(cmd & SDHC_CMD_RSP_MASK) == SDHC_CMD_RSP_BUSY)
	{
		uint32_t	tout;

		tout = c->tout;
		if (!tout)
		{
			tout = 1000;	/* 1s default */
		}
		sdhc_tout(t, tout);
	}
#endif
	c->rt = SDHC_RET_NONE;

	/* set arg, cmd registers & execute */
	io_wr32(r->arg, c->arg);
	t->cmd = c;
#if SDHC_IO == 32
	io_wr32(r->mode, mode | (cmd << 16));
#else
	io_wr16(r->cmd, cmd);
#endif

	return SDHC_RET_NONE;
}


void  sdhc_flush(sdhc_t *t)
{
	sd_cmd_t	*c;

//dbg("flush dma=%08X\n", io_rd32(((sdhcr_t *)t->r)->dma));
	sdhc_rst((sdhcr_t *)t->r, SDHC_RST_CMD | SDHC_RST_DAT);
	c = t->dat;
	if (c)
	{
		c->rt = SDHC_RET_REM;
		t->dat = 0;
		t->evt(t->ctx, c);
	}
	c = t->cmd;
	if (c)
	{
		c->rt = SDHC_RET_REM;
		t->cmd = 0;
		t->evt(t->ctx, c);
	}
}


void  sdhc_ioirq_ena(sdhc_t *t, int ena)
{
	sdhcr_t	*r = (sdhcr_t *)t->r;
	uint32_t	sta;

#if SDHC_IO == 32
#define _iord	io_rd32
#define _iowr	io_wr32
#else
#define _iord	io_rd16
#define _iowr	io_wr16
#endif
	sta = _iord(r->istaen);
	if (!(sta & SDHC_IRQ_IO) != !ena)
	{
		_iowr(r->istaen, sta ^ SDHC_IRQ_IO);
		_iowr(r->isigen, sta ^ SDHC_IRQ_IO);
	}
}


static void sdhc_abort(sdhcr_p r)
{
	sdhc_rst(r, SDHC_RST_CMD | SDHC_RST_DAT);
	io_wr32(r->arg, 0);
	io_wr32(r->mode, (SDHC_C_ABORT | SDHC_CMD_TYP_ABORT |
		SDHC_CMD_RSP_BUSY | SDHC_CMD_IDX | SDHC_CMD_CRC) << 16);
}


static void sdhc_dmawait(sd_dat_p d, sdhcr_p r)
{
	uint32_t adr, adr2;

	adr = (uint32_t)d->buf + d->actual * d->blk;
	if ((adr2 = io_rd32(r->dma)) != adr) 
	{
		uint32_t adr1, tout;

		/* dma has not reached expected address */
		tout = 0;
		while ((adr1 = io_rd32(r->dma)) != adr)
		{
			if (adr1 != adr2)
			{
				/* address changed ie. progress - reset timeout */
				tout = 0;
				adr2 = adr1;
			}
			else
			{
				tout++;
				if (tout > 200)
				{
					/* give up, adjust actual to dma address */
					adr2 = (adr1 - (uint32_t)d->buf) / d->blk;
dbg("dmat=%X/%X %d/%d rem%d\n", 
adr1, adr, adr2, d->actual, io_rd32(r->blk) >> 16);
					d->actual = adr2;
					return;
				}
				/* keep off the bus - WARNING OS CALL */
				udelay(5);
			}
		}
		/* dma has caught up */
dbg("dmao=%X %X\n", adr2, adr);
	}
}


int  sdhc_isr(sdhc_t *t)
{
	sdhcr_t	*r = (sdhcr_t *)t->r;
	sd_cmd_t	*c;
#if SDHC_IO == 32
	uint32_t	sta;

	sta = io_rd32(r->ista);
	io_wr32(r->ista, sta);		/* clr ista & esta irqs */
	if (!(sta & 0xFFFF))
#else
	uint16_t	sta;

	sta = io_rd16(r->ista);
	io_wr16(r->ista, sta);		/* clr irqs */
	if (!sta)
#endif
	{
		dbg("sdhc: isr warn - why r we here then?\n");
		return 0;
	}
//dbg("sdhc: isr info - ista=%04X sta=%08X\n", sta, io_rd32(r->sta));

	/* insertion/removal */
	if (sta & (SDHC_IRQ_INS | SDHC_IRQ_REM))
	{
		uint32_t	psta;
		sd_cmd_t	*d;

		/* remove pending commands */
		d = t->dat;
		if (d)
		{
			d->rt = SDHC_RET_REM;
			t->dat = 0;
		}
		c = t->cmd;
		if (c)
		{
			c->rt = SDHC_RET_REM;
			t->cmd = 0;
		}

		/* clear FIFO & report event */
#if SDHC_IO == 32
		psta = io_rd32(r->clk); 
		io_wr32(r->clk, psta & ~SDHC_CLK_ENA);
		io_wr32(r->clk, psta | SDHC_CLK_ENA);
#else
		psta = io_rd8(r->clk); 
		io_wr8(r->clk, psta & ~SDHC_CLK_ENA);
		io_wr8(r->clk, psta | SDHC_CLK_ENA);
#endif
		sdhc_rst(r, SDHC_RST_CMD | SDHC_RST_DAT);
#if SDHC_IO == 32
		io_rd32(r->clk, psta); 
#else
		io_wr8(r->clk, psta); 
#endif

		psta = io_rd32(r->sta);
		if (psta & SDHC_STA_INS)
		{
			t->fins = 1;
/*	if the platform use TF card use "t->fwp = (psta & SDHC_STA_WP) ? 0 : 1;"
	if the platform usr SD card use "t->fwp = (psta & SDHC_STA_WP) ? 1 : 0;"	*/
#if defined CONFIG_ACCIO_CM5208 || CONFIG_ACCIO_LITE || CONFIG_LUMOS_WE8623_P0
			t->fwp = (psta & SDHC_STA_WP) ? 0 : 1;
#else
			/* tmp handle for the wrong socket */
 			t->fwp = (psta & SDHC_STA_WP) ? 1 : 0;
#endif

//dbg("sdhc: isr info - card inserted wp=%d\n", t->fwp);
			t->evt(t->ctx, SDHC_EVT_INS);
		}
		else
		{
			t->fins = 0;
//dbg("sdhc: isr info - card removed\n");
			t->evt(t->ctx, SDHC_EVT_REM);
		}

		/* complete pending commands */
		if (d)
		{
			t->evt(t->ctx, d);
		}
		if (c)
		{
			t->evt(t->ctx, c);
		}

		return 1;
	}

	if (!t->fins)
	{
dbg("sdhc: isr warn - no card sta=%04X\n", sta);
		return 1;
	}

	if (sta & SDHC_IRQ_ERR)
	{
		uint16_t	err;
		uint8_t		a12err;

#if SDHC_IO == 32
		err = sta >> 16;
		a12err = (err & SDHC_ERR_A12) ? (uint8_t)io_rd32(r->a12err) : 0;
#else
		err = io_rd16(r->esta);
		a12err = (err & SDHC_ERR_A12) ? io_rd8(r->a12err) : 0;
		io_wr16(r->esta, err);	/* clr errs */
#endif

#ifdef GPIO_DBG
{
uint32_t k;
k = err & (SDHC_ERR_DAT_CRC | SDHC_ERR_DAT_TOUT);
if (k && k != (SDHC_ERR_DAT_CRC | SDHC_ERR_DAT_TOUT))
{
	gpio_set_value(GPIO_DBGP, 1);
	udelay(1);
	gpio_set_value(GPIO_DBGP, 0);
}
}
#endif
		dbg("sdhc: isr err - esta=%04X a12=%02X sta=%04X\n", 
			err, a12err, sta);

		/* complete pending commands on error */
		if (err & SDHC_ERR_RSP_MASK)
		{
			sta &= ~SDHC_IRQ_CMD;
			sdhc_rst(r, SDHC_RST_CMD);
			c = t->cmd;
			if (c)
			{
				t->cmd = 0;
				if (!c->rt)
				{
					if ((err & SDHC_ERR_RSP_MASK) == SDHC_ERR_RSP_TOUT)
					{
						/* timeout - card state not changed */
						c->rt = SDHC_RET_CMD_TOUT;
					}
					else
					{
						c->rt = SDHC_RET_CMD_LINE;
					}
//dbg("sdhc: isr err - rsp cmd%d\n", err, c->cmd);
					if (c->flag & SDCMD_F_DAT)
					{
						/* abort to clear errors */
						sdhc_abort(r);
						/* sasi - 090420 added line */
						t->cmd = c;
						return 1;
					}
				}
				/* previous abort */
//dbg("aborted in cmd, err_resp\n");
				t->evt(t->ctx, c);
			}
			else
			{
				c = t->dat;
				if (c && c->rt)
				{
//dbg("aborted in dat, err_resp\n");
					t->dat = 0;
					t->evt(t->ctx, c);
				}
				else
				{
//dbg("sdhc: isr warn - unexpected ERR_RSP %X "
//"cmd=x%X arg=%X\n", err, io_rd32(r->cmd), io_rd32(r->arg));
				}
			}
		}
		if (err & SDHC_ERR_DAT_MASK)
		{
			c = t->dat;
			if (c)
			{
				sd_dat_p	d;

				switch (err & SDHC_ERR_DAT_MASK)
				{
					case SDHC_ERR_DAT_TOUT:
#if 0
						if (sta & SDHC_IRQ_DAT)
						{
							goto l_ignore_err;
						}
#endif
						c->rt = SDHC_RET_DAT_TOUT;
						break;

					case (SDHC_ERR_DAT_TOUT | SDHC_ERR_DAT_CRC):
						/* proprietary */
						c->rt = SDHC_RET_DAT_OVR;
						break;

					default:
						c->rt = SDHC_RET_DAT_LINE;
						break;
				}
				sta &= ~(SDHC_IRQ_DAT | SDHC_IRQ_RD | SDHC_IRQ_WR);
				d = c->dat;
				d->actual = !(c->flag & SDCMD_F_MULTI) ? 0 : d->count -
#if SDHC_IO == 32
					(io_rd32(r->blk) >> 16);
#else
					io_rd16(r->count);
#endif
/* wait for DMA to complete */
if (c->flag & SDCMD_F_DMA)
{
dbg("sdhc: isr err - cmd=%d arg=%08X blk=%d %d/%d\n", 
c->cmd, c->arg, d->blk, d->actual, d->count);
	sdhc_dmawait(d, r);
}
				sdhc_rst(r, SDHC_RST_CMD | SDHC_RST_DAT);
#ifdef SDHC_CMD12
				if (c->flag & SDCMD_F_STOP)
				{
					io_wr32(r->arg, 0);
					io_wr32(r->mode, (SDHC_C_ABORT | SDHC_CMD_TYP_ABORT |
						SDHC_CMD_RSP_BUSY | SDHC_CMD_IDX | SDHC_CMD_CRC) << 16);
					goto l_ignore_err;
				}
#endif
				t->dat = 0;
				t->evt(t->ctx, c);
#ifdef SDHC_CMD12
l_ignore_err:;
#endif
			}
			else
			{
				sdhc_rst(r, SDHC_RST_DAT);
				c = t->cmd;
				if (c && c->rt)
				{
					t->cmd = 0;
					t->evt(t->ctx, c);
				}
				else
				{
//dbg("sdhc: isr warn - unexpected ERR_DAT %X "
//"cmd=x%X arg=%X\n", err, io_rd32(r->cmd), io_rd32(r->arg));
				}
			}
		}
		if (a12err)
		{
			/* send abort manually */
			c = t->dat;
			if (c && !c->rt)
			{
				c->rt = SDHC_RET_ABORT;
				sdhc_abort(r);
				return 1;
			}
		}
	}

	/* handle cmd completion */
	if (sta & SDHC_IRQ_CMD)
	{
		c = t->cmd;
		if (!c)
		{
			c = t->dat;
			if (c && c->rt)
			{
				/* previous abort */
//dbg("aborted in dat, irq_cmd\n");
				t->dat = 0;
				t->evt(t->ctx, c);
			}
			else
			{
#ifdef SDHC_CMD12
				if (c)
				{
					c->flag &= ~(SDCMD_F_DMA | SDCMD_F_STOP);
				}
				else
#endif
				dbg("sdhc: isr warn - unexpected IRQ_CMD, "
					"cmd=x%X arg=%X\n", io_rd16(r->cmd), io_rd32(r->arg));
			}
		}
		else
		{
			t->cmd = 0;
			if (c->rt)
			{
				/* previous abort */
//dbg("aborted in cmd, irq_cmd\n");
				t->evt(t->ctx, c);
			}
			else if ((c->flag & SDCMD_F_RMASK) == SDCMD_F_R2)
			{
				uint8_t		rsp[16];
				uint32_t	*pr = (uint32_t *)rsp;
				uint8_t		*d = c->dat->buf;
				volatile uint32_t	*s = r->rsp;
				int		i;
				static const char rspmap[16] = 
				{
					11, 12, 13, 14,
					7, 8, 9, 10,
					3, 4, 5, 6,
					0, 0, 1, 2
				};
				uint8_t		*rm = (uint8_t *)rspmap;

				for (i = 0; i < 4; i++, pr++, s++)
				{
					*pr = io_rd32(*s);
				}
				for (i = 0; i < 16; i++, d++, rm++)
				{
					*d = rsp[*rm];
				}
				t->evt(t->ctx, c);
			}
			else
			{
				c->rsp = io_rd32(r->rsp[0]);
				/* for R1/R6, check response status */
				if (((c->flag & 3) == SDCMD_F_R1) && ((c->cmd & ~1) != 52))
				{
					uint32_t	v;

					v = (c->cmd == 3) ? SD_S_R6_ERR_MASK : SD_S_R1_ERR_MASK;
					if (c->rsp & v)
					{
						/* error set in response */
						if (c->flag & SDCMD_F_DAT)
						{
							/* if there is data, abort */
						}
//dbg("sdhc: isr warn - R1/R6 %08X\n", c->rsp);
					}
				}

				if (c->flag & SDCMD_F_DAT)
				{
					t->dat = c;
					if ((c->flag & (SDCMD_F_WR | SDCMD_F_DMA)) == SDCMD_F_WR)
					{
						/* attempt 1st write to FIFO */
						sdhc_pio(t, c);
						sta &= ~SDHC_IRQ_WR;
					}
				}
				else
				{
					t->evt(t->ctx, c);
				}
			}
		}
	}

	if (sta & SDHC_IRQ_DMA)
	{
		io_wr32(r->dma, io_rd32(r->dma));
	}

	/* handle dat completion */
	c = t->dat;
	if (sta & (SDHC_IRQ_WR | SDHC_IRQ_RD))
	{
		if (c)
		{
			if (c->flag & SDCMD_F_DMA)
			{
//dbg("sta=%08X r/w irq in dma mode\n", sta);
			}
			else
			{
				sdhc_pio(t, c);
			}
		}
//else dbg("sdhc: isr warn - no buf data\n");
	}
	if (sta & SDHC_IRQ_DAT)
	{
		/* data xfer completed */
		if (c)
		{
			sd_dat_p	d;

			d = c->dat;
			if (c->flag & SDCMD_F_DMA)
			{
				d->actual = d->count;
/* wait for DMA to complete */
if (c->flag & SDCMD_F_DMA)
{
	sdhc_dmawait(d, r);
}
			}
			if (c->flag & SDCMD_F_STOP)
			{
//dbg("sdhc: isr info - a12 R1b %08X\n", io_rd32(r->rsp[3]));
#ifdef SDHC_CMD12
				io_wr32(r->arg, 0);
				io_wr32(r->mode, (SDHC_C_ABORT | SDHC_CMD_TYP_ABORT |
					SDHC_CMD_RSP_BUSY | SDHC_CMD_IDX | SDHC_CMD_CRC) << 16);
#endif
			}
#ifdef SDHC_CMD12
else {
#endif
			t->dat = 0;
			t->evt(t->ctx, c);
#ifdef SDHC_CMD12
}
#endif
		}
		else
		{
			dbg("sdhc: isr warn - unexpected IRQ_DAT, "
				"cmd=x%X arg=%X\n", io_rd16(r->cmd), io_rd32(r->arg));
		}
	}

	/* handle io card */
	if (sta & SDHC_IRQ_IO)
	{
		t->evt(t->ctx, SDHC_EVT_IO);
	}
	return 1;
}


static int  sdhc_rst(sdhcr_t *r, uint8_t rst)
/**<
reset cmd and/or data lines
@param[in] r	register base
@param[in] rst	SDHC_RST_XXX bitmask
*/
{
	int	i;

#if SDHC_IO == 32
	io_wr32(r->clk, (io_rd32(r->clk) & 0xFFFFFF) | (rst << 24));
#else
	io_wr8(r->rst, rst);
#endif
	for (i = 0; i < 100; i++)
	{
#if SDHC_IO == 32
		if (!(io_rd32(r->clk) >> 24))
#else
		if (!io_rd8(r->rst))
#endif

		{
			/* set maximum tout */
#if SDHC_IO == 32
			io_wr32(r->clk, (io_rd32(r->clk) & ~0xFF0000) | (14 << 16));
#else
			io_wr8(r->tout, 14);
#endif
			return SDHC_RET_NONE;
		}
	}
	dbg("sdhc: rst err - reset timeout\n");
	return SDHC_RET_TOUT;
}


#ifdef SDHC_TOUT
static int  sdhc_tout(sdhc_t *t, int ms)
/*
	shortest possible tout = (2^13)/(63MHz) = 130us
	so we use milliseconds
*/
{
	uint32_t	cycles;
	uint8_t		count;
	sdhcr_t	*r = (sdhcr_t *)t->r;

	cycles = t->toutclk * ms;
	cycles >>= 13;
	for (count = 0; cycles && count < 14; count++)
	{
		cycles >>= 1;
	}
//dbg("sdhc: tout info - ms=%d clk=%dKHz count=%d\n", 
//ms, t->toutclk, count);
#if SDHC_IO == 32
	io_wr32(r->clk, (io_rd32(r->clk) & ~0xFF0000) | (count << 16));
#else
	io_wr8(r->tout, count);
#endif
	return 0;
}
#endif


static void  sdhc_pio(sdhc_t *t, sd_cmd_p c)
/**<
programmed IO for read/write
@param[in]		t	context
@param[in/out]	c	command & data
*/
{
	uint16_t	k, count, blen;
	char		wr;
	volatile uint32_t	*port, *sta;
	uint32_t	*d;
	sd_dat_p	dat;

	wr = c->flag & SDCMD_F_WR;
	dat = c->dat;
	blen = dat->blk;
	count = dat->actual;
	d = (uint32_t *)(dat->buf + blen * count);
	count = dat->count - count;
	if (!count)
	{
//dbg("sdhc: xfer warn: no more data\n");
		return;
	}
	port = &((sdhcr_t *)t->r)->port;
	sta = &((sdhcr_t *)t->r)->sta;

	/* for number of blocks */
	for (k = 0; k < count; k++)
	{
		int	i;

		if (wr)
		{
			if (!(io_rd32(*sta) & SDHC_STA_WRDY))
			{
				/* FIFO full */
//if (!k) dbg("no wr ena %08X\n", io_rd32(*sta));
				break;
			}
			for (i = 0; i < blen; i += 4, d++)
			{
				io_wr32(*port, *d);
			}
//dbg("sdhc: xfer info - wrote %d\n", blen);
		}
		else
		{
			if (!(io_rd32(*sta) & SDHC_STA_RRDY))
			{
				/* FIFO empty */
//if (!k) dbg("no rd ena %08X\n", io_rd32(*sta));
				break;
			}
			for (i = 0; i < blen; i += 4, d++)
			{
				*d = io_rd32(*port);
			}
//dbg("sdhc: xfer info - read %d\n", blen);
		}
	}
	dat->actual += k;
}

