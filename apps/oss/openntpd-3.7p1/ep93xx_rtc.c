/* ---------------------------------------------------------------------------
 * FILE NAME - [ep93xx_rtc.c]
 * ---------------------------------------------------------------------------
 * ABSTRACT: Header file for EP93xx's RTC support in NTPd
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
/* INCLUDE FILE DECLARATIONS */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>

#include "ep93xx_rtc.h"
//#include "sem.h"
//#include "I2C.h"
/* NAMING CONSTANT DECLARATIONS */
//static int I2C_DS1337_init	= 0;
/* STATIC SUBPROGRAM DECLARATIONS */

int i2c_rtc_readbyte(const int i2cfd, unsigned char device_id, unsigned addr, unsigned char *data)
{
	struct i2c_rdwr_ioctl_data msgset;
	struct i2c_msg msgs[2];
	char reg_addr = addr;
	
	msgs[0].addr = device_id>>1;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = &reg_addr;
	
	msgs[1].addr = device_id>>1;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = 1;
	msgs[1].buf = data;
	
	msgset.msgs = msgs;
	msgset.nmsgs = 2;
	
	if(ioctl(i2cfd, I2C_RDWR, &msgset)<0){
		perror("I2C RDWR ioctl fail");
		return -1;
	}
	return 0;
}

int i2c_rtc_writebyte(const int i2cfd, unsigned char device_id, unsigned addr, unsigned char *data)
{
	struct i2c_rdwr_ioctl_data msgset;
	struct i2c_msg msgs[2];
	char reg_addr = addr;
	unsigned char reg_buf = data;
	
	msgs[0].addr = device_id>>1;
	msgs[0].flags = 0;
	msgs[0].len = 1;
	msgs[0].buf = &reg_addr;
	
	msgs[1].addr = device_id>>1;
	msgs[1].flags = I2C_M_NOSTART;
	msgs[1].len = 1;
	msgs[1].buf = &data;
	
	msgset.msgs = msgs;
	msgset.nmsgs = 2;
	
	if(ioctl(i2cfd, I2C_RDWR, &msgset)<0){
		perror("I2C RDWR ioctl fail");
		return -1;
	}
	return 0;
}


#define UIDS1337_DEVID 0xD0
static time_t EP93xx_ReadDS1337()
{
	struct tm tm;
	time_t now;
	unsigned char ds1337_data;
	memset(&tm, 0 , sizeof(tm));
	int i2cfd = open("/dev/i2c-0", O_RDWR);
	if(i2cfd==-1){
		perror("Cannot open I2C bus");
		return -1;
	}
	if(i2c_rtc_readbyte(i2cfd, UIDS1337_DEVID,0,&ds1337_data)!=0)// Read seconds		
		return -1;
	tm.tm_sec = ((ds1337_data>>4) & 0x07)*10 + (ds1337_data&0x0F);
	
	i2c_rtc_readbyte(i2cfd, UIDS1337_DEVID,1,&ds1337_data); // Read minutes
	tm.tm_min = ((ds1337_data>>4) & 0x07)*10 + (ds1337_data&0x0F);
	
	i2c_rtc_readbyte(i2cfd, UIDS1337_DEVID,2,&ds1337_data); // Read hour
	tm.tm_hour = ((ds1337_data>>4) & 0x03)*10 + (ds1337_data&0x0F);
	
	i2c_rtc_readbyte(i2cfd, UIDS1337_DEVID,4,&ds1337_data); // Read day
	tm.tm_mday = ((ds1337_data>>4) & 0x03)*10 + (ds1337_data&0x0F);
	
	i2c_rtc_readbyte(i2cfd, UIDS1337_DEVID,5,&ds1337_data); // Read month
	tm.tm_mon = ((ds1337_data>>4) & 0x01)*10 + (ds1337_data&0x0F) - 1;
	
	i2c_rtc_readbyte(i2cfd, UIDS1337_DEVID,6,&ds1337_data); // Read year
	tm.tm_year = 2000 + ((ds1337_data>>4) & 0x0F)*10 + (ds1337_data&0x0F) - 1900;
	now = mktime(&tm);
	close(i2cfd);
	printf("NTP: ReadDS1337: %s\n", asctime(&tm));
	return now;
}

