/*
 * include/configs/magus.h
 *
 * (c) Copyright 2007
 * Solomon Systech Ltd.
 *
 * Sasi <sasin@solomon-systech.com>
 *
 * This is the Configuration setting for Solomon Magus board
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

#ifndef __CONFIG_H
#define __CONFIG_H

#include "../../../include/config.h"

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_ARM926EJS		1	/* This is an ARM926EJS Core		*/
#define CONFIG_MAGUS			1	/* on a Solomon Magus Board			*/
#undef CONFIG_USE_IRQ				/* we don't need IRQ/FIQ stuff		*/
//#define DEBUG
#define CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_SKIP_RELOCATE_UBOOT

#if defined CONFIG_ACCIO_P1_LITE
#define CONFIG_LOAD_BASE_ADDR		0x50400000 /*0x50600000*/ /*0x50C00000*/
#else
#define CONFIG_LOAD_BASE_ADDR		0x51000000
#endif

/*
 * Size of malloc() pool
 */

#define CFG_MALLOC_LEN		(CFG_ENV_SIZE + 128*1024)


#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

#if 0
/*
 *  CS8900 Ethernet drivers
 */
//#define CONFIG_ETHADDR			02:04:06:08:0A:0C
#define CONFIG_IPADDR			192.168.22.66
#define CONFIG_DRIVER_CS8900	1	/* we have a CS8900 on-board */
#define CS8900_BASE				0x2C000000
#define CS8900_BUSMAGUS			1	/* addresses 32-bit, data 16-bit */
#endif

/*
 * select serial console configuration
 */

/* #define CONFIG_UART1			*/
/* #define CONFIG_UART2		1	*/
#define CONFIG_CONS_INDEX	1
#define CFG_NS16550
#define CFG_NS16550_SERIAL
#define CFG_NS16550_REG_SIZE	4
#define CFG_NS16550_CLK		60000000
#define CFG_NS16550_COM1	0x0810300C
#define CONFIG_BAUDRATE		115200
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }


/***********************************************************
 * Command definition
 ***********************************************************/

#define CONFIG_COMMANDS \
	((CONFIG_CMD_DFL & ~(CFG_CMD_AUTOSCRIPT | CFG_CMD_IMLS | CFG_CMD_FLASH | CFG_CMD_NET)) \
	| CFG_CMD_I2C | CFG_CMD_EEPROM \
	| CFG_CMD_REGINFO)

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#define CONFIG_BOOTDELAY	0
//#define CONFIG_BOOTARGS		"root=/dev/mtdblock2 mem=44M console=ttyS0,115200n8 init=/init"
#define CONFIG_BOOTFILE		"uImage"
#define CFG_LOAD_ADDR		(CONFIG_LOAD_BASE_ADDR + 0x7FC0)	/* default load address */
#define CONFIG_BOOTCOMMAND	"bootm"

/*
 * Miscellaneous configurable options
 */

#define CFG_HUSH_PARSER		1
#define CFG_PROMPT_HUSH_PS2	"> "

#define CFG_LONGHELP				/* undef to save memory		*/

#ifdef CFG_HUSH_PARSER
#define CFG_PROMPT		"Magus$ "	/* Monitor Command Prompt */
#else
#define CFG_PROMPT		"Magus=> "	/* Monitor Command Prompt */
#endif

#define CFG_CBSIZE		256		/* Console I/O Buffer Size	*/
#define CFG_PBSIZE		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16)
						/* Print Buffer Size */
#define CFG_MAXARGS		16		/* max number of command args	*/
#define CFG_BARGSIZE	CFG_CBSIZE	/* Boot Argument Buffer Size	*/

#define CFG_MEMTEST_START	0x10000000	/* memtest works on	*/
#define CFG_MEMTEST_END		0x001F0000	/* 63 MB in DRAM	*/

#undef	CFG_CLKS_IN_HZ				/* everything, incl board info, in Hz */
#define	CFG_HZ				1000

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */

#define CONFIG_NR_DRAM_BANKS	1		/* we have 1 bank of SDRAM	*/
#define PHYS_SDRAM_1		0x50000000	/* SDRAM  on CSD0		*/
#define PHYS_SDRAM_1_SIZE	0x04000000	/* 64 MB			*/

//#define CFG_MAX_FLASH_BANKS	1		/* 1 bank of SyncFlash		*/
//#define CFG_FLASH_BASE		0x0C000000	/* SyncFlash on CSD1		*/
//#define FLASH_BANK_SIZE		0x01000000	/* 16 MB Total			*/

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */

#define CFG_RAM_BOOT
//#define CONFIG_SYNCFLASH	1
//#define PHYS_FLASH_SIZE		0x01000000
//#define CFG_MAX_FLASH_SECT	(16)
//#define CFG_ENV_ADDR		(CFG_FLASH_BASE+0x00ff8000)

//#define CFG_ENV_IS_IN_FLASH	1
#define CFG_NO_FLASH
#define CFG_ENV_IS_NOWHERE	1
#define CFG_ENV_SIZE		0x04000 /* Total Size of Environment Sector */
#define CFG_ENV_ADDR		0x52F30000
//#define CFG_ENV_ADDR		0x10030000
//#define CFG_ENV_SECT_SIZE	0x100000

/*-----------------------------------------------------------------------
 * Enable passing ATAGS
 */

#define CONFIG_CMDLINE_TAG		1	/* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS	1

#define CONFIG_SYS_CLK_FREQ		16780000
#define CONFIG_SYSPLL_CLK_FREQ	16000000

/* i2C */
#define CONFIG_HARD_I2C			1
#define CFG_I2C_SPEED			115200
#define CFG_I2C_SLAVE			0x7F

/* EEPROM */
#define CFG_I2C_EEPROM_ADDR		0x50
#define CFG_I2C_EEPROM_ADDR_LEN		1
//#define CFG_EEPROM_PAGE_WRITE_BITS	4
//#define CFG_EEPROM_SIZE			2048
#define CFG_EEPROM_PAGE_WRITE_BITS	3
#define CFG_EEPROM_SIZE			256

#define BOARD_LATE_INIT			1
//#define MLC_NAND_ASSURANCE		1


#if 0
#define CONFIG_LCD			1
#define CONFIG_SHARP_16x9		1
#define CONFIG_SPLASH_SCREEN		1
#endif

#endif	/* __CONFIG_H */
