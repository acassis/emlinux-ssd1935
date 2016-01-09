#ifndef __GPT_H__
#define __GPT_H__

int gpt_init (unsigned long base, unsigned long presc);
int gpt_usec_wait (unsigned long usec);
int gpt_reset (void);
int gpt_start (void);
int gpt_stop (unsigned long *cnt);

int create_32k_clk (
		unsigned long base, 	// base address of gpt to use
		unsigned long presc, 	// perclk value (MHz)
		unsigned long us_period	// required clk period in usec
		);
int start_32k_clk (void);
int stop_32k_clk (void);
#endif

