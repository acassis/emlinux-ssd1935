#include <common.h>

#include "os.h"
#include "spir.h"
#include "spi.h"

#define	SPI_DEF_WW	16
#define MIN(a, b)	((a) < (b)) ? (a) : (b)

/* internal functions */
static void spi_rx(volatile uint32_t *r, char *buf, int count, int inc);
static void spi_tx(volatile uint32_t *r, const char *buf, int count, int inc);


static spi_err spi_reset(spi_p t)
{
	int tout = SPI_TOUT;
	spir_p reg;
	
	reg = t->r;
	io_wr32(reg->ctl, SPI_CTL_ENA | SPI_CTL_RST);
	while (io_rd32(reg->ctl) & SPI_CTL_RST)
	{
		if (!tout--)
		{
			dbg("spi: rst err - time out\n");
			io_wr32(reg->ctl, 0);
			return SPI_ERR_TOUT;
		}
	}
	io_wr32(reg->ctl, 0);
	return SPI_ERR_NONE;
}


/* external functions */

spi_err spi_init(spi_p t)
{
	uint32_t id, cap;
	spir_p	reg;

	reg = t->r;
	/* check id register */
	id = io_rd32(reg->id);
	if (SPI_ID_CLASS(id) != SPI_PCI_CLASS)
	{
		dbg("spi: init err - id mismatch\n");
		return SPI_ERR_HW;
	}

	/* check cap register */
	cap = io_rd32(reg->cap);
	t->fifosize = SPI_CAP_BUF(cap);

#if 0
	/* report findings */
	dbg("spi: init info - ver=%d.%d fifo=%d slaves=%d master=%d\n",
		SPI_ID_MAJ(id), SPI_ID_MIN(id), t->fifosize, 
		SPI_CAP_SLAVE(cap) ? SPI_CAP_NSLAVE(cap) + 1 : 0, 
		!!SPI_CAP_MASTER(cap));
#endif

	/* reset */
	if (spi_reset(t))
	{
		return SPI_ERR_HW;
	}

#if 0
	if (((SPI_DEF_WW - 1) | SPI_OP_RXEN) != io_rd32(reg->op))
	{
		dbg("spi: init err - op reset value %02x\n", io_rd32(reg->op));
		return SPI_ERR_HW;
	}
#endif

	io_wr32(reg->fifo, SPI_FIFO_RWM | SPI_FIFO_TWM);

	return SPI_ERR_NONE;
}


void spi_exit(spi_p t)
{
	spi_reset(t);
}


int  spi_cs(spi_p t, int ena)
{
	spir_p		reg;
	uint32_t	v;

	reg = t->r;
	v = io_rd32(reg->master);
	if (!ena != !(v & SPI_MST_FSS))
	{
//dbg("spi: cs info - %d\n", ena);
		io_wr32(reg->master, v ^ SPI_MST_FSS);
	}
	return 0;
}


spi_err spi_cfg(spi_p t, spi_cfg_p c)
{
	uint32_t	r;
	spir_p		reg;

	reg = t->r;

	/* write op register */
	r = SPI_OP_CS(c->cs) | SPI_OP_WORD(c->word);
#ifdef SPI_AUTOCS
	if (c->burst)
	{
		r |= SPI_OP_BURST(c->burst);
		t->burst = 1;
	}
	else
	{
		t->burst = 0;
	}
#endif
	if (c->lsb_first)
	{
		r |= SPI_OP_LSB;
	}
	if (c->cs_high)
	{
		r |= SPI_OP_CSHIGH;
	}
	if (c->clk_fall)
	{
		r |= SPI_OP_CLKFALL;
	}
	if (c->clk_phase)
	{
		r |= SPI_OP_CLKPHASE;
	}
	if (!t->master)
	{
		io_wr32(reg->op, r);
		/* 4 word time */
		io_wr32(reg->slave_tout, c->word << 2);
	}
	else
	{
		int	clkdiv, frqdiv;

		io_wr32(reg->op, r | SPI_OP_MASTER);
//dbg("spi: cfg info - op = %X, %X\n", r, io_rd32(reg->op));

		frqdiv = c->baud;
		if (!frqdiv)
		{
			dbg("spi: cfg err - baud is 0\n");
			return SPI_ERR_PARM;
		}
		frqdiv <<= 2;
		if (frqdiv > c->per_clk)
		{
			clkdiv = 0;
		}
		else
		{
			frqdiv = ((c->per_clk + frqdiv - 1) / frqdiv) - 1;
			for (clkdiv = 1; frqdiv > 0; clkdiv++)
			{
				frqdiv >>= 1;
			}
		}
		if (!c->wdly)
		{
			c->wdly = 1;
		}
		if (!c->csdly)
		{
			c->csdly = 1;
		}
		/* preserve chip select */
		io_wr32(reg->master, (io_rd32(reg->master) & SPI_MST_FSS) | 
			SPI_MST_WDLY(c->wdly) | SPI_MST_CSDLY(c->csdly) | clkdiv);
	}
	t->inc = (c->word - 1) >> 3;

	return SPI_ERR_NONE;
}


