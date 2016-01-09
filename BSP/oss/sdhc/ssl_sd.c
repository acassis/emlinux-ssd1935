/*
 *  linux/drivers/mmc/ssl_sd.c - Solomon Systech Ltd SDHC driver
 *
 *  Copyright (C) 2005 Ambat Sasi Nair, Solomon Systech Ltd
 *
 *  derived from pxamci.c by Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/blkdev.h>
#include <linux/dma-mapping.h>
#include <linux/mmc/host.h>
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/sd.h>

#include <asm/dma.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/sizes.h>

#include "sdhc.h"
#include "sd.h"

#if IO_MAP == 3
#warning "using ebm interface"
#include "ebm_mem.h"
#include "io.h"
#endif

#if INTC
#warning "using intc as a module"
#include "intck.h"
#define request_irq	intc_req
#define free_irq	intc_free
#define enable_irq	intc_enable
#define disable_irq	intc_disable
#endif

static uint32_t	clk = MAGUS_CLK_SDHC << 1;	/* base SD clock */
static uint32_t	tout;						/* timeout SD clock */
static uint32_t	sw;							/* switches */
static uint32_t sd_clk=25;						/* SD clock */
module_param(clk, uint, 0);
module_param(tout, uint, 0);
module_param(sw, uint, 0);
module_param(sd_clk, uint, 0664);
static int sdhc_irq;

//#define SD_1BIT

//#define SD_SIMPNP 	/* insert device onto bus for simulated plug & play */

#if IO_MAP == 3 && defined SD_DMA
void	*ebm_mem;
#endif

#ifdef SD_SIMPNP
static struct resource _rc[2];
#endif


#if 0
void ddump(void *p2, int len)
{
	char	*p = (char *)p2;
	int	i;

	for (i = 0; i < len; i++)
	{
		printk("%02X ", *p++);
		if ((i & 15) == 15)
		{
			printk("\n");
		}
	}
	if (!(i & 15))
	{
		printk("\n");
	}
}
#endif


#define DRV_NAME "sdhc"


typedef struct
{
	struct mmc_host		*mmc;
	struct resource		*res;
	int					irq;

	sdhc_t				hw;
	sd_cmd_t			tcmd;
	sd_dat_t			tdat;
	struct mmc_command	*pcmd;

	struct scatterlist	*sg;	/* current sg index */
	struct scatterlist	*sgn;	/* last sg index */
	int					sgc;	/* total mapped sg count */
	int					sgofs;	/* offset into sg for retry */
	struct timer_list	timer;
	int					retry;
}
sslsd_host;


static void sslsd_map(struct scatterlist *sg, sd_cmd_p c, sd_dat_p d, int ofs)
{
	uint32_t	buf;

#if SD_DMA
#warning "dma version"
	if (c->flag & SDCMD_F_DMA)
	{
#if IO_MAP == 1
		buf = sg_dma_address(sg);
#else
		if (c->flag & SDCMD_F_WR)
		{
			/* pre-copy for EBM */
			io_wrs(ebm_mem, sg_virt(sg), sg_dma_len(sg));
		}
		/* physical memory local address */
		buf = ebm_mem;
#endif	// IO_MAP
	}
	else
#endif	// SD_DMA
	{
		buf = (uint32_t)sg_virt(sg);
	}
	d->buf = (void *)(buf + ofs);

	/* hardware has no SG support, so break it into multiple xfers */
	buf = (sg_dma_len(sg) - ofs) / d->blk;
	if (buf >= (1 << 16))
	{
		printk("sslsd: map err - count %d blk %d\n", buf, d->blk);
	}
	d->count = buf;
}


#define SSLSD_CARD_BLKADR	1
#define SSLSD_CARD_SDIO		2

