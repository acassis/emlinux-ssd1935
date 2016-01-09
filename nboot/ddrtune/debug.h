#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef MAGUS_LIB
#define NPRINTF		1
#include "magus.h"
#else
#define NPRINTF		0
#endif

#if (NPRINTF == 1)
#if (TAPEOUTVER == 3)
#define flush()
#endif
#define dbg(s, ...)
#define print(x)	do {puts(x); flush();} while(0)
//#define print	printf
#else
#include "stdio.h"
#define dbg	printf
#define print	printf
#endif

#endif	// __DEBUG_H__

