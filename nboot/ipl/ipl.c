#include <stdint.h>
#include "proc.h"
#include "nand.h"
#include "nfc.h"
#include "../include/nboot.h"
#include "../include/config.h"

int		puts(char *);
void	brd_init(void);
void	prism_cfg(void);

/* boot area and kernel area size */
//#define BOOT_SIZE	(1 << 20)
//#define KERN_SIZE	(2 << 20)

#define ECC_ON

int main(void)
{
	nfc_t			t;
	nand_id_t		id;
	nand_geo_t		geo;
	void			(*spl)(void);
	char			*p;
	int			pgsz, pg, npages;
#if defined DDR_TUNE
	struct ddr_tune_desc	*ddr_tune;
#else
	struct nboot_desc	*nboot;
#endif

	brd_init();
	prism_cfg();

#ifdef ECC_ON
#warning "ecc on"
//	puts("NAND ECC\n");
	t.ecc = 1;
#else
#warning "ecc off"
//	puts("NAND\n");
	t.ecc = 0;
#endif
	t.r = (void *)0x40000000;
	nfc_init(&t);
	nfc_nand_rst(&t);
	nfc_nand_id(&t, &id);
	nand_geo(&id, &geo);
	nfc_cfg(&t, &geo);
	
	pgsz = 1 << geo.page;
	pg = IPL_LEN >> geo.page;

#if defined DDR_TUNE
	ddr_tune = (struct ddr_tune_desc *)(LDR_BASE + IPL_LEN - sizeof(struct ddr_tune_desc));
	if(ddr_tune ->magic == TUNE_MAGIC && ddr_tune->len < NBOOT_MAX_LEN) {
		npages = (ddr_tune->len >> geo.page) + 1;
	}
	else {
		npages = NBOOT_MAX_LEN >> geo.page;
	}
	
	spl = (void *)0xffff2000;
#else
	nboot = (struct nboot_desc *)(LDR_BASE + IPL_LEN - sizeof(struct nboot_desc));

	if (nboot->magic == NBOOT_MAGIC && nboot->len < NBOOT_MAX_LEN)
	{
		npages = (nboot->len >> geo.page) + 1;
	}
	else
	{
		npages = NBOOT_MAX_LEN >> geo.page;
	}

	spl = (void *)0x52F40000;
#endif
	p = (void *)spl;
	while (npages--)
	{
		nfc_nand_read(&t, pg);
		burst8(p, (char *)0x40001000, pgsz);
		pg++;
		p += pgsz;
	}
	spl();
	return 0;
}