inline static int sslsd_card_type(struct mmc_card *card)
/**<
this is a safe card type check as during initialization, there is no card
@param[in]	card	card
@return		SSLSD_CARD_XXX bitmap flag
*/
{
	int	flag;

	if (!card)
	{
		return 0;
	}
	flag = 0;
	if (mmc_card_sdio(card))
	{
		flag = SSLSD_CARD_SDIO;
	}
	if (mmc_card_blockaddr(card))
	{
		flag |= SSLSD_CARD_BLKADR;
	}
	return flag;
}


static void sslsd_cmd_done(sslsd_host *host, sd_cmd_p c)
{
	struct mmc_command	*cmd;

	del_timer(&host->timer);
	cmd = host->pcmd;

//printk("sslsd: cmd_done info - cmd=%d arg=%08X flag=%02X rt=%d\n", 
//c->cmd, c->arg, c->flag, c->rt);

	if (mmc_resp_type(cmd) != MMC_RSP_R2)
	{
		struct mmc_data		*d;

		d = cmd->data;
		cmd->resp[0] = c->rsp;
//printk("sslsd: cmd_done info - resp %08X\n", c->rsp);
		if (d)
		{
			struct scatterlist	*sg;
			sd_dat_p			dat;
			int					len;
			int					card;

			card = sslsd_card_type(host->mmc->card);
			dat = c->dat;
			len = dat->actual;
			if (!len)
			{
				if (!host->retry || (card & SSLSD_CARD_SDIO))
				{
					goto l_done;
				}
				host->retry = 0;
			}

			/* ignore any errors & retry as long as there is progress */
			c->rt = 0;
			sg = host->sg;
			len *= dat->blk;
#if SD_DMA && IO_MAP==3
			/* copy read data from EBM buffer */
			io_rds(ebm_mem + host->sgofs, sg_virt(sg) + host->sgofs, len);
#endif
			d->bytes_xfered += len;

			if (card & SSLSD_CARD_SDIO)
			{
				/* not sure if we can continue transfer from last point */
				goto l_done;
			}

#ifndef NSD_HANDLE_SG_N_ERR
			/* do remainder of scatter page */
			if (dat->actual < dat->count)
			{
				host->sgofs += len;
				goto l_next;
			}

			/* go to next scatter page */
			if (sg < host->sgn)
			{
				sg++;
				host->retry = 1;
				host->sg = sg;
				host->sgofs = 0;

l_next:
				c->arg += (card & SSLSD_CARD_BLKADR) ? dat->actual : len;
				sslsd_map(sg, c, dat, host->sgofs);
				if (sdhc_cmd(&host->hw, c))
				{
					goto l_done;
				}
				return;
			}
#endif

l_done:
#if SD_DMA && IO_MAP==1
			/* data xfer completed for scatter list - unmap it */
			if (c->flag & SDCMD_F_DMA)
			{
				dma_unmap_sg(mmc_dev(host->mmc), host->sg, host->sgc, 
					(c->flag & SDCMD_F_WR) ? DMA_TO_DEVICE : DMA_FROM_DEVICE);
			}
#endif
		}
	}

	if (c->rt)
	{
printk("sslsd: cmd_done err - cmd=%d arg=%08X fl=%02X rsp=%08X rt=%d\n", 
c->cmd, c->arg, c->flag, c->rsp, c->rt);
if (c->dat) printk("cnt=%d act=%d\n", c->dat->count, c->dat->actual);

		switch (c->rt)
		{
			case SDHC_RET_TOUT:
			case SDHC_RET_CMD_TOUT:
			case SDHC_RET_DAT_TOUT:
				cmd->error = -ETIMEDOUT;
				break;

			case SDHC_RET_DAT_LINE:
			case SDHC_RET_CMD_LINE:
				cmd->error = -EILSEQ;
				break;

			case SDHC_RET_REM:
				cmd->error = -ENOMEDIUM;
				break;

			default:
				cmd->error = -EIO;
				break;
		}
	}

	mmc_request_done(host->mmc, cmd->mrq);
}