static void ddump(char *s, uint8_t *b, int len)
{
	if (!len || !b) return;
	dbg(s);
	while (len--)
	{
		dbg("%02X ", *b); b++;
	}
	dbg("\n");
}


int spi_isr(spi_p t)
{
	int			st;
	spir_p		reg;
	spi_xfr_p	bp;
	register int	actual, count, rem, inc;

	reg = t->r;
	st = io_rd32(reg->sta);
#if 0
	st &= io_rd32(reg->ier);
	if (!st)
#else
	if (!(st & io_rd32(reg->ier)))
#endif
	{
		dbg("spi: isr err - no source\n");
		return 0;
	}
//dbg("isr=%X\n", st);
	io_wr32(reg->sta, st);

	/* errors */
	if (st & (SPI_INT_WOVR | SPI_INT_WUNR | SPI_INT_FOVR))
	{
		dbg("spi: isr err - %X\n", st);
		t->evt(t->ctx, SPI_EVT_ERR);
	}

	/* data xfer */
	bp = t->bp;
	if (!bp)
	{
		dbg("spi: isr err - no buf, %X\n", st);
		return 0;
	}
	inc = t->inc;
	actual = bp->actual;
	rem = bp->len - actual;

	if (st & SPI_INT_TEMT)
	{
		/*	use transmitter empty to minimize io & irq for all cases
			except slave rx-only */
		int	size;

		size = t->fifosize << inc;
		count = MIN(size, rem);
		rem -= count;

		if (bp->rx)
		{
			spi_rx(&reg->rx, bp->rx + actual, count, inc);
		}

		actual += count;
		bp->actual = actual;
		if (!rem)
		{
			io_wr32(reg->ier, 0);
			io_wr32(reg->op, io_rd32(reg->op) & ~SPI_OP_EN);
			io_wr32(reg->ctl, 0);
#ifndef SPI_AUTOCS
			if (bp->csflag & SPI_XFR_NCS)
			{
				io_wr32(reg->master, io_rd32(reg->master) & ~SPI_MST_FSS);
			}
#endif
			t->bp = 0;
//ddump("spi: rx - ", bp->rx, bp->len);
//dbg("spi: isr info - done, %X %X\n", st, io_rd32(reg->sta));
			t->evt(t->ctx, SPI_EVT_DONE);
		}
		else
		{
			count = MIN(size, rem);
			rem = count >>= t->inc;
#ifdef SPI_AUTOCS
			if (t->master && !t->burst)
			{
				/* assert chip select for rest of xfer */
				io_wr32(reg->op, (io_rd32(reg->op) & ~SPI_OP_BURST(16)) 
									| SPI_OP_BURST(rem));
			}
#endif
			if (bp->tx)
			{
				spi_tx(&reg->tx, bp->tx + actual, count, inc);
			}
			else
			{
				while (rem--)
				{
					io_wr32(reg->tx, 0);
				}
			}
		}
	}
	else if (st & (SPI_INT_RDR | SPI_INT_TOUT))
	{
		/* slave rx-only */
		union
		{
			uint8_t		*buf;
			uint16_t	*buf2;
			uint32_t	*buf4;
		}
		u;

		u.buf = bp->rx + actual;
		do
		{
			uint32_t	d;

			d = io_rd32(reg->rx);
			switch (inc)
			{
				case 0: *u.buf++ = d; break;
				case 1: *u.buf2++ = d; break;
				case 2: *u.buf4++ = d; break;
			}
			rem -= inc;
			actual += inc;
		}
		while (rem && (io_rd32(reg->sta) & SPI_INT_RDR));
		bp->actual = actual;
		if (!rem)
		{
			io_wr32(reg->ier, 0);
			io_wr32(reg->op, io_rd32(reg->op) & ~SPI_OP_EN);
			io_wr32(reg->ctl, 0);
			t->bp = 0;
//dbg("spi: isr info - done, %X\n", st);
			t->evt(t->ctx, SPI_EVT_DONE);
		}
	}

	return 1;
}


