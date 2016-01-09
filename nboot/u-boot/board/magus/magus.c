/*
 * board/magus/magus.c
 *
 * (c) Copyright 2007
 * Solomon Systech Ltd
 *
 * Sasi Nair <sasin@solomon-systech.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

DECLARE_GLOBAL_DATA_PTR;

#define REGD(a)		(*(volatile uint32_t *)(a))


int board_init (void)
{
	gd->bd->bi_arch_number = 1933;
	gd->bd->bi_boot_params = 0x8C000100;	/* adress of boot parameters	*/
	icache_enable();

#if 0
	/* CS0-5: CS0-SRAM, CS4-LAN */
	REGD(0x0800100c) = 0xff;
	REGD(0x08001010) = 0x1d0020;
	REGD(0x08001020) = 0x1d0021;
	REGD(0x08001030) = 0x1d0021;
	REGD(0x08001040) = 0x1d0021;
	REGD(0x08001050) = 0x1d0020;	/* LAN needs more cycles, no */
	REGD(0x08001054) = 0x111122;
#endif
	return 0;
}


int board_late_init (void)
{
	setenv ("stdout", "serial");
	setenv ("stderr", "serial");
	return 0;
}


int dram_init (void)
{
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
#if 0
	sdramc_start();
#endif
	return 0;
}


void lowlevel_init(void) { }

void reset_cpu(unsigned long adr) { }

#include "sdr.h"

int sdramc_start (void)
{
	sdr_cfg_t cfg;

	/* enable lvcmos */
	REGD(0x08100828) = 0x02000000;

	if (sdr_init (SDR_REG_BASE) < 0)
	{
		printf ("sdramc initialisation failed\n");
		return -1;
	}

	if (sdr_reset () < 0)
	{
		printf ("sdramc reset failed\n");
		return -1;
	}

	sdr_enable ();	  // clk running, cke low for 200us
	udelay (200);
	sdr_power_up ();	// clk running, cke high for 200us
	udelay (200);

	// configure cs0
	cfg.cs = 0;
	cfg.dsz = 1;	// 1=32-bit, 0=16-bit
	cfg.row = 13;
	cfg.col = 9;
	cfg.bl = 8;
	cfg.cas = 3;

	if (sdr_start (&cfg) < 0)
	{
		printf ("sdramc csd0 start up failed\n");
		return -1;
	}

	// configure cs1
	cfg.cs = 1;
	if (sdr_start (&cfg) < 0)
	{
		printf ("sdramc csd1 start up failed\n");
		return -1;
	}

	return 0;
}

