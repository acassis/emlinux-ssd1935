#include <common.h>

#include "nand.h"
#include "nfcr.h"
#include "nfc.h"

#define io_rd32(r)		(r)
#define io_wr32(r, d)	((r) = (d))

int nfc_wait_read(nfc_p t, uint32_t sta);

static void  nfc_rst(nfc_p t)
{
	nfcr_p		r;

	r = t->r;
	io_wr32(r->CFG, NFC_CFG_RST | NFC_CFG_EN);
	while (1)
	{
		uint32_t	d;

		d = io_rd32(r->CFG);
		if ((d & (NFC_CFG_RST | NFC_CFG_EN)) == NFC_CFG_EN)
		{
			break;
		}
	}
}


int  nfc_init(nfc_p t)
{
	uint32_t	d;
	nfcr_p		r;

	r  = t->r;

	/* check id */
	d = io_rd32(r->ID);
	if (NFC_ID_CLID(d) != NFC_PCI_CLASS)
	{
		return -1;
	}

	/* tapeout2 ID not updated */
	d = NFC_ID_VER(d);
	if (d == 0x40 && io_rd32(r->CAP) == (NFC_CAP_BUF4K | NFC_CAP_BIT16))
	{
		d = 0x41;
	}
	t->ver = d;

	/* enable module,  reset and check reset value */
	nfc_rst(t);
	io_wr32(r->FSCR, (NAND_CMD_STAT << 16) | 0xFF);
	io_wr32(r->CFGR2, 0x33031101);
	return 0;
}


void  nfc_exit(nfc_p t)
{
	nfcr_p	r;

	r = t->r;
	nfc_rst(t);
	io_wr32(r->CFG, 0);
}