static void sslsd_request(struct mmc_host *mmc, struct mmc_request *req)
{
	sslsd_host			*host = mmc_priv(mmc);
	struct mmc_command	*cmd = req->cmd;
	sd_cmd_p			c = &host->tcmd;
	struct mmc_data		*d;
	int					flag;
	unsigned long		iflags;

	if (!sdhc_is_in(&host->hw))
	{
		cmd->error = -ENOMEDIUM;
		mmc_request_done(mmc, req);
		return;
	}

	c->dat = 0;
	switch (mmc_resp_type(cmd))
	{
		case MMC_RSP_R1: /* & R5, R6 */
			flag = SDCMD_F_R1;
			break;

		case MMC_RSP_R1B: /* & R5b */
			flag = SDCMD_F_R1B;
			break;

		case MMC_RSP_R2:
			flag = SDCMD_F_R2;
			c->dat = &host->tdat;
			c->dat->buf = (uint8_t *)cmd->resp;
			break;

		case MMC_RSP_R3:
			flag = SDCMD_F_R3;
			break;

		default:
			flag = 0;
			break;
	}

	c->cmd = cmd->opcode;
	c->arg = cmd->arg;
	host->pcmd = cmd;

	d = cmd->data;
	if (d)
	{
		struct scatterlist	*sg;
		sd_dat_p			dat;

		if (d->flags & MMC_DATA_STREAM) 
		{
			/* not supported */
			cmd->error = -EINVAL;
			mmc_request_done(mmc, req);
			return;
		}

		flag |= SDCMD_F_DAT;
		if (d->flags & MMC_DATA_WRITE)
		{
			flag |= SDCMD_F_WR;
		}
		if (d->blocks > 1)
		{
			flag |= SDCMD_F_MULTI;
		}
#if SD_DMA
		if (host->hw.fdma)
		{
			flag |= SDCMD_F_DMA;
		}
#endif

		dat = c->dat = &host->tdat;
		dat->blk = d->blksz;
#if 1
		c->tout = (d->timeout_ns + 1000000 - 1) / 1000000 + 
			d->timeout_clks / (mmc->ios.clock / 1000);
#endif

		sg = d->sg;
		host->sg = sg;
		host->sgc = d->sg_len;
		host->sgn = sg + d->sg_len - 1;
		host->sgofs = 0;

#if SD_DMA && IO_MAP == 1
		if (flag & SDCMD_F_DMA)
		{
			int	count;

			count = dma_map_sg(mmc_dev(mmc), sg, d->sg_len, 
						(flag & SDCMD_F_WR) ? DMA_TO_DEVICE : DMA_FROM_DEVICE);
			if (!count)
			{
				/* failed to map even 1 page */
				cmd->error = -ENOMEM;
				mmc_request_done(mmc, req);
				return;
			}
			if (count != d->sg_len)
			{
				/* not all pages got mapped */
				count = d->sg_len - count;
				host->sgc -= count;
				host->sgn -= count;
			}
		}
#endif
		if (req->stop)
		{
			flag |= SDCMD_F_STOP;
		}
		c->flag = flag;
		sslsd_map(sg, c, dat, 0);
		host->retry = 1;
	}
	else
	{
		c->flag = flag;
	}

//printk("sslsd: cmd info - cmd=%d arg=%08X flags=%02X\n", 
//cmd->opcode, cmd->arg, flag);

	/* 2 sec timeout */
	mod_timer(&host->timer, jiffies + 2 * HZ);
	local_irq_save(iflags);
	flag = sdhc_cmd(&host->hw, c);
	local_irq_restore(iflags);
	if (flag)
	{
		sslsd_cmd_done(host, c);
	}
}


static irqreturn_t sslsd_irq(int irq, void *devid)
{
	sslsd_host	*host = devid;

	return sdhc_isr(&host->hw);
}


