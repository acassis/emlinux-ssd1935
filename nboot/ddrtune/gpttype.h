#ifndef __GPTTYPE_H__
#define __GPTTYPE_H__

#include "mytypes.h"

typedef struct _gpt_t {
	unsigned long	id;
	unsigned long	cap;
	unsigned long	ctl;
	unsigned long	ier;
	unsigned long	isr;
	unsigned long	presc;
	unsigned long	cnt;
	unsigned long	cmp;
	unsigned long	cpt;
} gpt_t;

#define GPT_ID	0x08020040
#define GPT_CAP 0x00000040

/* control reg */
#define GPT_CTL_EN	BIT(0)
#define GPT_CTL_RST	BIT(1)
#define GPT_CTL_CLR	BIT(2)	/* {clr,en}=> halt={0,0}, rst={1,0}
				   running={x,1}
				*/
#define GPT_CTL_FRS	BIT(3)	/* 1=free-run, 0=restart */
#define GPT_CTL_TOSEL	BIT(4)	/* 1=toggle, 0=pulse */
#define GPT_CTL_TISEL	(BIT(6) | BIT(5))
#define GPT_CTL_CLKSEL	(BIT(9) | BIT(8))
#define GPT_CTL_CMPEN	BIT(10)
#define GPT_CTL_CPTEN	BIT(11)

/* ier, isr */
#define GPT_INT_CMP	BIT(0)
#define GPT_INT_CPT	BIT(1)

#define GPT_CLKSEL(x)		((x) << 8)
#define GPT_CLKSEL_NONE		0
#define GPT_CLKSEL_PERCLK	1
#define GPT_CLKSEL_32KCLK	2
#define GPT_CLKSEL_EXTCLK	3

#define GPT_MODE(x)		((x) << 10)
#define GPT_MODE_CMP		1
#define GPT_MODE_CPT		2

#define GPT_TISEL(x)		((x) << 6)
#define GPT_TOSEL(x)		((x) << 4)

#define GPT_DEF_PRESC	25

#endif // __GPTTYPE_H__
