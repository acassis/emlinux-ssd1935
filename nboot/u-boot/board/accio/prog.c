#include <common.h>
#include <asm/byteorder.h>
#include "nand.h"
#include "nfc.h"
#include "../../include/nboot.h"

void burst8(char *dst, char *src, int len);

void mcpy32(uint32_t *dst, uint32_t *src, int len)
{
	while (len--)
	{
		*dst = *src;
		dst++;
		src++;
	}
}

int mcmp32(uint32_t *dst, uint32_t *src, int len)
{
	int	i;

	for (i = 0; i < len; i++)
	{
		if (*dst != *src)
		{
			printf("\b%X!=%X", *dst, *src);
			return i;
		}
		dst++;
		src++;
	}
	return -1;
}


int  bb_checkok(uint32_t *buf, int blk)
{
	return (buf[(blk >> 5) + 1] & (1 << (blk & 31)));
}


void  bb_markbad(uint32_t *buf, int blk)
{
	buf[(blk >> 5) + 1] &= ~(1 << (blk & 31));
}


int nand_copy(char *prog, uint32_t dest, uint32_t src, int size)
{
	nfc_t		t;
	nand_id_t	id;
	nand_geo_t	geo;
	uint32_t	i;
	uint32_t	pages, blocks, ipl_pages = 0;
	uint32_t	*nfcbuf;
	uint32_t	blksz, pgsz, pg, pg_src, ppb, pgstart;
	int		blks, pgs;
	uint32_t	buf[2048];
	int		rt = 0, pgcnt, pgmax, handled;
	char		c = 0, c2 = 0;
	struct nboot_desc *nboot;

	/* decide program functionality by exe name */
	i = strlen(prog) - 1;
	c = prog[i];
	switch (c)
	{
		case 'v':
			c2 = prog[i - 1];
			if (c2 == 'c')
			{
				t.ecc = 1;
			}
			else if (c2 == 'g')
			{
				t.ecc = 0;
			}
			break;

		case 'c':
			t.ecc = 1;
			break;

		case 'g':
			t.ecc = 0;
			break;
	}

	/* initialize NFC & get NAND FLASH geometry */
	t.r = (void *)0x40000000;
	t.cs = 0;
#if 1
	rt = nfc_init(&t);
	if (rt < 0)
	{
		printf("nfc_init err %d\n", rt);
		goto out;
	}

	rt = nfc_nand_rst(&t);
	if (rt < 0)
	{
		printf("nfc_nand_rst err %d\n", rt);
		goto out;
	}
#endif
	rt = nfc_nand_id(&t, &id);
	if (rt < 0)
	{
		printf("nfc_nand_id err %d\n", rt);
		goto out;
	}

#if 1
	printf("nfc_nand_id %08X %08X nfcver=%X\n", 
		*(uint32_t *)&id, ((uint32_t *)&id)[1], t.ver);
#endif

	rt = nand_geo(&id, &geo);
	if (rt < 0)
	{
		printf("nfc_nand_geo err %d\n", rt);
		goto out;
	}
#if 1
	printf("page=%d block=%d size=%dM spare=%d %dbit\n",
		1 << geo.page, 1 << geo.block, 1 << geo.size, 
		geo.spare, 8 << geo.bit16);
#endif

	rt = nfc_cfg(&t, &geo);
	if (rt < 0)
	{
		printf("nfc_cfg err %d\n", rt);
		goto out;
	}
	nfcbuf = (uint32_t *)(((char *)t.r) + 0x1000);


	/* calculate page & block sizes */
	blksz = geo.block - geo.page;
	pgsz = 1 << geo.page;
	pg = dest >> geo.block << blksz;
	pg_src = src >> geo.block << blksz;
	ppb = 1 << blksz;
	pgmax = 1 << (geo.size + 20 - geo.page);
	pgstart = pg;

	/* revise size */
	if (c == 'g' || c2 == 'g') //case of boot loader
	{
		/* the 1st block is alway valid */
		ipl_pages = IPL_LEN >> geo.page;
		nfc_nand_read(&t, pg + ipl_pages - 1);
		nboot = (struct nboot_desc *)((char *)nfcbuf + pgsz - sizeof(struct nboot_desc));

		if (nboot->magic == NBOOT_MAGIC)
		{
			pages = nboot->len;
			if (c == 'v')
				printf("- nboot image size: %d\n", pages);
		}
		else
		{
			pages = size;
			if (c == 'v')
				printf("- unknow image size: %d\n", pages);
		}
	}
	else
	{
		while (block_isbad(&t, pg, pgsz))
		{
			pg += ppb;
			if (pg > pgmax)
			{
				printf("error: too many bad blocks...exit 1\n");
				goto out;
			}
			printf("page %d is bad block\n",pg);
			continue;
			
		}
		if (0x28cd3d45 == nfcbuf[0])
		{
			pages = nfcbuf[1];
			//if (c == 'v')
			//	printf("- cramfs image size: %d\n", pages);
		}
		else
		{
			if (IH_MAGIC == ntohl(nfcbuf[0]))
			{
				image_header_t *hdr = (image_header_t *)nfcbuf;
				pages = ntohl(hdr->ih_size) + sizeof(image_header_t);
				if (c == 'v')
					printf("- kernel image size: %d\n", pages);
			}
			else
			{
				pages = size;
				if (c == 'v')
					printf("- unknow image size: %d\n", pages);
			}
		}
	}

	if (pages > size)
	{
		printf("image size exceed the section\n");
		goto out;
	}

	pages = (pages + ((1 << geo.page) - 1)) >> geo.page;
	blocks = (pages + ((1 << blksz) - 1)) >>  blksz;
	blks = blocks;
	pgs = pages;
	pgcnt = 0;
	handled = 0;
	pg = pgstart;

	/* skip to verify */
	if (c == 'v') goto l_verify;

	/* start programming sequence */
	//printf("  program:");
	while (blks > 0)
	{
		/* check */
		if (c != 'g')
		{
			if (block_isbad(&t, pg, pgsz))
			{
				printf(" B");
				pg += ppb;
				if (pg > pgmax)
				{
					printf("error: too many bad blocks...exit 2\n");
					goto out;
				}
				printf("page %d is bad block\n",pg);
				continue;
			}
		}

		/* find good source */
		while (block_isbad(&t, pg_src, pgsz))
		{
			pg_src += ppb;
			if (pg_src > pgmax)
			{
				printf("error: bad blocks so many.\n");
				goto out;
			}
			continue;
		}

		/* erase */
		rt = nfc_nand_erase(&t, pg);
		if (rt < 0)
		{
			printf("%dr%d ", pg, rt);
			goto out;
		}
		else if (rt & (NAND_ST_FAIL | NAND_ST_FAIL_N1))
		{
			printf("%ds%X ", pg, rt);
			goto out;
		}

		/* program */
		if (c == 'g' && !handled)
		{
			pgcnt = ipl_pages < ppb ? ipl_pages : ppb;
			for (i = 0; i < pgcnt; i++)
			{
				nfc_nand_read(&t, pg_src);
				burst8((char *)buf, (char *)nfcbuf, pgsz);
				rt = nfc_nand_prog(&t, pg);
				if (rt < 0)
				{
					printf("\b%dr%d", pg, rt);
					goto out;
				}
				else if (rt & (NAND_ST_FAIL | NAND_ST_FAIL_N1))
				{
					printf("\b%ds%X", pg, rt);
					goto out;
				}
				pg++;
				pg_src++;
			}
			t.ecc = 1;
			nfc_cfg(&t, &geo);
			handled = 1;
			pgs -= pgcnt;
		}

		pgcnt = pgs > ppb ? ppb : pgs;
		for  (i = 0; i < pgcnt; i++)
		{
			nfc_nand_read(&t, pg_src);
			burst8((char *)buf, (char *)nfcbuf, pgsz);
			rt = nfc_nand_prog(&t, pg);
			if (rt < 0)
			{
				printf("\b%dr%d", pg, rt);
				goto out;
			}
			else if (rt & (NAND_ST_FAIL | NAND_ST_FAIL_N1))
			{
				printf("\b%ds%X", pg, rt);
				goto out;
			}

			/* verify */
			nfc_nand_read(&t, pg);
			rt = mcmp32(nfcbuf, buf, pgsz >> 2);
			if (rt >= 0)
			{
				printf(" %do%d ", pg, rt << 2);
				goto out;
			}
			pg++;
			pg_src++;
		}
		//printf(" W");
		blks--;
		pgs -= pgcnt;
	}
	//printf(" ... OK\n");
	return 0;

l_verify:
{
	//printf("  verify :");
	while (blks > 0)
	{
		if (c2 != 'g')
		{
			while (block_isbad(&t, pg, pgsz))
			{
				printf(" B");
				pg += ppb;
				if (pg > pgmax)
				{
					printf("error: bad blocks so many.\n");
					goto out;
				}
				continue;
			}
		}

		/* find good source */
		while (block_isbad(&t, pg_src, pgsz))
		{
			pg_src += ppb;
			if (pg_src > pgmax)
			{
				printf("error: bad blocks so many.\n");
				goto out;
			}
			continue;
		}

		if (c2 == 'g' && !handled)
		{
			pgcnt = ipl_pages < ppb ? ipl_pages : ppb;
			for (i = 0; i < pgcnt; i++)
			{
				pg++;
				pg_src++;
			}
			t.ecc = 1;
			nfc_cfg(&t, &geo);
			handled = 1;
			pgs -= pgcnt;
		}

		pgcnt = pgs > ppb ? ppb : pgs;
		for  (i = 0; i < pgcnt; i++)
		{
			nfc_nand_read(&t, pg_src);
			burst8((char *)buf, (char *)nfcbuf, pgsz);
			nfc_nand_read(&t, pg);
			rt = mcmp32(nfcbuf, buf, pgsz >> 2);
			if (rt >= 0)
			{
				goto out;
			}
			pg++;
			pg_src++;
		}
		//printf(" V");
		blks--;
		pgs -= pgcnt;
	}
	//printf(" ... OK\n");
	return 0;
}
out:
	printf(" ... exit with error!\n");
	return -1;
}


