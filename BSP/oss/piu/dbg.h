#ifndef _DBG_SASI
#define _DBG_SASI


int dbg(char *, ...);
#ifndef __KERNEL__
int	printf(const char *, ...);
#define dbg printf
#else
int	printk(const char *, ...);
#define dbg printk
#endif


#endif

