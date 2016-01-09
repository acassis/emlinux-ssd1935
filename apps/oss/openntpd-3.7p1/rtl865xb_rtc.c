/* ---------------------------------------------------------------------------
 * FILE NAME - [rtl865xb_rtc.c]
 * ---------------------------------------------------------------------------
 * ABSTRACT: Header file for RTL865xB's RTC support in NTPd
 *
 * PURPOSE: [Module/API purpose here]   
 *
 * NOTE: [Additional note]
 * ---------------------------------------------------------------------------
 * HISTORY
 *
 *		Max Yin	2005/3/24	new created
 *
 * ---------------------------------------------------------------------------
 * Copyright (c) 2004 U-MEDIA Communications, Inc.  All rights reserved.
 * 
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by U-MEDIA 
 * Communications, Inc. The Material contains trade secrets and proprietary 
 * and confidential information of U-MEDIA. No part of the codes may be used, 
 * copied, reproduced, modified, published, uploaded, posted, transmitted, 
 * distributed, or disclosed in any way without U-MEDIA's prior express 
 * written permission.
 * ---------------------------------------------------------------------------
 */ 
/* INCLUDE FILE DECLARATIONS */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rtl865xb_rtc.h"

/* NAMING CONSTANT DECLARATIONS */

/* STATIC SUBPROGRAM DECLARATIONS */

/* -----------------------------------------------------------------
 * ROUTINE NAME : RTL865xb_SyncWithSysTime
 * -----------------------------------------------------------------
 * FUNCTION: Synchronize RTL865xb's RTC with system time, plus offset
 *
 * PARAMETERS: 
 *	NONE
 * RETURN:  NONE
 * NOTE:    Time offset should be defined in envionmental variable: NTPTIME_OFF
 * -----------------------------------------------------------------
 */
void RTL865xb_SyncWithSysTime()
{
#ifdef MIPS_RTL865XB
	int memfd;
	unsigned int *rtcbase;
	unsigned int rtc2utc;
	time_t now;
	
	char *NTPTimeOff = getenv(ENVVAR_NTPTIME_OFF);
	int time_off = 0;
	
	// Convert time offset from environmental variable
	if(NTPTimeOff!=NULL){
		time_off = atoi(NTPTimeOff);
	}
	
	// Map RTC's address 
	memfd = open("/dev/mem", O_RDWR);
	rtcbase = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, memfd,  0x1D012000);
	close(memfd);
	if(rtcbase != (void*)-1){
		printf("%s(): RTC time: %s", __PRETTY_FUNCTION__, ctime(&rtcbase[18]));
		rtc2utc = rtcbase[18] - time_off;
		now = time(NULL);
		if(rtc2utc != now){
			now += time_off ;
			rtcbase[18] = now;
			printf("%s(): adjust time to %s", __PRETTY_FUNCTION__, ctime(&now));
		}
		munmap(rtcbase, 4096);
	}
#endif	
}

