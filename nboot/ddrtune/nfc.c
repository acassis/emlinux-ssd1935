#include <stdint.h>
#include "nand.h"
#include "nfcr.h"
#include "nfc.h"

#define io_rd32(r)		(r)
#define io_wr32(r, d)	((r) = (d))

static uint32_t cap;

static void  nfc_rst(nfc_p t)
{
	nfcr_p		r;

	r = t->r;
	io_wr32(r->CFGR1, NFC_CFGR1_RST | NFC_CFGR1_EN);
	while (1)
	{
		uint32_t	d;

		d = io_rd32(r->CFGR1);
		if ((d & (NFC_CFGR1_RST | NFC_CFGR1_EN)) == NFC_CFGR1_EN)
		{
			break;
		}
	}
}


void  nfc_init(nfc_p t)
{
	uint32_t	d;
	nfcr_p		r;

	r  = t->r;

	/* check id */
	d = io_rd32(r->IDR);
	if (NFC_IDR_CLID(d) != NFC_PCI_CLASS)
	{
		;
	}

	cap = io_rd32(r->CAPR);
	/* enable module,  reset and check reset value */
	nfc_rst(t);
	io_wr32(r->FSCR, (NAND_CMD_STAT << 16) | 0xFF);
	io_wr32(r->CFGR2, 0x33031101);
}


void  nfc_exit(nfc_p t)
{
	nfcr_p	r;

	r = t->r;
	nfc_rst(t);
	io_wr32(r->CFGR1, 0);
}



int  nfc_cfg(nfc_p t, nand_geo_t *c)
{
	static const uint8_t _pgsz[8] = {0, 1, 2, 2, 3, 4, 6, 7};
	nfcr_p	r;

	r = t->r;
	io_wr32(r->CFGR1, 
		((c->size - c->page + 20 - 1) << 24) | (c->page << 16) | 
		(((c->block - c->page) - 3) << 12) |
		(c->bit16 << 11) | (_pgsz[c->page - 8] << 8) |
		NFC_CFGR1_SEQ | NFC_CFGR1_EN);
	t->page = c->page;

	if (t->ecc)
	{
		uint32_t	ecc;

		switch (c->page)
		{
			case 9:
				ecc = NFC_ECC_MODEH | NFC_ECC_HOFS(0) | NFC_ECC_HRST;
				break;

			case 11:	//2K
				
				ecc = NFC_ECC_MODET4;
				io_wr32(r->TECC4, NFC_TECC4_RST | NFC_TECC4_REDEC);
				break;

			case 12:	//4K
				
				if(c->spare==128)
				{	
					//case of 4K page + 128 oob				
					ecc = NFC_ECC_MODET4;
					io_wr32(r->TECC4, NFC_TECC4_RST | NFC_TECC4_REDEC);					
				}
				else	
				{								
					//case of 4K page + 218 oob				
					ecc = NFC_ECC_MODET8;
					io_wr32(r->TECC8, NFC_TECC8_RST | NFC_TECC8_SIZE(23));
				}	
				break;

			default:
				t->ecc = 0;
				return 0;
		}
		io_wr32(r->ECTRL, ecc);
	}
	return 0;
}

void  nfc_nand_rst(nfc_p t)
{
	nfcr_p		r;
	uint32_t	d;

	r = t->r;
	io_wr32(r->CTLR, NFC_CTLR_OP_RST | NFC_CTLR_GO);
	while (!((d = io_rd32(r->ISR)) & NFC_ISR_CMP))
	{
		;
	}
	io_wr32(r->ISR, d);
}


void  nfc_nand_id(nfc_p t, nand_id_t *id)
{
	nfcr_p		r;
	uint32_t	d;

	r = t->r;
	io_wr32(r->CMDR, NAND_CMD_ID);
	io_wr32(r->ADDR1, 0);
	io_wr32(r->HADDR1, 0);
	io_wr32(r->CTLR, NFC_CTLR_COUNT(5) | NFC_CTLR_OP_ID | NFC_CTLR_GO);
	while (!((d = io_rd32(r->ISR)) & NFC_ISR_CMP))
	{
		;
	}
	io_wr32(r->ISR, d);
	*id = *(nand_id_t *)r->BUF;
}


int  nfc_nand_read(nfc_p t, uint32_t page)
{
	nfcr_p		r;
	uint32_t	d;
	int			pgshf;

	r = t->r;
	pgshf = t->page + 1;
	io_wr32(r->CMDR, (NAND_CMD_READ_MLC << 8) | NAND_CMD_READ);
	io_wr32(r->ADDR1, page << pgshf);
	io_wr32(r->HADDR1, page >> (32 - pgshf));
	io_wr32(r->CTLR, 
		NFC_CTLR_CYCLE(pgshf < 12 ? 1 : 2) | NFC_CTLR_COUNT(1) | 
		NFC_CTLR_OP_READ | NFC_CTLR_GO | 
		(t->ecc ? NFC_CTLR_ECCEN | NFC_CTLR_ECCAUTO : 0));

	if ((cap & 0x4) > 1)
		while (((d = io_rd32(r->ISR)) & NFC_ISR_CMP)
					!= NFC_ISR_CMP);
	else
		while (((d = io_rd32(r->ISR)) & (NFC_ISR_CMP | NFC_ISR_DRDY))
					!= (NFC_ISR_CMP | NFC_ISR_DRDY));

	io_wr32(r->ISR, d);
	if (d & NFC_ISR_ERR)
	{
		return NFC_ERR_CMD;
	}
	if (d & NFC_ISR_ECCERR)
	{
		return NFC_ERR_DAT;
	}
	return 0;
}


