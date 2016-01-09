#include <stdint.h>

#include "nand.h"
#include "nfc.h"




#if 0
int  nand_main(void)
#else
void entry(void)
#endif
{
	void (*kernel)(int r0, int r1, int r2) = (void *)0x51008000UL;
	nand_id_t	id;
	nfc_t		t;
	nand_geo_t	geo;
	uint32_t	*nbuf = (uint32_t *)0x40001000UL;
	uint32_t	*p = (uint32_t *)0x51008000UL;
	int			pgsz;
	int			i;
	int			pages;

	t.r = (void *)0x40000000;
	nfc_init(&t);

	nfc_nand_rst(&t);
	nfc_nand_id(&t, &id);
	nand_geo(&id, &geo);
	nfc_cfg(&t, &geo);

#if 0
	/* read 1MB of kernel */
	pgsz = geo.page;
	i = 1 << (geo.block - pgsz);
	pages = (0x100000 >> pgsz) + i;
	pgsz = 1 << (pgsz - 2);
	for (; i < pages; i++)
	{
		uint32_t	d;

		nfc_nand_read(&t, i);
		for (d = 0; d < pgsz; d++)
		{
			*p = nbuf[d];
			p++;
		}
	}

	/* run kernel */
	kernel(0, 1933, 0);
	return 0;
#else
#include "con.h"
#include "uart.h"
{
	volatile uint32_t	*p;
	int	blksz, pgsz, blk, pg;

	blk = 1;
	blksz = 1 << (geo.block - geo.page);
	pgsz = (1 << geo.page) >> 2;
#if 1
puts("erase\n");
flush();
	nfc_nand_erase(&t, blk * blksz);
	for  (pg = 0; pg < blksz; pg++)
	{
		uint32_t	i;

		p = nbuf;
		for (i = 0; i < pgsz; i ++)
		{
			p[i] = i + pg * pgsz;
		}
printf("write %d\n", pg);
flush();
		i = nfc_nand_prog(&t, blk * blksz + pg);
printf("write %d stat=%X\n", pg, i);
flush();
	}
#endif
	for  (pg = 0; pg < blksz; pg++)
	{
		uint32_t	i;

printf("read %d\n", pg);
flush();
		nfc_nand_read(&t, blk * blksz + pg);
		p = nbuf;
		for (i = 0; i < pgsz; i ++)
		{
			if (p[i] != i + pg * pgsz)
			{
				printf("%d %d %X %X\n", pg, i, p[i], i + pg * pgsz);
				flush();
				return;
			}
		}
	}
}
#endif
}