static int EP93xx_WriteDS1337(time_t time)
{
	struct tm *tm = localtime(&time);
	unsigned char ds1337_data;
	int i2cfd = open("/dev/i2c-0", O_RDWR);
	if(i2cfd==-1){
		perror("Cannot open I2C bus");
		return -1;
	}
	// set year
	ds1337_data = tm->tm_year - 100; // tm.tm_year is the year from 1900;
	ds1337_data = (ds1337_data/10)<<4 | (ds1337_data%10);
	if(i2c_rtc_writebyte(i2cfd, UIDS1337_DEVID,6,ds1337_data)!=0)
		return -1;
	
	//set month
	ds1337_data = ((tm->tm_mon+1)/10)<<4 | (tm->tm_mon+1)%10 ; 
	i2c_rtc_writebyte(i2cfd, UIDS1337_DEVID,5,ds1337_data);
	
	//set day
	ds1337_data = ((tm->tm_mday)/10)<<4 | (tm->tm_mday)%10 ; 
	i2c_rtc_writebyte(i2cfd, UIDS1337_DEVID,4,ds1337_data);
	
	//set hour
	ds1337_data = ((tm->tm_hour)/10)<<4 | (tm->tm_hour)%10 ; 
	i2c_rtc_writebyte(i2cfd, UIDS1337_DEVID,2,ds1337_data);
	
	//set min
	ds1337_data = ((tm->tm_min)/10)<<4 | (tm->tm_min)%10 ; 
	i2c_rtc_writebyte(i2cfd, UIDS1337_DEVID,1,ds1337_data);
	
	//set sec
	ds1337_data = ((tm->tm_sec)/10)<<4 | (tm->tm_sec)%10 ; 
	i2c_rtc_writebyte(i2cfd, UIDS1337_DEVID,0,ds1337_data);
	close(i2cfd);
	return 0;
}

#define PCF8563_DEVID 0xA2
static time_t EP93xx_ReadPCF8563()
{
	struct tm tm;
	time_t now;
	unsigned char pcf8563_data;
	memset(&tm, 0 , sizeof(tm));
	int i2cfd = open("/dev/i2c-0", O_RDWR);
	if(i2cfd==-1){
		perror("Cannot open I2C bus");
		return -1;
	}
	if(i2c_rtc_readbyte(i2cfd, PCF8563_DEVID,0x02,&pcf8563_data)!=0) // Read seconds		
		return -1;
	tm.tm_sec = ((pcf8563_data&0x70)>>4)*10 + (pcf8563_data&0x0F);
	
	i2c_rtc_readbyte(i2cfd, PCF8563_DEVID,0x03,&pcf8563_data); // Read minutes
	tm.tm_min = ((pcf8563_data&0x70)>>4)*10 + (pcf8563_data&0x0F);
	
	i2c_rtc_readbyte(i2cfd, PCF8563_DEVID,0x04,&pcf8563_data); // Read hour
	tm.tm_hour = ((pcf8563_data&0x30)>>4)*10 + (pcf8563_data&0x0F);
	
	i2c_rtc_readbyte(i2cfd, PCF8563_DEVID,0x05,&pcf8563_data); // Read day
	tm.tm_mday = ((pcf8563_data&0x30)>>4)*10 + (pcf8563_data&0x0F);
	
	i2c_rtc_readbyte(i2cfd, PCF8563_DEVID,0x07,&pcf8563_data); // Read month
	tm.tm_mon = ((pcf8563_data&0x10)>>4)*10 + (pcf8563_data&0x0F);
	
	int centry = (pcf8563_data&0x80)?1900:2000; 
	i2c_rtc_readbyte(i2cfd, PCF8563_DEVID,0x08,&pcf8563_data); // Read year
	tm.tm_year = ((pcf8563_data&0xF0)>>4)*10 + (pcf8563_data&0x0F);
	tm.tm_year = (centry + tm.tm_year)-1900;
	now = mktime(&tm);
	close(i2cfd);
	printf("NTP: ReadPCF8563: %s\n", asctime(&tm));
	return now;
}

static int EP93xx_WritePCF8563(time_t time)
{
	struct tm *tm = localtime(&time);
	unsigned char pcf8563_data;
	int i2cfd = open("/dev/i2c-0", O_RDWR);
	if(i2cfd==-1){
		perror("Cannot open I2C bus");
		return -1;
	}
	int centry=0;
	// set year
	if(tm->tm_year>100){// tm.tm_year is the year from 1900;
		tm->tm_year -= 100;
		centry = 0;
	}
	else{
		centry = 1;
	}
	pcf8563_data = ((tm->tm_year/10)<<4) | (tm->tm_year%10) ;
	
	if(i2c_rtc_writebyte(i2cfd, PCF8563_DEVID,0x08,pcf8563_data)!=0)
		return -1;
	
	//set month
	pcf8563_data = ((tm->tm_mon/10)<<4)|(tm->tm_mon%10);
	pcf8563_data |= (centry<<7);
	i2c_rtc_writebyte(i2cfd, PCF8563_DEVID,0x07,pcf8563_data);
	
	//set day
	pcf8563_data = ((tm->tm_mday/10)<<4)|(tm->tm_mday%10);
	i2c_rtc_writebyte(i2cfd, PCF8563_DEVID,0x05,pcf8563_data);
	
	//set hour
	pcf8563_data = ((tm->tm_hour/10)<<4) | (tm->tm_hour%10);
	i2c_rtc_writebyte(i2cfd, PCF8563_DEVID,0x04,pcf8563_data);
	
	//set min
	pcf8563_data = ((tm->tm_min/10)<<4) | (tm->tm_min%10); 
	i2c_rtc_writebyte(i2cfd, PCF8563_DEVID,0x03,pcf8563_data);
	
	//set sec
	pcf8563_data = ((tm->tm_sec/10)<<4) | (tm->tm_sec%10) ; 
	i2c_rtc_writebyte(i2cfd, PCF8563_DEVID,0x02,pcf8563_data);
	close(i2cfd);
	return 0;
}