int nand_check(uint32_t phy_add,int size,int *total_errpage)
{
	nfc_t		t;
	nand_id_t	id;
	nand_geo_t	geo;
	uint32_t	i;
	uint32_t	pages, blocks, ipl_pages = 0;
	uint32_t	*nfcbuf;
	uint32_t	blksz, pgsz, pg, pg_src, ppb, pgstart;
	int		blks, pgs;
	uint32_t	buf[2048];
	int		rt = 0, pgcnt, pgmax, handled;
	char		c = 0, c2 = 0;
	struct nboot_desc *nboot;
	int max_errcnt=0;
	*total_errpage=0;
	
	/* initialize NFC & get NAND FLASH geometry */
	t.r = (void *)0x40000000;
	t.cs = 0;

	rt = nfc_init(&t);
	if (rt < 0)
	{
		printf("nfc_init err %d\n", rt);
		goto out;
	}

	rt = nfc_nand_rst(&t);
	if (rt < 0)
	{
		printf("nfc_nand_rst err %d\n", rt);
		goto out;
	}

	rt = nfc_nand_id(&t, &id);
	if (rt < 0)
	{
		printf("nfc_nand_id err %d\n", rt);
		goto out;
	}


	rt = nand_geo(&id, &geo);
	if (rt < 0)
	{
		printf("nfc_nand_geo err %d\n", rt);
		goto out;
	}

	rt = nfc_cfg(&t, &geo);
	if (rt < 0)
	{
		printf("nfc_cfg err %d\n", rt);
		goto out;
	}
	nfcbuf = (uint32_t *)(((char *)t.r) + 0x1000);

	/* calculate page & block sizes */
	blksz = geo.block - geo.page;
	pgsz = 1 << geo.page;	
	pg_src = phy_add >> geo.block << blksz;
	ppb = 1 << blksz;
	pgmax = 1 << (geo.size + 20 - geo.page);
	pgstart = pg_src;

	blks=size/pgsz/ppb;
	
	//printf("ecc check\n");
	while (blks > 0)
	{		
		while (block_isbad(&t, pgstart, pgsz))
		{
			printf("B");
			pgstart += ppb;
			if (pgstart > pgmax)
			{
				printf("error: too many bad block.\n");
				return -1;
			}
			continue;
		}		 
		
		for  (i = 0; i < ppb; i++)
		{			
			if(rt = nfc_nand_read(&t, pgstart)>0)
			{				
				int b,p;
				b=pgstart/ppb;
				p=pgstart-b*ppb;					
				//printf("ecc corrected at block %d page = %d count = %d\n",b,p,rt);				
				if(rt>max_errcnt)
					max_errcnt = rt;
				(*total_errpage)++;	
				
				
					
			}
			pgstart++;
		}		
		blks--;		
	}
	//printf(" ... OK\n");	
	return max_errcnt;

out:
	printf(" ... exit with error!\n");
	return -1;
	
}
