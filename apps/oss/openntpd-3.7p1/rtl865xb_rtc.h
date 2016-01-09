/* ---------------------------------------------------------------------------
 * FILE NAME - [rtl865xb_rtc.h]
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
#ifndef RTL865XB_RTC_H
#define RTL865XB_RTC_H

/* INCLUDE FILE DECLARATIONS */ 

/* Naming Constant declaration */ 
#define ENVVAR_NTPTIME_OFF	"NTPTIME_OFF"

/* Strucure/class declaration */


/* EXPORTED SUBPROGRAM DECLARATIONS */          
#ifdef __cplusplus
extern "C" {
#endif	
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
void RTL865xb_SyncWithSysTime();

#ifdef __cplusplus
}
#endif
	
#endif /* RTL865XB_RTC_H */