static void sslsd_evt(void *ctx, sd_cmd_p ev)
{
	sslsd_host	*host = ctx;

	if (ev > SDHC_EVT_MAX)
	{
		sslsd_cmd_done(host, ev);
	}
	else if (ev == SDHC_EVT_INS || ev == SDHC_EVT_REM)
	{
		mmc_detect_change(host->mmc, 0);
	}
	else if (ev == SDHC_EVT_IO)
	{
//printk("sslsd: evt err - IO interrupt\n");
		mmc_signal_sdio_irq(host->mmc);
	}
}


static void sslsd_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	sslsd_host		*host = mmc_priv(mmc);
	unsigned long	iflags;

//printk("sslsd: set_ios info - clk=%d vdd=%d bus=%d pwr=%d\n", 
//ios->clock, ios->vdd, ios->bus_mode, ios->power_mode);
#if !SD_DMA
/* slow down xfer for slow host if */
if (ios->clock)
{
//printk("%d->100KHz\n", ios->clock);
ios->clock = 100000; 
}
#endif
	local_irq_save(iflags);
	sdhc_set_clk(&host->hw, ios->clock ? (ios->clock / 100000) : 0);
	sdhc_set_pwr(&host->hw, ios->power_mode ? (1 << ios->vdd) : 0);
	sdhc_set_width(&host->hw, (ios->bus_width == MMC_BUS_WIDTH_4));
	local_irq_restore(iflags);
}


static int sslsd_get_ro(struct mmc_host *mmc)
{
	sslsd_host	*host = mmc_priv(mmc);

	return sdhc_is_wp(&host->hw);
}


static void sslsd_enable_sdio_irq(struct mmc_host *mmc, int enable)
{
	sslsd_host	*host = mmc_priv(mmc);
	unsigned long		iflags;

	local_irq_save(iflags);
	sdhc_ioirq_ena(&host->hw, enable);
	local_irq_restore(iflags);
}


static struct mmc_host_ops sslsd_ops = 
{
	.request	= sslsd_request,
	.set_ios	= sslsd_set_ios,
	.get_ro		= sslsd_get_ro,
	.enable_sdio_irq = sslsd_enable_sdio_irq,
};


static void sslsd_tout(unsigned long data)
{
	sslsd_host		*host = (sslsd_host *)data;
	unsigned long	iflags;

printk("%s\n", __FUNCTION__);
	local_irq_save(iflags);
	sdhc_flush(&host->hw);
	local_irq_restore(iflags);
}

static int sslsd_suspend(struct platform_device *dev, pm_message_t state);
static int sslsd_resume(struct platform_device *dev);
static ssize_t sslsd_show(struct device *dev,
			   struct device_attribute *attr, char *buf) 
{
	return sprintf(buf, "%d\n", dev->power.power_state.event);
}

static ssize_t sslsd_store(struct device *dev,
			   struct device_attribute *attr,
			   const char *buf, size_t count) 
{
	char *endp;
	int i;
	struct platform_device *pdev;
	pdev = container_of(dev, struct platform_device, dev);

	i = simple_strtol(buf, &endp, 10);

	if (i == dev->power.power_state.event)
	{
		printk("the same power state. \n");
		return count;
	}
	dev->power.power_state.event = i;
	if (i == PM_EVENT_SUSPEND)
		
	{
		sslsd_suspend(pdev, PMSG_SUSPEND);
	}
	else if (i == PM_EVENT_ON)
	{
		sslsd_resume(pdev);
	}
	else
	{
		dev->power.power_state.event = 0;
	}
	return count;
}

static DEVICE_ATTR(pm_ctrl, 0664, sslsd_show, sslsd_store);

static struct attribute *sslsd_attributes[] =
{ 
	&dev_attr_pm_ctrl.attr, NULL, 
};

static struct attribute_group sslsd_attr_group = { 
	.attrs = sslsd_attributes, 
};

