#include "mytypes.h"
#include "gpttype.h"
#include "gpt.h"

//#define GPT_DBG

#ifdef GPT_DBG
#include "magus.h"
#include "str.h"
#include "debug.h"
#endif

static volatile gpt_t *gpt;
static volatile gpt_t *gpt_32k;

static unsigned long __gpt_presc = -1UL;

int gpt_init (unsigned long base, unsigned long presc)
{
	gpt = (gpt_t *)base;

	if (gpt->id != GPT_ID)
		return -1;
	
	if (presc > 0)
		presc -= 1;

	__gpt_presc = presc;
	
	return 0;
}

int gpt_reset (void)
{
	if (!gpt)
		return -1;

	gpt->ctl |= GPT_CTL_RST;
	while (gpt->ctl & GPT_CTL_RST);

	gpt->presc = __gpt_presc;
	return 0;
}

int gpt_start (void)
{
#ifdef GPT_DBG
	char	p[30];
#endif

	if (!gpt)
		return -1;
	
	gpt->ctl &= ~GPT_CTL_EN;		// stop the counter
	gpt->ctl |= GPT_CTL_CLR;		// clears the counter
	gpt->ctl &= ~GPT_CTL_CLR; 		// counting is halted
	
	gpt->presc = (__gpt_presc == -1UL) ? GPT_DEF_PRESC : __gpt_presc  ;
#ifdef GPT_DBG
	print ("gpt->presc="); print (long2str(p,gpt->presc)); print ("\n");
#endif
//	gpt->cmp = usec;
	gpt->ctl |= GPT_CLKSEL(GPT_CLKSEL_PERCLK) | 
			GPT_CTL_FRS | GPT_CTL_EN;
	return 0;
}
	
int gpt_stop (unsigned long *cnt)
{
	if (!gpt)
		return -1;
	
	gpt->ctl &= ~GPT_CTL_EN;		// stop the counter
	if (cnt) 
		*cnt = gpt->cnt;
	gpt->ctl |= GPT_CTL_CLR;		// clears the counter
	gpt->ctl &= ~GPT_CTL_CLR; 		// counting is halted
	
	return 0;
}

int gpt_usec_wait (unsigned long usec)
{
	int	i;
	
	if (!gpt)
		return -1;
	
	gpt->ctl &= ~GPT_CTL_EN;		// stop the counter
	gpt->ctl |= GPT_CTL_CLR;		// clears the counter
	gpt->ctl &= ~GPT_CTL_CLR; 		// counting is halted
	
	gpt->presc = (__gpt_presc == -1UL) ? GPT_DEF_PRESC : __gpt_presc  ;
	gpt->cmp = usec;

	gpt->ctl |= GPT_CTL_CMPEN | GPT_CLKSEL(GPT_CLKSEL_PERCLK) | 
			GPT_CTL_FRS | GPT_CTL_EN;
	
	for (i = 0; i < 0x10000; i++)
		;

	while (!(gpt->isr & GPT_INT_CMP))
		;

	gpt->isr |= GPT_INT_CMP;	// clear the compare status
	gpt->ctl &= ~GPT_CTL_EN;	// stop the counter
	return 0;
}

int create_32k_clk (
		unsigned long base, 	// base address of gpt to use
		unsigned long presc, 	// perclk value (MHz)
		unsigned long us_period	// required clk period in usec
		)
{
	gpt_32k = (gpt_t *) base;

	if (gpt_32k->id != GPT_ID) 
	{
		return -1;
	}

	// configure gpt to produce a output on tout that will toggle 
	// at a ~32kHz rate
	gpt_32k->ctl |= (GPT_CTL_RST | GPT_CTL_EN);
	while (gpt_32k->ctl & GPT_CTL_RST)
		;
	gpt_32k->presc = presc ? presc : GPT_DEF_PRESC;
	gpt_32k->cmp = us_period/2;
	gpt_32k->ctl = GPT_CTL_CMPEN |
		GPT_CTL_TOSEL |	/* toggle output on compare */
		GPT_CTL_EN;	/* enable the module */

	// clock is not started as no source is specified
		
	return 0;
}

int start_32k_clk (void)
{
	gpt_32k->ctl |= GPT_CLKSEL(GPT_CLKSEL_PERCLK);
	return 0;
}

int stop_32k_clk (void)
{
	gpt_32k->ctl &= ~GPT_CTL_CLKSEL;
	return 0;
}

/* end */