static time_t (*EP93xx_ReadRTC)() = NULL;
static int (*EP93xx_WriteRTC)(time_t t)=NULL;


static void EP93xx_TestRTC()
{
	if(EP93xx_ReadRTC == NULL){
		if(EP93xx_ReadDS1337()==-1){
			EP93xx_ReadRTC = EP93xx_ReadPCF8563;
			EP93xx_WriteRTC = EP93xx_WritePCF8563;
		}
		else{
			EP93xx_ReadRTC = EP93xx_ReadDS1337;
			EP93xx_WriteRTC = EP93xx_WriteDS1337;
		}
	}
}

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
unsigned int EP93xx_GetGMTTime()
{
#ifdef ARM_EP93xx
	unsigned int rtc2utc;

	char *NTPTimeOff = getenv(ENVVAR_NTPTIME_OFF);
	int time_off = 0;
	
	// Convert time offset from environmental variable
	if(NTPTimeOff!=NULL){
		time_off = atoi(NTPTimeOff);
	}
#if 1
	EP93xx_TestRTC();
	rtc2utc = EP93xx_ReadRTC();
	if(rtc2utc == -1)
		return 0; // both RTCs are not installed
#else
	rtc2utc = EP93xx_ReadDS1337();
	if(rtc2utc == -1){
		rtc2utc = EP93xx_ReadPCF8563();
		if(rtc2utc == -1)
			return 0; // both RTCs are not installed
	}
#endif
	rtc2utc -= time_off;
	return rtc2utc;
	
#else
#error Not EP93xx platform!
#endif
}

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
void EP93xx_SetTime(int now)
{
#ifdef ARM_EP93xx
	char *NTPTimeOff = getenv(ENVVAR_NTPTIME_OFF);
	int time_off = 0;
	
	// Convert time offset from environmental variable
	if(NTPTimeOff!=NULL){
		time_off = atoi(NTPTimeOff);
	}
	
	now += time_off ;
#if 1
	EP93xx_TestRTC();
	EP93xx_WriteRTC(now);
#else
	if(EP93xx_WriteDS1337(now)==-1)
		EP93xx_WritePCF8563(now);
#endif
	printf("%s(): set time (+ offset: %d s) to %s", 
		   __PRETTY_FUNCTION__, time_off, ctime(&now));
	
#else
#error Not EP93xx platform!
#endif
}

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
void EP93xx_AdjTime(int diff)
{
#ifdef ARM_EP93xx
	if(diff!=0 && (diff > -10 && diff < 10)){
		unsigned int now;
#if 1
		EP93xx_TestRTC();
		now = EP93xx_ReadRTC();
		if(now == -1)
			return ; // both RTCs are not installed
#else
		now = EP93xx_ReadDS1337();
		if(now == -1){
			now = EP93xx_ReadPCF8563();
			if(now == -1)
				return ; // both RTCs are not installed
		}
#endif
		now += diff ;
#if 1
		EP93xx_WriteRTC(now);
#else
		if(EP93xx_WriteDS1337(now)==-1)
			EP93xx_WritePCF8563(now);
#endif
		printf("%s(): adjust time to %s", __PRETTY_FUNCTION__, ctime(&now));
	}
#else
#error Not EP93xx platform!
#endif
}

/* -----------------------------------------------------------------
 * ROUTINE NAME : EP93xx_SyncWithSysTime
 * -----------------------------------------------------------------
 * FUNCTION: Synchronize EP93xx's RTC with system time, plus offset
 *
 * PARAMETERS: 
 *	NONE
 * RETURN:  NONE
 * NOTE:    Time offset should be defined in envionmental variable: NTPTIME_OFF
 * -----------------------------------------------------------------
 */
void EP93xx_SyncWithSysTime()
{
#ifdef ARM_EP93xx
	unsigned int rtc2utc;
	time_t now;
	/*
	if(!I2C_DS1337_init)
		I2C_Init();
	*/
	char *NTPTimeOff = getenv(ENVVAR_NTPTIME_OFF);
	int time_off = 0;
	
	// Convert time offset from environmental variable
	if(NTPTimeOff!=NULL){
		time_off = atoi(NTPTimeOff);
	}
#if 1
	EP93xx_TestRTC();
	rtc2utc = EP93xx_ReadRTC();
	if(rtc2utc == -1)
		return;
#else
	rtc2utc = EP93xx_ReadDS1337();
	if(rtc2utc == -1){
		rtc2utc = EP93xx_ReadPCF8563();
		if(rtc2utc == -1)
			return ; // both RTCs are not installed
	}
#endif
	rtc2utc -= time_off;

	now = time(NULL);
	if(rtc2utc != now){
		now += time_off ;
#if 1
		EP93xx_WriteRTC(now);
#else
		if(EP93xx_WriteDS1337(now)==-1)
			EP93xx_WritePCF8563(now);
#endif
		printf("%s(): adjust time to %s", __PRETTY_FUNCTION__, ctime(&now));
		//EP93xx_ReadPCF8563();
	}
#endif	
}