int  nfc_nand_prog(nfc_p t, uint32_t page)
{
	nfcr_p		r;
	uint32_t	d;
	int			pgshf;

	r = t->r;
	pgshf = t->page + 1;
	io_wr32(r->FSR, 0xFF);
	io_wr32(r->CMDR, (NAND_CMD_PROG << 8) | NAND_CMD_SEQ);
	io_wr32(r->ADDR1, page << pgshf);
	io_wr32(r->HADDR1, page >> (32 - pgshf));
	io_wr32(r->CTLR, 
		NFC_CTLR_CYCLE(2) | NFC_CTLR_COUNT(1) | NFC_CTLR_STAT |
		NFC_CTLR_OP_PROG | NFC_CTLR_GO |
		(t->ecc ? NFC_CTLR_ECCEN | NFC_CTLR_ECCAUTO : 0));
#if 0
	while (((d = io_rd32(r->ISR)) & (NFC_ISR_CMP | NFC_ISR_DRDY))
				!= (NFC_ISR_CMP | NFC_ISR_DRDY))
#else
	while (((d = io_rd32(r->ISR)) & NFC_ISR_CMP)
				!= NFC_ISR_CMP)
#endif
	{
		;
	}
	io_wr32(r->ISR, d);
	/* no data ready - so we wait for status instead */
	while (io_rd32(r->FSR) == 0xFF)
	{
		;
	}

	if (d & NFC_ISR_ERR)
	{
		return NFC_ERR_CMD;
	}
	if (d & NFC_ISR_ECCERR)
	{
		return NFC_ERR_DAT;
	}
	return io_rd32(r->FSR);
}


int  nfc_nand_erase(nfc_p t, uint32_t page)
{
	nfcr_p		r;
	uint32_t	d;
	int			pgshf;

	r = t->r;
	pgshf = t->page + 1;
	io_wr32(r->FSR, 0xFF);
	io_wr32(r->CMDR, (NAND_CMD_ERASE2 << 8) | NAND_CMD_ERASE);
	io_wr32(r->ADDR1, page << pgshf);
	io_wr32(r->HADDR1, page >> (32 - pgshf));
	io_wr32(r->CTLR, 
		NFC_CTLR_CYCLE(2) | NFC_CTLR_COUNT(1) | NFC_CTLR_STAT |
		NFC_CTLR_OP_ERASE | NFC_CTLR_GO);
	while (!((d = io_rd32(r->ISR)) & NFC_ISR_CMP))
	{
		;
	}
	io_wr32(r->ISR, d);
	/* no data ready - so we wait for status instead */
	while (io_rd32(r->FSR) == 0xFF)
	{
		;
	}

	if (d & NFC_ISR_ERR)
	{
		return NFC_ERR_CMD;
	}
	return io_rd32(r->FSR);
}


void nfc_buf_read(nfc_p t, uint32_t *buf, int count)
{
	uint32_t	*r = ((nfcr_p)t->r)->BUF;
	int			i;

	for (i = 0; i < count; i++)
	{
		buf[i] = r[i];
	}
}


void nfc_buf_write(nfc_p t, uint32_t *buf, int count)
{
	uint32_t	*r = ((nfcr_p)t->r)->BUF;
	int			i;

	for (i = 0; i < count; i++)
	{
		r[i] = buf[i];
	}
}


#if 0

int  memcpy(char *dst, char *src, int len)
{
	while (len--)
	{
		*dst = *src;
		dst++;
		src++;
	}
	return 0;
}


static int  nfc_xfer(nfc_p t, uint32_t adr, char *buf, 
				int ofs, int sz, int wr)
{
	char	*rbuf = (char *)((nfcr_p)t->r)->BUF;

	if (wr)
	{
		memcpy(rbuf + ofs, buf, sz);
		wr = nfc_nand_prog(t, adr);
	}
	else
	{
		wr = nfc_nand_read(t, adr);
		memcpy(buf, rbuf + ofs, sz);
	}
	return wr;
}


int  nfc_nand_io(nfc_p t, nand_geo_t *geo, 
		char *buf, uint32_t addr, int len, int wr)
{
	uint32_t	pgsz;
	uint32_t	pgadr;
	int			rlen;

	pgsz = 1 << geo->page;
	pgadr = addr & ~(pgsz - 1);
	rlen = len;

	if (pgadr != addr)
	{
		int	ofs;
		int	sz;

		ofs = addr - pgadr;
		sz = pgsz - ofs;
		if (sz > rlen)
		{
			sz = rlen;
		}
		nfc_xfer(t, pgadr, buf, ofs, sz, wr);
		buf += sz;
		pgadr += sz;
		rlen -= sz;
	}

	while (rlen >= pgsz)
	{
		nfc_xfer(t, pgadr, buf, 0, pgsz, wr);
		pgadr += pgsz;
		rlen -= pgsz;
		buf += pgsz;
	}

	if (rlen)
	{
		nfc_xfer(t, pgadr, buf, 0, rlen, wr);
		rlen = 0;
	}

	return len - rlen;
}

#endif

typedef void (*fn_void)(void);

void  nfc_nand_boot(nfc_p t)
{
	nfcr_p	r;

	r = t->r;
	nfc_nand_read(t, 0);
	((fn_void)r->BUF)();
}