int spi_io(spi_p t, spi_xfr_p bp)
{
	spir_p		reg;
	uint32_t	ier;
	int			count;

	bp->actual = 0;
	t->bp = bp;

//ddump("spi: tx - ", bp->tx, bp->len);
	reg = t->r;
	io_wr32(reg->ctl, SPI_CTL_ENA);
	io_wr32(reg->fifo, SPI_FIFO_RRST | SPI_FIFO_TRST);

	count = t->fifosize << t->inc;
	if (bp->tx)
	{
		/* fill up tx fifo */
		ier = SPI_INT_TEMT;
		count = MIN(count, bp->len);
		spi_tx(&reg->tx, bp->tx, count, t->inc);
	}
	else
	{
		if (!bp->rx)
		{
			io_wr32(reg->ctl, 0);
			return -1;
		}
		if (t->master)
		{
			/* autoread causes more interrupts - don't use it */
			count = MIN(t->fifosize, bp->len >> t->inc);
			while (count--)
			{
				io_wr32(reg->tx, 0);
			}
			ier = SPI_INT_TEMT;
		}
		else
		{
			/* use WM only if size is sufficient */
			ier = (bp->len < (count >> 1)) ?
				SPI_INT_TOUT | SPI_INT_RDR | SPI_INT_WOVR | SPI_INT_FOVR :
				SPI_INT_TOUT | SPI_INT_RDR | SPI_INT_RWM | 
					SPI_INT_WOVR | SPI_INT_FOVR;
		}
	}
	io_wr32(reg->ier, ier);
	/* start clocking */
	ier = io_rd32(reg->op);
#ifdef SPI_AUTOCS
	if (t->master && !t->burst)
	{
		count = bp->len >> t->inc;
		if (count > 16)
		{
			count = 16;
		}
		ier &= ~SPI_OP_BURST(16);
		ier |= SPI_OP_BURST(count);
	}
#else
	if (bp->csflag & SPI_XFR_CS)
	{
		io_wr32(reg->master, io_rd32(reg->master) | SPI_MST_FSS);
	}
#endif
	if (bp->rx)
	{
		ier |= SPI_OP_RXEN | SPI_OP_EN;
	}
	else
	{
		ier &= ~SPI_OP_RXEN;
		ier |= SPI_OP_EN;
	}
//dbg("spi: io info - op=%X mst=%X ctl=%X\n", 
//ier, io_rd32(reg->master), io_rd32(reg->ctl));
	io_wr32(reg->op, ier);
	return 0;
}


static void spi_rx(volatile uint32_t *r, char *buf, int count, int inc)
{
//dbg("rx %d %d\n", count, inc);
	switch (inc)
	{
		case 0:
			while (count--)
			{
				*buf++ = io_rd32(*r);
			}
			break;

		case 1:
		{
			uint16_t	*buf2 = (uint16_t *)buf;

			count >>= 1;
			while (count--)
			{
				*buf2++ = io_rd32(*r); 
			}
			break;
		}

		case 2:
		{
			uint32_t	*buf4 = (uint32_t *)buf;

			count >>= 2;
			while (count--)
			{
				*buf4++ = io_rd32(*r);
			}
			break;
		}
	}
}


static void spi_tx(volatile uint32_t *r, const char *buf, int count, int inc)
{
//dbg("tx %d %d\n", count, inc);
	switch (inc)
	{
		case 0:
			while (count--)
			{
				io_wr32(*r, *buf++);
			}
			break;

		case 1:
		{
			uint16_t	*buf2 = (uint16_t *)buf;

			count >>= 1;
			while (count--)
			{
				io_wr32(*r, *buf2++);
			}
			break;
		}

		case 2:
		{
			uint32_t	*buf4 = (uint32_t *)buf;

			count >>= 2;
			while (count--)
			{
				io_wr32(*r, *buf4++);
			}
			break;
		}
	}
}

spi_err spi_tx_wait(spir_p r)
{
	int tout = SPI_TOUT;

	while ((io_rd32(r->sta) & 0xFF) != 0x86)
	{
		if (!tout--)
		{
			dbg("spi: tx - time out\n");
			io_wr32(r->sta, 0);
			return SPI_ERR_TOUT;
		}
	}

	return SPI_ERR_NONE;
}

