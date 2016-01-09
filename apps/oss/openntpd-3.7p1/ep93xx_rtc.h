/* ---------------------------------------------------------------------------
 * FILE NAME - [ep93x_rtc.h]
 * ---------------------------------------------------------------------------
 * ABSTRACT: Header file for Cirrus EP93xx's RTC support in NTPd
 *
 * PURPOSE: [Module/API purpose here]   
 *
 * NOTE: [Additional note]
 * ---------------------------------------------------------------------------
 * HISTORY
 *
 *		Max Yin	2005/3/24	new created
 *		Max Yin	2005/8/8	ported from RTL865xB's project
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
#ifndef EP93xx_RTC_H
#define EP93xx_RTC_H

/* INCLUDE FILE DECLARATIONS */ 

/* Naming Constant declaration */ 
#define ENVVAR_NTPTIME_OFF	"NTPTIME_OFF"

/* Strucure/class declaration */


/* EXPORTED SUBPROGRAM DECLARATIONS */          
#ifdef __cplusplus
extern "C" {
#endif	
/* -----------------------------------------------------------------
 * ROUTINE NAME : EP93xx_GetGMTTime
 * -----------------------------------------------------------------
 * FUNCTION: Get RTC time and adjust to GMT time
 *
 * PARAMETERS: 
 *	NONE
 * RETURN:  GMT time
 * NOTE:    Time offset should be defined in envionmental variable: NTPTIME_OFF
 * -----------------------------------------------------------------
 */
unsigned int EP93xx_GetGMTTime();
	
/* -----------------------------------------------------------------
 * ROUTINE NAME : EP93xx_SetTime
 * -----------------------------------------------------------------
 * FUNCTION: Set RTC time
 *
 * PARAMETERS: 
 *	int now: GMT time
 * RETURN:  NONE
 * NOTE:    Time offset should be defined in envionmental variable: NTPTIME_OFF
 * -----------------------------------------------------------------
 */	
void EP93xx_SetTime(int now);
		
/* -----------------------------------------------------------------
 * ROUTINE NAME : EP93xx_AdjTime
 * -----------------------------------------------------------------
 * FUNCTION: Adjust RTC time
 *
 * PARAMETERS: 
 *	int diff: time difference
 * RETURN:  NONE
 * NOTE:    Time offset should be defined in envionmental variable: NTPTIME_OFF
 * -----------------------------------------------------------------
 */
void EP93xx_AdjTime(int diff);
		
/* -----------------------------------------------------------------
 * ROUTINE NAME : EP93xx_SyncWithSysTime
 * -----------------------------------------------------------------
 * FUNCTION: Synchronize ep93xx's RTC with system time, plus offset
 *
 * PARAMETERS: 
 *	NONE
 * RETURN:  NONE
 * NOTE:    Time offset should be defined in envionmental variable: NTPTIME_OFF
 * -----------------------------------------------------------------
 */
void EP93xx_SyncWithSysTime();

#ifdef __cplusplus
}
#endif
	
#endif /* RTL865XB_RTC_H */

