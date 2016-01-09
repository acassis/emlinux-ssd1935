/*
 * (C) Copyright 2003
 * Texas Instruments <www.ti.com>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002-2004
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * (C) Copyright 2004
 * Philippe Robin, ARM Ltd. <philippe.robin@arm.com>
 *
 * (C) Copyright 2007
 * Sasi, Solomon Systech Ltd. <sasin@solomon-systech.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <arm926ejs.h>

typedef struct
{
    uint32_t    idr;        /**< GPT ID register */
    uint32_t    capr;       /**< GPT capability register */
    uint32_t    ctrl;       /**< GPT control register */
    uint32_t    ier;        /**< GPT interrupt enable register */
    uint32_t    isr;        /**< GPT interrupt status register */
    uint32_t    pslr;       /**< GPT prescaler register */
    uint32_t    cnt;        /**< GPT counter register */
    uint32_t    comp;       /**< GPT compare register */
    uint32_t    capt;       /**< GPT capture register */
//  uint32_t    tst;        /**< GPT test register */
}
volatile gpt_reg_t, *gpt_reg_p;

#define GPT_CTRL_RST    (1 << 1)
#define GPT_CTRL_EN 1
#define GPT_CTRL_DIS    0
#define GPT_CTRL_CAPEN  (1 << 11)
#define GPT_CTRL_COMPEN (1 << 10)
#define GPT_CTRL_CLKSEL (3 << 8)
#define GPT_CTRL_CLKEXT (3 << 8)
#define GPT_CTRL_CLK32K (2 << 8)
#define GPT_CTRL_CLKPER (1 << 8)
#define GPT_CTRL_EDGEDET    (3 << 5)
#define GPT_CTRL_TOUTSEL    (1 << 4)
#define GPT_CTRL_FRS    (1 << 3)
#define GPT_CTRL_CLRCNT (1 << 2)

static const volatile gpt_reg_p	r = (void *)0x08109000;

#define READ_TIMER (r->cnt)

static ulong timestamp;
static ulong lastdec;

void timer_init (void)
{
	r->ctrl = GPT_CTRL_EN | GPT_CTRL_RST;
	while (r->ctrl & GPT_CTRL_RST)
	{
		;
	}
	r->ctrl = 0;
	r->pslr = 31;
	r->ctrl = GPT_CTRL_EN | GPT_CTRL_CLK32K | GPT_CTRL_FRS;

	reset_timer_masked();
}

/*
 * timer without interrupts
 */

void reset_timer (void)
{
	reset_timer_masked ();
}

ulong get_timer (ulong base)
{
	return get_timer_masked () - base;
}

void set_timer (ulong t)
{
	timestamp = t;
}

/* delay x useconds AND perserve advance timstamp value */
void udelay (unsigned long usec)
{
	ulong tmo, tmp;

	if(usec >= 1000){		/* if "big" number, spread normalization to seconds */
		tmo = usec / 1000;	/* start to normalize for usec to ticks per sec */
		tmo *= CFG_HZ;		/* find number of "ticks" to wait to achieve target */
		tmo /= 1000;		/* finish normalize. */
	}else{				/* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CFG_HZ;
		tmo /= (1000*1000);
	}

	tmp = get_timer (0);		/* get current timestamp */
	if( (tmo + tmp + 1) < tmp )	/* if setting this fordward will roll time stamp */
		reset_timer_masked ();	/* reset "advancing" timestamp to 0, set lastdec value */
	else
		tmo += tmp;		/* else, set advancing stamp wake up time */

	while (get_timer_masked () < tmo)/* loop till event */
		/*NOP*/;
}

void reset_timer_masked (void)
{
	/* reset time */
	lastdec = READ_TIMER;  /* capure current decrementer value time */
	timestamp = 0;	       /* start "advancing" time stamp from 0 */
}

ulong get_timer_masked (void)
{
	ulong now = READ_TIMER;

	if (lastdec < now) 
	{
		timestamp += now - lastdec;
	}
	else
	{
		/* we have overflow of the count down timer */
		timestamp += lastdec - now;
	}
	lastdec = now;
	return timestamp;
}

/* waits specified delay value and resets timestamp */
void udelay_masked (unsigned long usec)
{
	ulong tmo;
	ulong endtime;
	signed long diff;

	if (usec >= 1000) {		/* if "big" number, spread normalization to seconds */
		tmo = usec / 1000;	/* start to normalize for usec to ticks per sec */
		tmo *= CFG_HZ;		/* find number of "ticks" to wait to achieve target */
		tmo /= 1000;		/* finish normalize. */
	} else {			/* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CFG_HZ;
		tmo /= (1000*1000);
	}

	endtime = get_timer_masked () + tmo;

	do {
		ulong now = get_timer_masked ();
		diff = endtime - now;
	} while (diff >= 0);
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
	ulong tbclk;

	tbclk = CFG_HZ;
	return tbclk;
}