static int sslsd_probe(struct platform_device *dev)
{
	struct mmc_host	*mmc;
	sslsd_host		*host;
	int				ret;
	int				irq;
	struct resource	*r;
	sd_clk = sd_clk * 1000000;

#if IO_MAP == 1
	r = platform_get_resource(dev, IORESOURCE_MEM, 0);
	irq = platform_get_resource(dev, IORESOURCE_IRQ, 0)->start;
#else
	r = _rc;
	irq = _rc[1].start;
#endif
	if (!r || irq == NO_IRQ)
	{
		printk("sslsd: probe err - resource\n");
		return -ENXIO;
	}

	mmc = mmc_alloc_host(sizeof(sslsd_host), &dev->dev);
	if (!mmc) 
	{
		printk("sslsd: probe err - alloc host\n");
		return -ENOMEM;
	}

	host = mmc_priv(mmc);
#if IO_MAP == 1
//	r = request_mem_region(r->start, 0x100, DRV_NAME);
	host->hw.r = (uint32_t)ioremap_nocache(r->start, r->end - r->start + 1);
	if (!host->hw.r)
	{
		printk("sslsd: probe err - ioremap\n");
		ret = -EBUSY;
		goto l_region;
	}
#else
	host->hw.r = r->start;
#endif
	host->hw.r += 0x100;

	if (!clk)
	{
		printk("sslsd: probe info - clk not passed it assuming 25MHz\n");
		clk = 25000000;
	}
	if (!tout)
	{
		tout = clk;
	}

	host->hw.evt = sslsd_evt;
	host->hw.baseclk = clk / 100000;
	host->hw.toutclk = tout / 1000;
	host->hw.ctx = host;
	ret = sdhc_init(&host->hw);
	if (ret)
	{
		printk("sslsd: probe err - sdhc_init\n");
		ret = -ENODEV;
		goto l_host;
	}

#if SD_DMA
	if (!host->hw.fdma)
	{
		printk("sslsd: probe warn - no dma support\n");
	}
	else
	{
#if IO_MAP == 3
		ebm_mem = ebm_malloc(0x1000);
#endif
	}
#endif

	mmc->ops = &sslsd_ops;
	mmc->f_min = 100000;
//	mmc->f_max = host->hw.fhspeed ? 50000000 : 25000000;
	mmc->f_max = host->hw.fhspeed ? 50000000 : sd_clk;
	mmc->ocr_avail = host->hw.ocr;
	mmc->caps = MMC_CAP_4_BIT_DATA | MMC_CAP_MULTIWRITE | MMC_CAP_SDIO_IRQ;
	if (host->hw.fhspeed)
	{
		mmc->caps |= MMC_CAP_SD_HIGHSPEED;
	}
#ifdef SD_1BIT
	mmc->caps &= ~MMC_CAP_4_BIT_DATA;
#endif
	mmc->max_hw_segs = 64;
	mmc->max_phys_segs = 64;
	mmc->max_seg_size = mmc->max_req_size = 512 * 1024;
	mmc->max_blk_size = 1 << (9 + host->hw.maxblksz);
	mmc->max_blk_count = 65535;

//mmc->max_hw_segs = mmc->max_phys_segs = 1;
//printk("sslsd: segs=%d blks=%d blksz=%d caps=%X\n", 
//mmc->max_hw_segs, mmc->max_blk_count, mmc->max_blk_size, mmc->caps);

	host->mmc = mmc;
	host->res = r;
	host->irq = irq;

	sdhc_irq = irq;

//printk("sslsd: probe info - req_irq %d\n", irq);
	ret = request_irq(irq, sslsd_irq, IRQF_DISABLED, DRV_NAME, host);
	if (ret)
	{
		printk("sslsd: probe err - req_irq %d err %d\n", irq, ret);
		goto l_init;
	}

	/* tout timer */
	init_timer(&host->timer);
	host->timer.data = (unsigned long)host;
	host->timer.function = sslsd_tout;

	platform_set_drvdata(dev, mmc);

	mmc_add_host(mmc);

	/* build sysfs */ 
{
	int error = sysfs_create_group(&dev->dev.kobj, &sslsd_attr_group);
	if (error)
	{
		printk("register sysfs for power is failure %d\n", error);
	}
}

	return 0;

l_init:
	sdhc_exit(&host->hw);
l_region:
#if IO_MAP == 1
//	release_resource(r);
	iounmap((void *)(host->hw.r - 0x100));
#endif
l_host:
	mmc_free_host(mmc);
	return ret;
}