int  nfc_cfg(nfc_p t, nand_geo_t *c)
{
	static const uint8_t _pgsz[8] = {0, 1, 2, 2, 3, 4, 6, 7};
	nfcr_p	r;

	r = t->r;
	io_wr32(r->CFG, 
		((c->size - c->page + 20 - 1) << 24) | (c->page << 16) | 
		(((c->block - c->page) - 3) << 12) |
		(c->bit16 << 11) | (_pgsz[c->page - 8] << 8) |
		NFC_CFG_SEQ | NFC_CFG_EN);
	t->page = c->page;

	if (t->ecc)
	{
		uint32_t	ecc;

		switch (c->page)
		{
			case 9:
				ecc = NFC_ECC_MODEH | NFC_ECC_HOFS(0) | NFC_ECC_HRST;
				break;

			case 11:
				t->eccmode = 2;
				ecc = NFC_ECC_MODET4;
				io_wr32(r->TECC4, NFC_TECC4_RST | NFC_TECC4_REDEC);
				break;

			case 12:
				if(c->spare==128)
				{	
					t->eccmode = 2;
					ecc = NFC_ECC_MODET4;
					io_wr32(r->TECC4, NFC_TECC4_RST | NFC_TECC4_REDEC);					
				}	
				else
				{	
					t->eccmode = 3;	
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


int  nfc_wait(nfcr_p r, uint32_t sta)
{
	uint32_t	d;
	uint32_t	t = 0;

	do
	{
		d = io_rd32(r->ISR);
		t++;
		if (t == 1000000)
		{
			return NFC_ERR_TOUT;
		}
	}
	while ((d & sta) != sta);
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

//add for nand read only, to return max no. of ecc correctable bit
int  nfc_wait_read(nfc_p t, uint32_t sta)
{
	uint32_t	d;
	uint32_t	timeout = 0;

	nfcr_p	r;	
	r = t->r;

	do
	{
		d = io_rd32(r->ISR);
		timeout++;
		if (timeout == 1000000)
		{
			return NFC_ERR_TOUT;
		}
	}
	while ((d & sta) != sta);
	
	

	
	if (d & NFC_ISR_CMP)			
	{				
		//command is completed
		uint32_t fix, err, ecc;			
		uint32_t errcnt,i,max_errcnt;
		int loop;
		max_errcnt = 0;			
		ecc = d >> 8;
		err = fix = 0;
		
		//check bit 8 to bit 31
		while (ecc)
		{
			switch (ecc & 3)
			{
				case 1: fix++; break;
				case 2: case 3: err++; break;
			}
			ecc >>= 2;
		}					
					
		//check max ECC corection count
		if(fix)
		{						
			//fix = (fix << 8);	//no. of fix per page, to return out
			//fix |= NFC_EVT_ECC_CORRECTED;				
			
			//for checking no. of ecc used
			ecc = d >> 8;			
						
			if(t->eccmode == 3)			//8 bit ecc
			{	
				//printf("r->TECC8R =0x%08x \n",io_rd32(r->TECC8R));				
				errcnt = io_rd32(r->TECC8R);
				for(i=0;i<8;i++)			
				{
					if ((errcnt & 0x07) > max_errcnt)
						max_errcnt	= (errcnt & 0x07);
					errcnt >>= 4;
				}							
			}	
			else						//4 bit ecc
			{	
				//printf("r->TECC4R =0x%08x \n",io_rd32(r->TECC4R));				
				errcnt = io_rd32(r->TECC4R);
				
				//4Kpage
				if(t->page==12)	
					loop=8;
				else
					loop=4;
				
				
				for(i=0;i<loop;i++)			
				{					
					if(ecc&0x3)	//check which 512 has ecc fix, only necessary for case of 4bit ecc
					{
						if ((errcnt & 0x03) > max_errcnt)
							max_errcnt	= (errcnt & 0x03);					
					}					
					ecc >>= 2;
					errcnt >>= 4;
				}							
				max_errcnt+=1;	//0 means 1bit error!
			}		
			
			//store max ecc bit per 518 bytes
			//fix |= (max_errcnt << 16);
		}
		//d  = NFC_EVT_CMP | fix;		
		io_wr32(r->ISR, d);		
			
		return max_errcnt;
	}
		
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


int  nfc_nand_rst(nfc_p t)
{
	nfcr_p	r;

	r = t->r;
	io_wr32(r->CTL, NFC_CTL_OP_RST | NFC_CTL_GO | NFC_CTL_CHIP(t->cs));
	return nfc_wait(r, NFC_ISR_CMP);
}


int  nfc_nand_id(nfc_p t, nand_id_t *id)
{
	nfcr_p	r;
	int		rt;

	r = t->r;
	io_wr32(r->CMD, NAND_CMD_ID);
	io_wr32(r->ADDR1, 0);
	io_wr32(r->HADDR1, 0);
	io_wr32(r->CTL, NFC_CTL_COUNT(5) | NFC_CTL_OP_ID | 
		NFC_CTL_GO | NFC_CTL_CHIP(t->cs));

	rt = nfc_wait(r, NFC_ISR_CMP);
	if (rt)
	{
		return rt;
	}
	*id = *(nand_id_t *)r->BUF;
	return 0;
}


int  nfc_nand_read(nfc_p t, uint32_t page)
{
	nfcr_p	r;
	int		pgshf, rt;

	r = t->r;
	pgshf = t->page + 1;
	io_wr32(r->CMD, (NAND_CMD_READ_MLC << 8) | NAND_CMD_READ);
	io_wr32(r->ADDR1, page << pgshf);
	io_wr32(r->HADDR1, page >> (32 - pgshf));
	io_wr32(r->CTL, 
		NFC_CTL_CYCLE(pgshf < 12 ? 1 : 2) | NFC_CTL_COUNT(1) | 
		NFC_CTL_OP_READ | NFC_CTL_GO | NFC_CTL_CHIP(t->cs) | 
		(t->ecc ? NFC_CTL_ECCEN | NFC_CTL_ECCAUTO : 0));

	//TO1
	if(t->ver == 0x40)
		rt = nfc_wait(r, NFC_ISR_CMP | (t->ver == 0x40 ? NFC_ISR_DRDY : 0));
	else
		rt = nfc_wait_read(t, NFC_ISR_CMP);

	if (rt == NFC_ERR_DAT && t->ver == 0x40)
	{
		rt = 1 << (pgshf - 3);
		while (rt--)
		{
			if (r->BUF[rt] != 0xFFFFFFFF)
			{
				return NFC_ERR_DAT;
			}
		}
		return 0;
	}
	return rt;
}


int  nfc_nand_prog(nfc_p t, uint32_t page)
{
	nfcr_p	r;
	int		rt;
	int		pgshf;

	r = t->r;
	pgshf = t->page + 1;

	io_wr32(r->FSR, 0xFF);
	io_wr32(r->CMD, (NAND_CMD_PROG << 8) | NAND_CMD_SEQ);
	io_wr32(r->ADDR1, page << pgshf);
	io_wr32(r->HADDR1, page >> (32 - pgshf));
	io_wr32(r->CTL, 
		NFC_CTL_CYCLE(2) | NFC_CTL_COUNT(1) | NFC_CTL_STAT |
		NFC_CTL_OP_PROG | NFC_CTL_GO | NFC_CTL_CHIP(t->cs) |
		(t->ecc ? NFC_CTL_ECCEN | NFC_CTL_ECCAUTO : 0));

	rt = nfc_wait(r, NFC_ISR_CMP);
	if (rt)
	{
		return rt;
	}
	return io_rd32(r->FSR);
}


int  nfc_nand_erase(nfc_p t, uint32_t page)
{
	nfcr_p	r;
	int		rt;
	int		pgshf;

	r = t->r;
	pgshf = t->page + 1;
	io_wr32(r->FSR, 0xFF);
	io_wr32(r->CMD, (NAND_CMD_ERASE2 << 8) | NAND_CMD_ERASE);
	io_wr32(r->ADDR1, page << pgshf);
	io_wr32(r->HADDR1, page >> (32 - pgshf));
	io_wr32(r->CTL, 
		NFC_CTL_CYCLE(2) | NFC_CTL_COUNT(1) | NFC_CTL_STAT |
		NFC_CTL_OP_ERASE | NFC_CTL_GO | NFC_CTL_CHIP(t->cs));

	rt = nfc_wait(r, NFC_ISR_CMP);
	if (rt)
	{
		return rt;
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

int block_isbad(nfc_p t, uint32_t pg, uint32_t pgsz)
{
	uint8_t *nfcbuf = ((uint8_t *)t->r) + 0x1000;

	nfc_nand_read(t, pg);
	return (nfcbuf[pgsz] != 0xff);
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


typedef void (*fn_void)(void);

void  nfc_nand_boot(nfc_p t)
{
	nfcr_p	r;

	r = t->r;
	nfc_nand_read(t, 0);
	((fn_void)r->BUF)();
}

#endif

