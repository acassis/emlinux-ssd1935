#define NPRINTF 1

#include <common.h>
#include "sdrtype.h"
#include "sdr.h"

#if (NPRINTF == 1)
#define dbg(s, ...)
#else
#include "stdio.h"
#define dbg	printf
#endif
#define BIT(x)		(1UL << (x))

static sdr_t *sdr;

#define USE_EXT_RFRCLK	1	// don't turn this off
#define SDR_CFG_SINGLE	1
#define USE_SDR_FCLK	1
#define USE_SELF_TUNE	1

int sdr_init (uint32_t base)
{
	sdr = (sdr_t *) base;

	if (sdr->id != SDR_ID) 
	{
		sdr = (sdr_t *) 0;
		return -1;
	}

	return 0;
}

int sdr_reset (void)
{
	int tmp = 0;

	if (sdr) 
	{
		sdr->ctl = (SDR_CTL_RST | SDR_CTL_EN);
		while (sdr->ctl & SDR_CTL_RST) 
		{
			tmp++;
			if (tmp == 0xFFFF)
			{
				return -1;
			}
		}
		return 0;
	}

	return -1;
}

int sdr_cfg (sdr_cfg_t *cfg)
{
	uint32_t w;
	uint32_t row = 0, col = 0, bl = 0, rfr = 0;
	int	i;

	if (!cfg) 
		return -1;
	
	row = cfg->row - 11;
	col = cfg->col - 7;

	// convert burst length
	bl = cfg->bl;
	for (i = 0; i < 4; i++)
	{
		if (bl & 0x01)
			break;
		bl >>= 1;
	}
	bl = (uint32_t) i;

#if (USE_EXT_RFRCLK)
	// generate refresh clock requirement
	if (cfg->row == 13)
		rfr = 3;
	else if (cfg->row == 12)
		rfr = 2;
	else if (cfg->row == 11)
		rfr = 1;
	else
		rfr = 0;
	rfr |= 0x04UL;
#endif
	w = SDR_ROW (row) | SDR_COL (col) |
		SDR_BL (bl) | SDR_CAS (cfg->cas) |
		(cfg->dsz ? SDR_CS_DSZ : 0);

	w |= SDR_ATRFR(0xF) |		/* at least 16 clocks between refresh */
		SDR_RCD (3) |		/* row to column delay */
		SDR_RPD (3) |		/* row to precharge delay */
		SDR_MRD (3) |		/* mode register delay */
		SDR_DPD (4); 		/* write data to precharge delay */
	
#if (USE_EXT_RFRCLK)
	w |= SDR_RFR(rfr);
#endif

	w &= ~SDR_CS_IAM;		/* 1=interleaved/0=sequential(linear) */
#if (!USE_EXT_RFRCLK)
	w &= ~SDR_CS_RFR;		/* external refresh clock is disabled */
#endif

#if (SDR_CFG_SINGLE)
	if (cfg->cs == 0)
		sdr->cfg0 = w | SDR_CS_EN;	/* enable the configuration */
	else
		sdr->cfg1 = w | SDR_CS_EN;	/* enable the configuration */
#else
	sdr->cfg0 = w | ((cfg->cs == 0) ? SDR_CS_EN : 0);
	sdr->cfg1 = w | ((cfg->cs == 1) ? SDR_CS_EN : 0);
#endif

	dbg ("sdr_cfg: cs0 = %08lx\n", sdr->cfg0);
	dbg ("sdr_cfg: cs1 = %08lx\n", sdr->cfg1);

	return 0;
}

int sdr_enable (void)
{
	if (!sdr)
		return -1;
	
	sdr->ctl = SDR_CTL_EN;
	return 0;
	
}

int sdr_power_up (void)
{
	if (!sdr)
		return -1;
	
	sdr->ctl = SDR_CTL_EN | SDR_CTL_PWRUP | SDR_CTL_CLKSEL;
	return 0;
	
}

int sdr_start (sdr_cfg_t *cfg)
{
	uint32_t	bl = 0;
	volatile uint32_t	tmp;
	uint32_t	*mem;
	uint32_t	cmd = 0;
	int i;

	if (!sdr)
		return -1;
	
	if (sdr_cfg (cfg) < 0)
	{
		return -1;
	}

	if (cfg->cs == 0)
		mem = (uint32_t *)(SDR_CSD0_BASE+0xAAA0);
	else if (cfg->cs == 1)
		mem = (uint32_t *)(SDR_CSD1_BASE+0xBBB0);
	else
		return -1;
	
	dbg ("sdr_start: mem = 0x%lx\n", mem);

	sdr->rfrt = 0x00000300;		// for a 64ms internal refresh clock
	sdr->dqs_cnt = 0x0000001F;	// defaults
	sdr->dly_dat = 0xA5FF5A00;	// defaults
	sdr->dly_cfg = 0x003F0000;	// defaults


	cmd = SDR_CTL_TAP | 		/* use internal alignment unit */
#if (!USE_EXT_RFRCLK)
		SDR_CTL_RFCSEL |
#endif

#if (!USE_SDR_FCLK)
		SDR_CTL_CLKSEL | 	/* internal delayed clock */
#endif
		SDR_CTL_PWRUP | 	/* 200us wait is over */
		SDR_CTL_EN;

	sdr->ctl = cmd;
	// we issue a precharge all
	// followed by 2 refresh commands

	sdr->ctl = cmd | SDR_CMD (SC_PRECHARGE_ALL);
	tmp = *mem;
	tmp = *mem;
	sdr->ctl = cmd | SDR_CMD (SC_AUTO_REFRESH);
	tmp = *mem;
	tmp = *mem;
	
	// set mrs
	bl = cfg->bl;
	for (i = 0; i < 4; i++)
	{
		if (bl & 0x01)
			break;
		bl >>= 1;
	}
	bl = (uint32_t) i;
	if (bl == 0x4) 		/* set for page ? */
		bl = MRS_BL_PAGE;

	sdr->mrs = ((((cfg->cas & 0x07UL) << 4) | bl)>>0);	// sequential
	sdr->ctl = cmd | SDR_CMD (SC_SET_MRS);
	tmp = *mem;
	tmp = *mem;

#if (CONFIG_EMRS)
	/* set emrs (ba[1:0] = 2'b10 */

	sdr->mrs = SDR_EMRS_SEL(EMRS_SEL) | SDR_EMRS_DS(DS_FULL) | SDR_EMRS_PASR(PASR_FULL);
	sdr->ctl = cmd | SDR_CMD (SC_SET_MRS);
	tmp = *mem;
	tmp = *mem;
#endif

	sdr->ctl = cmd | SDR_CMD (SC_AUTO_REFRESH);
	tmp = *mem;
	tmp = *mem;

	sdr->ctl = cmd;

#if (USE_SELF_TUNE)
	sdr->ctl |= SDR_CTL_STUNEN; 	/* self tune */
	tmp = *mem;
	tmp = *mem;
	tmp = *mem;
	tmp = *mem;
	sdr->ctl &= ~SDR_CTL_STUNEN; 	
#endif

	return 0;
}

/* end */