static int sslsd_remove(struct platform_device *dev)
{
	struct mmc_host	*mmc = platform_get_drvdata(dev);
	sslsd_host		*host = mmc_priv(mmc);

//printk("sslsd: remove\n");
	del_timer(&host->timer);
	sdhc_exit(&host->hw);
	platform_set_drvdata(dev, NULL);
	mmc_remove_host(mmc);
	free_irq(host->irq, host);
#if IO_MAP == 1
//	release_resource(host->res);
	iounmap((void *)(host->hw.r - 0x100));
#endif
#if SD_DMA
	if (host->hw.fdma)
	{
#if IO_MAP == 3
		ebm_mfree(ebm_mem);
#endif
	}
#endif
	mmc_free_host(mmc);
	sysfs_remove_group(&dev->dev.kobj, &sslsd_attr_group);
	return 0;
}


//#ifdef CONFIG_PM
#if 1
static int sslsd_suspend(struct platform_device *dev, pm_message_t state)
{
	struct mmc_host	*mmc = platform_get_drvdata(dev);
	disable_irq(sdhc_irq);
//printk("sslsd: suspend level=%d\n", level);
	return mmc_suspend_host(mmc, state);
}


static int sslsd_resume(struct platform_device *dev)
{
	struct mmc_host *mmc = platform_get_drvdata(dev);
	enable_irq(sdhc_irq);
//printk("sslsd: resume level=%d\n", level);
	return mmc_resume_host(mmc);
}

#endif


static struct platform_driver sslsd_drv = 
{
	.driver =
	{
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
	.probe		= sslsd_probe,
	.remove		= sslsd_remove,
#ifdef CONFIG_PM
	.suspend	= sslsd_suspend,
	.resume		= sslsd_resume,
#endif
};


#ifdef SD_SIMPNP
static uint32_t	addr;
static int		irq;
module_param(addr, uint, 0);
module_param(irq, int, 0);
#endif


#ifdef SD_SIMPNP
static struct platform_device dv = 
{
	.name = DRV_NAME,
#if IO_MAP == 1
	.num_resources = 2,
	.resource = _rc,
#endif
};
#endif


static int __init sslsd_init(void)
{
#ifdef SD_SIMPNP
	int	rt;

	if (!irq || !addr)
	{
		printk("ssl_sd: init err - no irq or addr\n");
		return -1;
	}
	_rc[0].start = addr;
	_rc[0].end = addr + 0x100 - 1;
	_rc[0].flags = IORESOURCE_MEM;
	_rc[1].start = irq;
	_rc[1].end = irq;
	_rc[1].flags = IORESOURCE_IRQ;

	rt = platform_device_register(&dv);
	if (rt)
	{
		printk("ssl_sd: init err - plat_dev_reg\n");
		return rt;
	}
printk("addr=%08X irq=%d clk=%d\n", addr, irq, clk);
#endif
	return platform_driver_register(&sslsd_drv);
}


static void __exit sslsd_exit(void)
{
	platform_driver_unregister(&sslsd_drv);
#ifdef SD_SIMPNP
	platform_device_unregister(&dv);
#endif
}


module_init(sslsd_init);
module_exit(sslsd_exit);


MODULE_DESCRIPTION(DRV_NAME);
MODULE_AUTHOR("Ambat Sasi Nair, Solomon Systech Ltd");
MODULE_LICENSE("GPL");

