/*
 * NFTL mount code with extensive checks
 *
 * Author: Fabrice Bellard (fabrice.bellard@netgem.com)
 * Copyright (C) 2000 Netgem S.A.
 *
 * $Id: nftlmount.c,v 1.26 2008/09/04 02:47:46 jackylam Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifdef MAGUS_LINUX
#include <linux/kernel.h>
#include <asm/errno.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nftl.h>


//#define NFTL_DEBUG
#undef NFTL_DEBUG

#define nftl_dumpmsg(fmt, arg ...) //printk(KERN_ERR fmt, ##arg)


#ifdef NFTL_DEBUG
#define nftl_debug(fmt, arg ...)  printk(KERN_ERR fmt, ##arg)
#else
#define nftl_debug(fmt, arg ...) 
#endif

#define nftl_error(fmt, arg ...) printk(KERN_ERR fmt, ##arg)
#define nftl_info(fmt, arg ...) printk(KERN_INFO fmt, ##arg)
#define nftl_msg(fmt, arg ...)  ///printk(KERN_ERR fmt, ##arg)

#else


#include "dummy.h"

uint16_t gblock=0;
uint8_t gpage=0;
int test_flag=0;

#define nftl_error		printk
#define nftl_msg		printk
#define nftl_dumpmsg	printk

#endif



//static uint8_t page_buffer[NAND_MAX_PAGESIZE];


#ifdef MTD_PAGE_2K

	#define MAX_PAGE_SIZE       (NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE)
	#define MAX_BLOCKS          16384
	#define MAX_PAGES_IN_BLOCK  128 /* max number of pages of per block*/
	#define MINI_FREE_BLOCK     20// stanley
	#define MT_MAX_BLOCKS       6    /*4  */  /*the max number of MapTab block */
	#define MTL_MAX_BLOCKS      MT_MAX_BLOCKS
	/* max number of pages on using MTLog blocks */
	#define MTL_MAX_PAGES       MTL_MAX_BLOCKS * MAX_PAGES_IN_BLOCK 
	
	#define DL_MAX_BLOCKS       128     /*the max number of DataLog blocks*/	
	#define DL_MAX_BLOCKS_DIV4	32	
	#define DL_MAX_BLOCKS_DIV8	16
	
	//stanley test speed improvment
	//#define DL_MAX_PAGES      1024    /* max number of pages on using DataLog blocks */
	#define DL_MAX_PAGES        4096    /* max number of pages on using DataLog blocks */
	#define DL_MAX_PAGES_DIV4   1024    /* max number of pages on using DataLog blocks */
	#define DL_MAX_PAGES_DIV8   512	    /* max number of pages on using DataLog blocks */
	
	//increased by kevinwu
	#define GT_MAX_BLOCKS       	1
	//#define GARBAGE_MAX_CNT   	2 * MAX_PAGES_IN_BLOCK * GT_MAX_BLOCKS
	#define GARBAGE_MAX_CNT     	256
	#define GARBAGE_MAX_CNT_DIV2	128


#else

//same as 2K at this stage

	#define MAX_PAGE_SIZE       (NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE)
	#define MAX_BLOCKS          16384
	#define MAX_PAGES_IN_BLOCK  128 /* max number of pages of per block*/
	#define MINI_FREE_BLOCK     20// stanley
	#define MT_MAX_BLOCKS       6    /*4  */  /*the max number of MapTab block */
	#define MTL_MAX_BLOCKS      MT_MAX_BLOCKS
	/* max number of pages on using MTLog blocks */
	#define MTL_MAX_PAGES       MTL_MAX_BLOCKS * MAX_PAGES_IN_BLOCK 
	
	#define DL_MAX_BLOCKS       128     /*the max number of DataLog blocks*/	
	#define DL_MAX_BLOCKS_DIV4	32	
	#define DL_MAX_BLOCKS_DIV8	16
	
	//stanley test speed improvment
	//#define DL_MAX_PAGES      1024    /* max number of pages on using DataLog blocks */
	#define DL_MAX_PAGES        4096    /* max number of pages on using DataLog blocks */
	#define DL_MAX_PAGES_DIV4   1024    /* max number of pages on using DataLog blocks */
	#define DL_MAX_PAGES_DIV8   512	    /* max number of pages on using DataLog blocks */
	
	//increased by kevinwu
	#define GT_MAX_BLOCKS       	1
	//#define GARBAGE_MAX_CNT   	2 * MAX_PAGES_IN_BLOCK * GT_MAX_BLOCKS
	#define GARBAGE_MAX_CNT     	256
	#define GARBAGE_MAX_CNT_DIV2	128

#endif

#define ECC_ERR_LIMIT		2



#define FIND_OK             0x00
#define FIND_FAIL           0x01

#define OUTOFSECTOR         0x01

#define APPEND_OPERATION    0x00
#define INSERT_OPERATION    0x01
#define DEL_OPERATION       0x02
#define UPDATE_OPERATION    0x03

#define FALSE               0
#define TRUE                1

#pragma pack (1)

typedef struct 
{
    uint32_t    maxlp;      /* max number of logical pages*/
    uint16_t    startpb;    /* start physical block address for logical block */
} logical_params;

typedef enum 
{
    FREE_BLOCK          =   0xff,
    DATA_BLOCK          =   0xfe,
    DATA_LOG_BLOCK      =   0xfd,
    MAP_TAB_BLOCK       =   0xfc,
    MT_LOG_BLOCK        =   0xfb,
    GARBAGE_TAB_BLOCK   =   0xfa,
    BBT_BLOCK           =   0xf9,
    BadBlock            =   0xaa
} block_state;

/* store on spare area of each page of Block */
typedef struct
{
    uint8_t blocktype;
    union
    {
        uint32_t lpa;  /* Logic Page address.Valid for Data and DataLog Block.*/
        struct
        {
             uint8_t tabindx;  /* the index of the MapTab block */
             uint16_t lastfree;/* last allocated free block address */
        }mtspare;
        struct 
        {
             uint8_t tabindx;    /* index of the Mappping Table */
             uint16_t pageofs;  /* physical page offset in whole MapTab */
        }mtlofs;                /* the offset of MTLog block */
        struct
        {
            uint8_t tabindx;   /* index of the Garbage Table */
            uint8_t thiscount; /* the number of pbas in this GarbageTap page */ 
        }gtinfo;
#if 0
        struct
        {
            uint8_t bbtofs;    /* denote the page offset of the BBT */
            uint16_t bbtsts;   /* the switch times of BBT block, "switch" means
                                * BBT block switch from a pba to another pba*/
        }bbtinfo;
#endif
    }; 
    uint16_t resvered[NAND_MAX_OOBSIZE - 5];//modified by kevinwu
} block_info;

typedef union 
{
    block_info  asbi;                   /* block information */
    uint8_t     asbyte[NAND_MAX_OOBSIZE]; /* package of block information */
} block_info_union;

typedef struct 
{
    uint8_t     mtindx; /* index of mapping table */
    uint16_t    pba;    /* physical block address */
    uint8_t     used;   /* number of used pages in this block */
    uint8_t     valid;  /* number of valid pages in this block */
} mtl_blockinfo;

typedef struct 
{
    uint16_t    ppa;    /* physical page address in MTLog block */
    uint16_t    pageofs;/* page offset in Mapping Table*/
} mtl_transitem;

typedef struct 
{
    uint16_t    lba;    /* logical block address */
    uint16_t    pba;    /* physical block address */
    uint8_t     used;   /* number of used pages in this block */
    uint8_t     valid;  /* number of valid pages in this block */
} dl_blockinfo;

typedef struct 
{
    uint16_t    ppa;    /* physical page address in DataLog block */
    uint32_t    lpa;    /* logical page address */
} dl_transitem;

/******************************************************************************
 *                                                                            *
 * Global Variable                                                            *
 *                                                                            *
 ******************************************************************************/

logical_params logparam;

/*******************************
 * Block Information on RAM    *
 *******************************/
uint8_t nr_pgsinblk;
uint16_t nr_blks;

/* store the physical block addresses of all MapTab blocks on RAM*/
uint16_t mt_pbas[MT_MAX_BLOCKS];
/* the number of MapTab blocks */
uint8_t mt_blkcnt = 0;

/* info table for MTLog block on RAM */
mtl_blockinfo mtl_blocktab[MT_MAX_BLOCKS];
/*total number of using MTLog blocks */
uint8_t mtl_blkcnt = 0;
/* transform table between page offset in MapTab and PPA in MTLog on RAM */
mtl_transitem mtl_transtab[MTL_MAX_PAGES];
/* total number of pages on using MTLog blocks */
uint16_t mtl_pgcnt = 0;

/* the information table between Data and DataLog block */
dl_blockinfo dl_blocktab[DL_MAX_BLOCKS];
/* total number of using DataLog blocks */
uint16_t dl_blkcnt = 0;
/* the transform table between LPA in Data and PPA in DataLog on RAM*/
dl_transitem dl_transtab[DL_MAX_PAGES]; 
/* total number of pages on using DataLog blocks */
uint16_t dl_pgcnt = 0;

/* store the physical block addresses of all GarbageTab blocks on RAM*/
uint16_t gt_pbas[GT_MAX_BLOCKS];
/* the number of pages used in GarbageTab block */
uint8_t gt_pgcnt = 0;
/* store current garbage block addresses */
uint16_t gar_pbas[GARBAGE_MAX_CNT];
/* the number of Garbage blocks */
uint16_t gar_blkcnt = 0;

/* which block is free and each bit presents a block*/

//Stanley try to fix re-free block issue

#undef BIT_STATUS

#ifdef BIT_STATUS
uint8_t blockstatus[MAX_BLOCKS/8];
#else
uint8_t blockstatus[4096];
#endif

/* total number of Free blocks on Nand Flash */
uint16_t free_blkcnt = 0;
/* it is read from MapTab Block, find the next free block from the block */
uint16_t lastfree;

/*************add by kevinwu for msc *******/
uint32_t g_badblkcnt=0;
uint32_t g_storagesize=0;
EXPORT_SYMBOL(g_storagesize);

/****************************************/

//extern struct mtd_info *g_mtd;

/*
 * Read data and/or oob from flash
 */
 
 
#define MT_READCACHE 
//#undef MT_READCACHE 

#ifdef MT_READCACHE
uint16_t mt_cacheblock = 0xffff;
uint8_t mt_cachepage = 0xff;
static uint8_t mt_readcache[NAND_MAX_PAGESIZE];
#endif




static int nftl_set_block_nofree(uint16_t block);
static int nftl_set_block_bad(uint16_t block);





int mtd_read(struct mtd_info *mtd, uint16_t block, uint8_t page,
		  size_t *retlen, uint8_t *buf, uint8_t *oob, uint8_t nr_page)
{
	struct mtd_oob_ops ops;
	int res;	
	//struct mtd_ecc_stats stats;
	
	//stats = mtd->ecc_stats;
	
	#ifdef MAGUS_LINUX
	loff_t offs = block * mtd->erasesize + page * mtd->writesize;
	#else
	//coz bin file didn't have spare area
	loff_t offs = block * (mtd->erasesize + nr_pgsinblk*mtd->oobsize) + page * (mtd->writesize + mtd->oobsize);
	#endif
	
	mtd->ecc_stats.corrected = 0;
	
	if (!buf)
	{		
		ops.mode = MTD_OOB_AUTO;
		ops.ooboffs = 0;//offs & (mtd->writesize - 1);
		ops.ooblen = mtd->oobsize;
		ops.oobbuf = oob;
		ops.datbuf = NULL;
		#ifdef MAGUS_LINUX
		res = mtd->read_oob(mtd, offs & ~(mtd->writesize - 1), &ops);
		#else
		res = mtd->read_oob(mtd, offs, &ops);
		#endif
		
		*retlen = ops.oobretlen;
		if(res)	
			nftl_error("mtd->read_oob() oob error : block=%d,page=%d\n",block,page);

		//#ifndef MAGUS_LINUX
		//if(block==100)
		//	return 1;
		//#endif


		return res;
	}
	else if (!oob)
	{		
		res = mtd->read(mtd, offs, mtd->writesize * nr_page, retlen, buf);		
		if(res)	
			nftl_error("mtd->read() data error : block=%d, page=%d\n",block,page);		

		//#ifndef MAGUS_LINUX
		//if(block==100)
		//	return 1;
		//#endif
		
		#if 0
		if( mtd->ecc_stats.corrected /*- stats.corrected*/)
		{	
			//nftl_error("NFTL ecc correction : block=%04d page=%03d max_cnt=%d\n",block,page,mtd->ecc_stats.corrected);
			// mtd->ecc_stats.corrected = 0;	//clean up fix count, coz driver update mtd_part only			 
		}	 
		#endif
		
		#ifndef MAGUS_LINUX		
		//if((block==40)||(block==50)||(block==60)||(block==70)||(block==80)||(block==90))
		//	mtd->ecc_stats.corrected=3;
		//if((test_flag) && (!(block%3)))
		//	mtd->ecc_stats.corrected=3;
		#endif
	    return res;
	}	
	//nftl_debug("mtd read oob&data block:%d, page:%d, nr_page:%d \n ",block,page,nr_page);	
  	ops.mode = MTD_OOB_AUTO;
	ops.ooboffs = 0;
	ops.ooblen = mtd->oobsize;
	ops.oobbuf = oob;
  	ops.datbuf = buf;
  	ops.len = mtd->writesize * nr_page;	

	#ifdef MAGUS_LINUX
	res = mtd->read_oob(mtd, offs & ~(mtd->writesize - 1), &ops);
	#else
	res = mtd->read_oob(mtd, offs, &ops);		//no need to align, for spare area handling
	#endif


	*retlen = ops.retlen + ops.oobretlen+2;//modified by kevinwu

	//#ifndef MAGUS_LINUX
	//if(block==100)
	//	return 1;
	//#endif
	#if 0
	if( mtd->ecc_stats.corrected /*- stats.corrected*/)
	{	
		 //nftl_error("NFTL ecc correction : block=%04d page=%03d max_cnt=%d\n",block,page,mtd->ecc_stats.corrected);
		 //mtd->ecc_stats.corrected = 0;	//clean up fix count, coz driver update mtd_part only			 
	}	 
	#endif	
	#ifndef MAGUS_LINUX		
	//if((block==40)||(block==50)||(block==60)||(block==70)||(block==80)||(block==90))
	//	mtd->ecc_stats.corrected=3;
	//if((test_flag) && (!(block%3)))
	//	mtd->ecc_stats.corrected=3;	
	#endif
	
	
	if(res)
		nftl_error("mtd->read_oob() data & oob error:block=%d,page=%d\n",block,page);
	return res;
}

/*
 * Write data and oob to flash
 */
int mtd_write(struct mtd_info *mtd, uint16_t block, uint8_t page,
		  size_t *retlen, uint8_t *buf, uint8_t *oob, uint8_t nr_page)
{
	struct mtd_oob_ops ops,ops2;
	int res;
	//uint8_t tempbuf[NAND_MAX_PAGESIZE];
	//uint8_t tempoob[NAND_MAX_OOBSIZE];
	//uint8_t *tempbuf=kmalloc(NAND_MAX_PAGESIZE,GFP_KERNEL);
		
	int retry=0,i;
	
	#ifdef MAGUS_LINUX
	loff_t offs = block * mtd->erasesize + page * mtd->writesize;	
	#else
	loff_t offs = block * (mtd->erasesize + nr_pgsinblk*mtd->oobsize) + page * (mtd->writesize + mtd->oobsize);	
	gblock=block; gpage=page;
	#endif

		
    /*if (!buf || !oob)
    {
        nftl_error("data and spare should be programed together\n");
	}*/
		
	//Sometimes it will return non 0xff even it is blank block, 
	//because blank blank didn't with ECC correction	
	#if 0		//verify clear page
	//stanley add verify to improve read error rate
   	ops2.mode		= MTD_OOB_AUTO;
	ops2.ooboffs	= 0;
	ops2.ooblen		= mtd->oobsize;
	ops2.oobbuf		= tempoob;
    ops2.datbuf		= tempbuf;
    ops2.len		= mtd->writesize * nr_page;		
	#ifdef MAGUS_LINUX
	res = mtd->read_oob(mtd, offs & ~(mtd->writesize - 1), &ops2);
	#else
	res = mtd->read_oob(mtd, offs, &ops2);	
	#endif

	if(res)
		nftl_error("mtd write() ----> mtd->read_oob() erase verify error1: block=%d,page=%d\n",block,page);
		
	for (i = 0; i < mtd->writesize; i++)
	{
		if (ops2.datbuf[i]!= 0xff)
		{
			nftl_error("mtd write() --> mtd->read_oob() erase verify data error2: block=%d,page=%d, byte=%d\n",block,page,i);
			res=-1;
		}	
	}
	
	for (i = 0; i < /*mtd->oobsize*/5; i++)
	{
		if (ops2.oobbuf[i]!= 0xff)
		{
			nftl_error("mtd write() -> mtd->read_oob() erase verify oob error3: block=%d,page=%d, byte=%d \n",block,page,i);				
			nftl_error("mtd->oobsize=%d\n",mtd->oobsize);
			res=-1;
		}	
	}
		
	#endif		
					
	
	
	
	
	
	
	
	
	//if(block==0)
	//	nftl_error("mtd write() error : write address 0 \n"); 
	
	
	
	
	ops.mode = MTD_OOB_AUTO;
	ops.ooboffs = 0;
	ops.ooblen = mtd->oobsize;
	ops.oobbuf = oob;
	ops.datbuf = buf;
	ops.len = mtd->writesize * nr_page;
	#ifdef MAGUS_LINUX
	res = mtd->write_oob(mtd, offs & ~(mtd->writesize - 1), &ops);
	#else
	res = mtd->write_oob(mtd, offs , &ops);		//no need to align, for spare area handling
	#endif

	*retlen = ops.retlen + ops.oobretlen;
	
	if(res)			
	{
		//if(tempbuf) kfree(tempbuf);		
		nftl_error("mtd write() error : res = %d, *retlen = %d, block = %d, page = %d \n", res, *retlen,block,page);	
		return res;
	}
	

	#if 0
err_retry:		
	//stanley add verify to improve read error rate
   	ops2.mode		= MTD_OOB_AUTO;
	ops2.ooboffs	= 0;
	ops2.ooblen		= mtd->oobsize;
	ops2.oobbuf		= tempoob;
    ops2.datbuf		= tempbuf;
    ops2.len		= mtd->writesize * nr_page;		
	#ifdef MAGUS_LINUX
	res = mtd->read_oob(mtd, offs & ~(mtd->writesize - 1), &ops2);
	#else
	res = mtd->read_oob(mtd, offs, &ops2);	
	#endif

	if(res)
		nftl_error("mtd write() ----> mtd->read_oob() data & oob error:block=%d,page=%d\n",block,page);
	else
	{	
		if(oob)
		{
			if(memcmp(ops.oobbuf, ops2.oobbuf, /*ops.oobretlen*/5))
			{				
				if(retry<50)
				{	
					retry++;
					nftl_error("mtd write() -> mtd->read_oob() oob verify error block=%d,page=%d retry=%d\n",block,page,retry);
					goto err_retry;				
				}
				res=-1;
				nftl_error("mtd write() -> mtd->read_oob() oob verify error :block=%d,page=%d\n",block,page);
			}
		}

		if(buf)
		{			
			if(memcmp(ops.datbuf, ops2.datbuf, ops.retlen))
			{				
				if(retry<50)
				{	
					retry++;					
					nftl_error("mtd write() -> mtd->read_oob() data verify error block=%d,page=%d retry=%d\n",block,page,retry);
					goto err_retry;				
				}
				res=-1;				
				nftl_error("mtdwrite() -> mtd->read_oob() data verify error block=%d,page=%d\n",block,page);
			}	
		}
	}
	#endif


	//if(tempbuf)	kfree(tempbuf);		
	return res;
	//end
	
	
}



int mtd_verify_clear(struct mtd_info *mtd, uint16_t block)
{
	struct mtd_oob_ops ops,ops2;
	int res;	
	uint8_t tempoob[NAND_MAX_OOBSIZE];
	uint8_t *tempbuf=kmalloc(NAND_MAX_PAGESIZE,GFP_KERNEL);		
	int i,j,page;	
	
	//verify clear page
	//stanley add verify to improve read error rate
   	ops2.mode		= MTD_OOB_AUTO;
	ops2.ooboffs	= 0;
	ops2.ooblen		= mtd->oobsize;
	ops2.oobbuf		= tempoob;
    ops2.datbuf		= tempbuf;
    ops2.len		= mtd->writesize;		
    
    page=0;    
	for(page=0;page<nr_pgsinblk;page++)
    {	     	
		#ifdef MAGUS_LINUX
		loff_t offs = block * mtd->erasesize + page * mtd->writesize;	
		res = mtd->read_oob(mtd, offs & ~(mtd->writesize - 1), &ops2);
		#else		
		loff_t offs = block * (mtd->erasesize + nr_pgsinblk*mtd->oobsize) + page * (mtd->writesize + mtd->oobsize);		
		res = mtd->read_oob(mtd, offs, &ops2);	
		#endif
		
		if(res)
			nftl_error("mtd verify clear() ----> mtd->read_oob() erase verify error1: block=%d,page=%d\n",block,page);
			
		for (i = 0; i < mtd->writesize; i++)
		{
			if (ops2.datbuf[i]!= 0xff)
			{
				nftl_error("mtd_verify_clear() --> mtd->read_oob() erase verify data error2: block=%d,page=%d, byte=%d\n",block,page,i);
				res=-1;
			}	
		}
		
		for (i = 0; i < /*mtd->oobsize*/5; i++)
		{
			if (ops2.oobbuf[i]!= 0xff)
			{
				nftl_error("mtd verify clear() -> mtd->read_oob() erase verify oob error3: block=%d,page=%d, byte=%d \n",block,page,i);				
				nftl_error("mtd->oobsize=%d\n",mtd->oobsize);
				res=-1;				
			}	
		}
	}		
				
	
	if(tempbuf)
		kfree(tempbuf);		
	return res;
	//end
	
}

static int g_debugcnt=0;

int mtd_erase(struct mtd_info *mtd, uint16_t block)
{
    struct erase_info instr;
    memset(&instr, 0, sizeof(struct erase_info));
	//nftl_debug("mtd erase");
    instr.mtd = mtd;    
    #ifdef MAGUS_LINUX
    instr.addr = block * mtd->erasesize;
	instr.len = mtd->erasesize;
	if(instr.addr > nr_blks* mtd->erasesize)
	{
		nftl_error("mtd erase address error \n");
		return -1;
	}		
    #else
    instr.addr = block * (mtd->erasesize + nr_pgsinblk*mtd->oobsize);
	instr.len = mtd->erasesize + nr_pgsinblk*mtd->oobsize;
	if(instr.addr >nr_blks* (mtd->erasesize + nr_pgsinblk*mtd->oobsize) )
	{
		nftl_error("mtd erase address error \n");
		return -1;
	}	
    #endif
    
        			
    mtd->erase(mtd, &instr);
    
    #if 0
    if(instr.addr == 0*mtd->erasesize)
    {	
    	g_debugcnt++;
    	nftl_error("mtd going to erase address 0, cnt = %d \n",g_debugcnt);
    	if(g_debugcnt>1)    
    		instr.state = MTD_ERASE_FAILED;    	
    }	 
    #endif   	
    	
    
    if (instr.state == MTD_ERASE_FAILED) 
    {
        nftl_error("Error while erase block %d\n", block);
        return -1;
    }
    
    //mtd_verify_clear(mtd,block);    
    return 0;
}

static uint16_t nftl_get_free_block(void);

void nand_info(struct NFTLrecord *s)
{
    block_info blkinfo;
    uint8_t *pBS;
    
    size_t retlen;
	int ret;
	int i;
	 int j;
	struct mtd_info *mtd = s->mbd.mtd;
	 uint16_t *pdata;
    /* get the garbage map block */
    pBS = (uint8_t*)&blkinfo;
    nftl_debug("\n------------------ nand_info -------------------\n ");
    
    
    for(i=0; i< s->nb_blocks; i++) 
    {
        ret = mtd_read(mtd, i, 0, &retlen, NULL, pBS, 1);		//read oob
		
		if (*pBS != 0xff && *(pBS + 1) != 0xff)					//how about it is a bad block ??
		{
	
			if(*pBS==0xfc)
			{
				uint8_t *databuff;
				nftl_debug("Block %d Read result: %d, %d, %x %x %x %x\n",
			    i, ret, retlen, *pBS, *(pBS + 1), *(pBS + 2),*(pBS + 3));
				
                databuff=kmalloc(NAND_MAX_PAGESIZE,GFP_KERNEL);
                nftl_debug("kmallow size :%x \n",NAND_MAX_PAGESIZE);		// NAND_MAX_PAGESIZE=4096
                
                
				if(!databuff)
				{
					nftl_error("error kmalloc \n");
					return;
				}
				
				 ret = mtd_read(mtd, i, 0, &retlen, databuff, NULL, 1);		//read data block with oob[0]=0xfc
				
				 pdata=databuff;
				 nftl_debug("\nblock=%d is mapping logic->physic\n",i);
				 for(j=0;j<mtd->writesize;j++)								//???? what are u doing
			 	{
			 	    //if(pdata[j]!=0xffff)
			 		nftl_debug("%d->%d ",j,pdata[j]);
					//pdata++;
			 	}
			 
				 if(databuff)
				 	kfree(databuff);
				 
			}
		}
    }
    nftl_debug("\n------------------------------------------------------\n ");

}

void debug_nftl_global_info()
{
	int i;
	nftl_debug("\n------------------debug_nftl_global_info -------------------\n ");
	nftl_debug("max pages=%d,startpb=%d\n",logparam.maxlp,logparam.startpb);
	nftl_debug("pages of one block =%d\n",nr_pgsinblk);
	nftl_debug("number of blocks=%d\n", nr_blks);

	nftl_debug("number of map table block=%d\n",mt_blkcnt);
	for(i=0;i<mt_blkcnt;i++)
	{
		nftl_debug("mt_pbas[%d]=%d ",i,mt_pbas[i]);
	}
	nftl_debug("\ntotal number of mtlog blocks=%d\n", mtl_blkcnt);
	
	nftl_debug("total number of pages on using MTLog blocks:mtl_pgcnt=%d\n",mtl_pgcnt);
	for(i=0;i<mtl_pgcnt;i++)
	{
	  nftl_debug("%d:ppa=%d,pageofs=%d\n",i,mtl_transtab[i].ppa,mtl_transtab[i].pageofs);
		
	}
	nftl_debug("\ntotal number of Datalog blocks:dl_blkcnt=%d\n",dl_blkcnt);
	for(i=0;i<dl_blkcnt;i++)
	{
		nftl_debug("%d:lba=%d,pba=%d,used=%d,valid=%d\n",i,dl_blocktab[i].lba,dl_blocktab[i].pba,dl_blocktab[i].used,dl_blocktab[i].valid);
		
		
	}
	
	/* the transform table between LPA in Data and PPA in DataLog on RAM*/
	
	/* total number of pages on using DataLog blocks */
	//uint16_t dl_pgcnt = 0;
	
	nftl_debug("\ntotal number pages on using Datalog blocks dl_pgcnt=%d\n",dl_pgcnt);
	for(i=0;i<dl_pgcnt;i++)
	{
	nftl_debug("%d:ppa=%d,lpa=%d\n",i,dl_transtab[i].ppa,dl_transtab[i].lpa);

	
	
	}
	
	/* store the physical block addresses of all GarbageTab blocks on RAM*/
	nftl_debug("garbage info:\n");
	for(i=0;i<GT_MAX_BLOCKS;i++)
	{
		nftl_debug("gabage physic block adress:gt_pbas[%d]=%d\n",i,gt_pbas[i]);
	}
	/* the number of pages used in GarbageTab block */
	
	nftl_debug("the number of pages used in GarbageTab block gt_pgcnt=%d\n",gt_pgcnt);


	nftl_debug("\nthe number of Garbage blocks:gar_blkcnt=%d\n",gar_blkcnt);
	
	/* which block is free and each bit presents a block*/
	//uint8_t blockstatus[MAX_BLOCKS/8];
	/* total number of Free blocks on Nand Flash */
	nftl_debug("free block number: free_blkcnt=%d\n",free_blkcnt);
	
	/* it is read from MapTab Block, find the next free block from the block */
	
	nftl_debug("lastfree=%d\n",lastfree);
	nftl_debug("\n-----------------------------------------------------------------\n ");

}



static int nftl_erase_error(struct mtd_info *mtd, uint16_t block)
{
	nftl_error("nftl_erase_error .... blk %d marked as bad block\n",block);
	nftl_set_block_bad(block);
    return mtd->block_markbad(mtd, block * mtd->erasesize);
} 

static int nftl_block_replace(struct mtd_info *mtd, uint16_t src_blk, uint16_t dst_blk, uint8_t pagecnt)
{
    //uint8_t databuff[NAND_MAX_PAGESIZE * pagecnt];
    uint8_t sparebuff[NAND_MAX_OOBSIZE];//modified by kevinwu
    uint8_t i;
    int ret=0;
    size_t retlen;
	uint8_t *databuff=kmalloc(NAND_MAX_PAGESIZE,GFP_KERNEL);
	if(!databuff)
 	{
 	 	nftl_error("malloc memory error\n");
 	 	//return 1 will cause error handler mark new blk as bad blk
 	 	//and finally, all blk become bad blk
		return 0;
 	}
 	
	nftl_msg("nftl block replace() ....\n");

    for (i = 0; i < pagecnt; i++)//modified by kevinwu
    {
	    ret = mtd_read(mtd, src_blk, i, &retlen, databuff, sparebuff, 1);//modified by kevinwu
	    
	    //we need to assume read src ok, although it will induce data loss!!
	    //but there's no solution on fixing read source blk error
	    //actually, we need to return I/O error here!!
	    //if(!ret) //test by kevinwu,because we have no capability to handle reading error, if readding error ocurs,it will consumes all free block
	    {
	   		 /*
	        if(databuff[0] == 0xff && sparebuff[0] == 0xff)
	        {
	            if(databuff)
			   kfree(databuff);
	            return ret;
	        }*/
	        ret = mtd_write(mtd, dst_blk, i, &retlen, databuff, sparebuff, 1);
	    }
	    if(ret)
	    {
	     	nftl_error("block replace error : %d Block %d Page \n",src_blk,i);
			if(databuff)
				kfree(databuff);
			return ret;
	    }
    }
	if(databuff)
		kfree(databuff);
    return ret;
}

static uint16_t nftl_write_error(struct mtd_info *mtd, uint16_t block, uint8_t page, 
                            uint8_t *data, uint8_t *spare,uint8_t nr_page) 
{
    uint16_t backup_block;
    int  write_ok;
	size_t retlen;
	nftl_msg("nftl write error  handling start ... \n");
    
Try_Handle_Write_Error:
    
    write_ok = 0;
    backup_block = nftl_get_free_block();
	if(backup_block == 0xffff)
		return 0xffff;
    
    //copy error blk to new blk (from 0 to error page)
    if (nftl_block_replace(mtd, block, backup_block, page) != 0)
    {
    	//we assume blk repalce err only caused by new blk is bad block
    	nftl_error("nftl_write error handling fail ... blk %d was marked as bad block \n",backup_block);
        mtd->block_markbad(mtd, backup_block * mtd->erasesize);
        nftl_set_block_bad(backup_block);
        goto Try_Handle_Write_Error;
    }
    
    //write data to new blk (error page to nr_page)
    write_ok = mtd_write(mtd, backup_block, page, &retlen, data, spare, nr_page);    

    
    if(write_ok == 0 && retlen == mtd->writesize + mtd->oobsize)
    {	
    	//done !! finally, we mark the error blk as bad block
    	nftl_msg("nftl_write error handling succeed ... blk %d was marked as bad block \n",block);
        mtd->block_markbad(mtd, block * mtd->erasesize);
        nftl_set_block_bad(block);
        return backup_block;
    }
    else
    {
    	//we assume blk repalce err only caused by new blk is bad block
    	nftl_error("nftl_write error handling fail ... blk %d was marked as bad block \n",backup_block);
        mtd->block_markbad(mtd, backup_block * mtd->erasesize);
        nftl_set_block_bad(backup_block);
        goto Try_Handle_Write_Error;
    }
}

/*********************************************************/

 static int nftl_copyback(struct mtd_info *mtd, uint16_t src_blk,uint8_t src_pg,
               uint16_t dst_blk,uint8_t dst_pg, block_state dst_blk_type)
{
	//uint8_t temp_data[NAND_MAX_PAGESIZE];
	block_info blkinfo;
	int ret;
    size_t retlen;

	
	uint8_t *temp_data=kmalloc(NAND_MAX_PAGESIZE,GFP_KERNEL);	  
   	if(!temp_data)
   	{
   		nftl_error("alloc memeory error\n");
		return EIO;	
   	}
	   	
	
	/*should memset to oxff, otherwise it won't return 0xffff if pda not found*/
	memset(temp_data,0xff,sizeof(temp_data));	
	   	
	ret = mtd_read(mtd, src_blk, src_pg, &retlen, temp_data, &blkinfo, 1);

	if(ret)
	{
	 	nftl_error("mtd read error in nftl copyback(), src block=%d, src page=%d\n",src_blk,src_pg);
	 	
	 	if(temp_data)	
			kfree(temp_data);
			
		return EIO;	
	}
	else
    //we need to assume read src ok, although it will induce data loss!!
    //but there's no solution on fixing read source blk error
	//actually, we need to return I/O error here!!
	//if(ret == 0) //test by kevinwu, because we have no capability to handle reading error, if readding error ocurs,it will consumes all free block
	{
		if(dst_blk_type != FREE_BLOCK )
		{
		    /* if dest_block_type == FREE_BLOCK,don't change the block type */
			blkinfo.blocktype = dst_blk_type;
		}	
		
	     ret = mtd_write(mtd, dst_blk, dst_pg, &retlen, temp_data, &blkinfo, 1);

	
		if(ret)
		{
			nftl_error("mtd_write error in nftl copyback() dst Block= %d, dst Page=%d \n",dst_blk,dst_pg);
		}	    
	}
	
	if(temp_data)	
		kfree(temp_data);
		   	
	  	
	
	return ret;	
}



static uint16_t nftl_copyback_error(struct mtd_info *mtd, uint16_t src_blk,
     uint8_t src_pg, uint16_t dst_blk,uint8_t dst_pg, block_state dst_blk_type)
{
    uint16_t backup_block;
    int  copy_ok;
	nftl_msg("nftl copyback error() handling start ...\n");

Try_Handle_CopyBack_Error:
    copy_ok = 0;
    backup_block = nftl_get_free_block();   
    

    if(backup_block==0xFFFF)
   	{
		nftl_error("have no free block\n");
		return backup_block;
     }//add by kevinwu
    
    if (nftl_block_replace(mtd, dst_blk, backup_block, dst_pg) != 0)
    {
		//we assume blk repalce err only caused by new blk is bad block
		nftl_error("nftl copyback error handling fail ... blk %d was marked as bad block \n",backup_block);
        mtd->block_markbad(mtd, backup_block * mtd->erasesize);
        nftl_set_block_bad(backup_block);
        goto Try_Handle_CopyBack_Error;
    }
    copy_ok = nftl_copyback(mtd, src_blk, src_pg, backup_block, dst_pg,dst_blk_type);
    
    if(copy_ok == 0)
    {
    	//done !! finally, we mark the error blk as bad block
    	nftl_msg("nftl copyback error handling succeed, blk %d was marked as bad block \n",dst_blk);
        mtd->block_markbad(mtd, dst_blk * mtd->erasesize);
        nftl_set_block_bad(dst_blk);
        return backup_block;
    }
    else 
    {
    	//we assume blk repalce err only caused by new blk is bad block
    	nftl_error("nftl copyback error handling fail ... blk %d was marked as bad block \n",backup_block);
        mtd->block_markbad(mtd, backup_block * mtd->erasesize);
        nftl_set_block_bad(backup_block);
        goto Try_Handle_CopyBack_Error;
    }
}

static uint8_t nftl_is_gar_blk(uint16_t block) 
{
    uint16_t i;
	
    for (i = 0;i < gar_blkcnt;i++) 
    {
        if (block == gar_pbas[i]) 
        {
            return TRUE;
        }
    }
    return FALSE;
}

static uint8_t nftl_get_gar_blks(struct mtd_info *mtd, uint16_t *gtblock, uint8_t *gtpgcnt, 
                            uint16_t *garblocks, uint16_t *garcnt)
{
    int ret = 0;
	size_t retlen;
    uint16_t i;
    //uint8_t databuff[NAND_MAX_PAGESIZE];
    block_info blkinfo;
    uint8_t *pBS;
    uint16_t *ptmpgar;
    uint8_t j, k;


	uint8_t *databuff=kmalloc(NAND_MAX_PAGESIZE,GFP_KERNEL);
	nftl_debug("nftl_get_gar_blks\n");

	if(!databuff)
	{
		nftl_error("malloc memory error\n");
		return 1;
	}
	    
    *garcnt =0;
    *gtpgcnt = 0;
    for (j = 0; j < GT_MAX_BLOCKS; j ++) 
    {
        if (gtblock[j] == 0xffff)
        {
            break;
        }
        
        pBS = ((block_info_union *)&blkinfo)->asbyte;
    
        for (i = 0; i < nr_pgsinblk; i++) 
        {
            if ((ret = mtd_read(mtd, gtblock[j], i, &retlen, databuff, pBS,1)) == 0)
            {
                if (blkinfo.gtinfo.thiscount == 0xff)
                {
                    break;
                }
		 		nftl_debug("blkinfo.gtinfo.thiscount=%d\n",blkinfo.gtinfo.thiscount);
		  
                for(k = 0; k < blkinfo.gtinfo.thiscount; k++)
                {
                    ptmpgar = (uint16_t *)&databuff[k*sizeof(uint16_t)];
                    if (*ptmpgar == 0xffff)
                    {
                        break;
                    }
                    else 
                    {
                        *(garblocks + *garcnt) = *ptmpgar;
                        (*garcnt) ++;
                    }
                }
                (*gtpgcnt) ++;
            }
            else
            {
              if(databuff)
			  	kfree(databuff);
                return ret;
            }
        }
    }
	if(databuff)
	kfree(databuff);
    return ret;
}



static int nftl_is_block_free(uint16_t block) 
{
    uint16_t byte_ofs;
    uint8_t  bit_ofs;
	
    /* check the block is in the span of block number */
    if(block > nr_blks) 
    {
        nftl_error("the number of block is out of the span of block \n");
        return -1;
    }


	#ifdef BIT_STATUS
	
    byte_ofs = block / 8 ;
    bit_ofs  = block % 8 ;
    
    if(blockstatus[byte_ofs] & (0x01 << bit_ofs))
   		return 1;
    
    #else
    
    if(blockstatus[block])
   		return 1;
    
    #endif
    
    return 0;
}


static int nftl_set_block_free(uint16_t block) 
{
    uint16_t byte_ofs;
    uint8_t  bit_ofs;

    /* check the block is in the span of block number */
    if(block > nr_blks) 
    {
        nftl_error("the number of block is out of the span of block \n");
        return -1;
    }
	
	if(nftl_is_block_free(block))
	{
		nftl_error("This block has been free already, blk=%d\n",block);	
		return 0;
	}		
	
	#ifdef BIT_STATUS
	
    byte_ofs = block / 8 ;
    bit_ofs  = block % 8 ;
    
    blockstatus[byte_ofs] |= (0x01 << bit_ofs);
    
    #else
    
    blockstatus[block] = 0xff;
        
    #endif
    free_blkcnt++;
    
    return 0;
}

static int nftl_set_block_nofree(uint16_t block) 
{
    uint16_t byteofs;
    uint8_t  bitofs;


    /* check the block is in the span of block number */
    if(block > nr_blks) 
    {
        nftl_error("the number of block is out of the span of block \n");
        return -1;
    }

	#ifdef BIT_STATUS
    byteofs = block / 8 ;
    bitofs  = block % 8 ;
    
    blockstatus[byteofs] &= ~(0x01 << bitofs);
    
    #else
    
	blockstatus[block] = 0;
    
    #endif
    
    free_blkcnt--;
    lastfree = block;
    return 0;
}

static int nftl_set_block_bad(uint16_t block) 
{
    uint16_t byteofs;
    uint8_t  bitofs;


    /* check the block is in the span of block number */
    if(block > nr_blks) 
    {
        nftl_error("the number of block is out of the span of block \n");
        return -1;
    }
	
	#ifdef BIT_STATUS
	
    byteofs = block / 8 ;
    bitofs  = block % 8 ;
    
    blockstatus[byteofs] &= ~(0x01 << bitofs);
    
    #else
    
    blockstatus[block] = 0;
    
    #endif
    
    free_blkcnt--;    
    return 0;
}


#ifdef BIT_STATUS

static uint16_t nftl_get_free_block(void)
{
    uint16_t byteofs;
    uint8_t  bitofs;
    uint16_t i;
    uint8_t temppointer;
    uint8_t j;
    uint16_t nr_blks_in_byte; 

    uint16_t nextfree;
	    
    nftl_debug("nftl get free block: lastfree is %d, free_blkcnt = %d\n",lastfree, free_blkcnt);
    
    
    if( free_blkcnt < MINI_FREE_BLOCK - 1)     
        nftl_error("Number of the free blocks is too samll, free_blkcnt = %d\n",free_blkcnt);

    if(free_blkcnt == 0)
        return 0xFFFF;
    
    nextfree = lastfree;
    byteofs = lastfree / 8;
    bitofs  = lastfree % 8;
	
	nr_blks_in_byte  = nr_blks/8 ;
	nr_blks_in_byte += (nr_blks%8>0)?1:0;


	//find next free block with current byte 
    temppointer = blockstatus[byteofs];
    for(i=bitofs;i<8;i++)
    {
        if( (temppointer & (0x01 << i )) != 0x00) 
        {
            nftl_set_block_nofree(nextfree);
            return nextfree;
        }
        else
            nextfree++;
    }
    
    //for( i = byteofs +1 ; i <  2 * (nr_blks/8) ; i++ ) 
	for( i = byteofs +1 ; i <  2 * (nr_blks_in_byte) ; i++ ) 
    {
        //if( i >= (nr_blks/8) )
        //    temppointer = blockstatus[i - (nr_blks/8) ];
		if( i >= nr_blks_in_byte )
            temppointer = blockstatus[i - nr_blks_in_byte ];
        else  
            temppointer = blockstatus[i];

        if(temppointer == 0x00)
        {
			//this is fucking wrong, it is not valid at last byte
            nextfree += 8;

			//should added below as quick fix
			if(nextfree>nr_blks)
				nextfree=0;
        }
        else 
        {
            for(j=0;j<8;j++)
            {
                if( (temppointer & (0x01 << j )) != 0x00)
                { 
                    if( nextfree >= nr_blks)
                        nextfree -= nr_blks;
                    nftl_set_block_nofree(nextfree);
                    return nextfree;
                }
                else 
                    nextfree++;
             }
        }
    }
    
    nftl_error("There is no free block , byteofs = %d bitofs = %d"
                                "i = %d\n",byteofs,bitofs,i);    
    
    return 0xFFFF;
}

#else

static uint16_t nftl_get_free_block(void)
{
    
    uint16_t i;
    uint16_t nextfree;
	    
    nftl_debug("nftl get free block: lastfree is %d, free_blkcnt = %d\n",lastfree, free_blkcnt);
        
    if( free_blkcnt < MINI_FREE_BLOCK - 1)     
        nftl_error("Number of the free blocks is too samll, free_blkcnt = %d\n",free_blkcnt);

    if(free_blkcnt == 0)
        return 0xFFFF;
        
    nextfree = lastfree;
            
    if(nextfree>nr_blks)
    	nextfree=0;            
            
    for( i = lastfree ; i <  2*nr_blks ; i++ ) 
    {
	    if(blockstatus[nextfree])
	    {	
	    	nftl_set_block_nofree(nextfree);
	    	return nextfree;
	    }
	    else	    
	    {	
    		nextfree++;
    		if(nextfree>nr_blks)
    			nextfree=0;
    	}	
    }    
    nftl_error("There is no free block \n");    
    
    return 0xFFFF;
}

#endif

int nftl_reclaim_garbages(struct mtd_info *mtd) 
{

    int ret = 0;
    uint16_t i;
	
    /* check whether there are garbage blocks */
    if(gar_blkcnt == 0)
    {
        return ret;
    } 

    /* erase those garbage block */
    for(i = 0; i < gar_blkcnt; i++) 
    {
        if(gar_pbas[i] != 0xffff)//add by zoe
        {
            ret = mtd_erase(mtd, gar_pbas[i]);
       
	        if(ret) 
	        {
				if(gar_pbas[i]>nr_blks)
					return -1;
	            nftl_error("erase the garbage block error = %d \n", gar_pbas[i]);
	            ret = nftl_erase_error(mtd, gar_pbas[i]);
	        }
	        else 
	        {
	            //nftl_debug("Erase Status 1: Block %d\n", gar_pbas[i]);
	            //nftl_debug("Erase garbage block: %d \n", gar_pbas[i]);
	            ret = nftl_set_block_free(gar_pbas[i]);
	            //nftl_debug("Free Block: %d, lastfree is %d\n", gar_pbas[i], lastfree);
	        }
		}
    }
 
    /* erase the old garbage map block */
    for( i = 0 ; i < GT_MAX_BLOCKS ; i ++) 
    {
        if (gt_pbas[i] != 0xffff)
        {
            ret = mtd_erase(mtd, gt_pbas[i]);
     
	        if (ret)
	        {
	            ret = nftl_erase_error(mtd, gt_pbas[i]);
	        }
	        else
	        {
	            //nftl_debug("Erase Status 2: Block %d\n", gt_pbas[i]);
	            ret = nftl_set_block_free(gt_pbas[i]);
	            //nftl_debug("Free Block: %d, lastfree is %d\n", gt_pbas[i], lastfree);
				//nftl_debug("Erase garbage table block: %d \n", gt_pbas[i]);
	        }
	        gt_pbas[i] = 0xffff;
		}
    }
    gar_blkcnt = 0;
    gt_pgcnt = 0;
    nftl_debug("After nftl_reclaim_garbages: free_blkcnt = %d\n", free_blkcnt);
    return ret;
}

static int nftl_addto_gt(struct mtd_info *mtd, uint16_t *garbages,uint8_t garcnt)
{	
    /* add the garbages into the GarbageTab block */
    int ret = 0;
   	size_t retlen;
    uint8_t pageofs,blkindx;
   // uint8_t databuff[NAND_MAX_PAGESIZE];
    block_info block_info;
    uint8_t *pBS;
    uint16_t i;
	
    uint8_t *databuff=kmalloc(NAND_MAX_PAGESIZE,GFP_KERNEL);
    if(!databuff)
	{
		nftl_error("malloc memory error\n");
		return 1;
	}
    
    //nftl_debug("add to garbage block, garbages[0] = %d , "
    //            "garbages[1] = %d , gt_pbas[0] = %d ,"
    //            "free_blkcnt = %d\n",garbages[0],garbages[1],
    //            gt_pbas[0],free_blkcnt);
    //nftl_debug("gar_blkcnt = %d \n", gar_blkcnt);
    
    blkindx = gt_pgcnt / nr_pgsinblk ;
    pageofs = gt_pgcnt % nr_pgsinblk;
    
    pBS = ((block_info_union *)&block_info)->asbyte;
    
    memset(databuff, 0xff, mtd->writesize);/* initialize the data */
    memset(pBS, 0xff, mtd->oobsize);/* initialize the spare */

    if(gt_pbas[blkindx] == 0xffff) 
    {
        gt_pbas[blkindx] = nftl_get_free_block();
    }

    for(i=0; i < garcnt; i++) 
    {
        *(uint16_t *)&databuff[i * sizeof(uint16_t)] = *(garbages + i);
        gar_pbas[gar_blkcnt] = *(garbages + i);
        gar_blkcnt ++;
    }
    block_info.blocktype = GARBAGE_TAB_BLOCK;
    block_info.gtinfo.tabindx = blkindx;
    block_info.gtinfo.thiscount = garcnt;
        
    ret = mtd_write(mtd, gt_pbas[blkindx], pageofs, &retlen, databuff, pBS, 1);
    if(ret || retlen != mtd->writesize + mtd->oobsize) 
    {    	
    	nftl_error("nftl addto gt error ... \n");
        gt_pbas[blkindx] = nftl_write_error(mtd,gt_pbas[blkindx],pageofs,databuff,pBS,1);
        ret=0;	//we have solved the problem !!
    }
    gt_pgcnt ++;
    
    if( gt_pgcnt >= nr_pgsinblk * GT_MAX_BLOCKS/2 ) //modified by kevinwu
    //if( gt_pgcnt >= nr_pgsinblk * GT_MAX_BLOCKS/4 ) //modified by kevinwu
    {
    	nftl_debug("gt_pgcnt >= nr_pgsinblk * GT_MAX_BLOCKS/2\n");    	
        ret = nftl_reclaim_garbages(mtd);
    }
    if(gar_blkcnt >= GARBAGE_MAX_CNT_DIV2) /* Garbage block is full. *///modified by kevinwu 
    //if(gar_blkcnt >= GARBAGE_MAX_CNT/4) /* Garbage block is full. *///modified by kevinwu 
    {
    	nftl_debug("gar_blkcnt >= GARBAGE_MAX_CNT/2\n");    	
        ret = nftl_reclaim_garbages(mtd);
    }
    //if(free_blkcnt <= MINI_FREE_BLOCK+10 && gar_blkcnt > 0)//modified by kevinwu
    if(free_blkcnt <= MINI_FREE_BLOCK && gar_blkcnt > 0)//stanley
    //if(free_blkcnt <= MINI_FREE_BLOCK+10 && gar_blkcnt > 0)//stanley
    {
        /* there is not enough free block  */
		nftl_debug("free_blkcnt <= MINI_FREE_BLOCK\n");		
        ret = nftl_reclaim_garbages(mtd);
    }
		
	for(i=0;i<gar_blkcnt;i++)
	{
		//nftl_debug("gar_pbas[%d]=%d \n",i,gar_pbas[i]);
		if(gar_pbas[i]>nr_blks)
		{	
			nftl_error("nftl addto gt fatal error \n");
			return 1;
		}	
	}
	
	if(databuff)
		kfree(databuff);

    return ret;
}

static uint8_t nftl_find_mtpg_in_mtl(uint16_t mtpgofs,uint16_t *retblk, uint8_t *retpg)
{
    uint8_t mtindex;
    uint16_t pageofs = 0;/* the page offset in mtl_transtab on RAM */
    uint16_t i,j;
    uint8_t ret = FIND_OK;
	    
    mtindex = mtpgofs / nr_pgsinblk;
     
    for (i=0; i< mtl_blkcnt; i++) 
    {
        if (mtl_blocktab[i].mtindx == mtindex) 
        {
            for (j=0; j < mtl_blocktab[i].valid; j++) 
            {
                if (mtl_transtab[pageofs +j].pageofs == mtpgofs) 
                {                    
                    *retblk = mtl_blocktab[i].pba;
                    *retpg  = mtl_transtab[pageofs +j].ppa;
                    ret = FIND_OK;
                    return ret;
                }
            }
        }
        else
        {
            pageofs += mtl_blocktab[i].valid;
        }
    }
    *retblk = 0xffff;
    *retpg  = 0xff;
    ret = FIND_FAIL;
    return ret;
}

static void nftl_get_physical_mt(uint8_t mtpgofs, uint16_t *block, uint8_t *page) 
{
    uint8_t mtindex = mtpgofs / nr_pgsinblk;	

    if (nftl_find_mtpg_in_mtl(mtpgofs,block,page) != FIND_OK)
    {
    	//if it is not in mtl_blocktab, then it should be in mt_pbas[]!!
        *block = mt_pbas[mtindex];
        *page = mtpgofs % nr_pgsinblk;
		#ifndef MAGUS_LINUX
		test_flag=1;
		#endif
    }
    return;
}

static uint8_t nftl_find_mtl_ram(uint8_t mtindex,uint8_t *ramindex) 
{
    uint8_t i;
    uint8_t ret;
	
    
    for (i = 0; i < mtl_blkcnt; i++) 
    {
        if (mtl_blocktab[i].mtindx == mtindex) 
        {
            *ramindex = i;
            ret = FIND_OK;
            return ret;
        }
    }
    ret = FIND_FAIL;
    *ramindex = mtl_blkcnt;
    return ret;
}

static int nftl_update_mtl_ram(uint8_t operation,uint16_t index,uint16_t mtpgofs,uint16_t mtlblock,uint16_t new_mtlblock) 
{
    uint32_t i;
    uint8_t mtindex;
    uint32_t offset;    
    uint8_t tempValid;
	
    
    mtindex = mtpgofs / nr_pgsinblk;
    
    switch(operation) 
    {
        case APPEND_OPERATION:
            /*the DataLog block is not existed */
            mtl_blocktab[mtl_blkcnt].used = 1;
            mtl_blocktab[mtl_blkcnt].valid = 1;
            mtl_blocktab[mtl_blkcnt].pba = mtlblock;
            mtl_blocktab[mtl_blkcnt].mtindx = mtindex;
            mtl_transtab[mtl_pgcnt].pageofs = mtpgofs;
            mtl_transtab[mtl_pgcnt].ppa= 0;
            mtl_blkcnt ++;
            mtl_pgcnt ++;
            break;
        case INSERT_OPERATION:
            /*the DataLog block is existed */
            offset = 0;
            for(i=0;i<index;i++)
            {
                offset += mtl_blocktab[i].valid;
            }
            for(i = offset; i< offset + mtl_blocktab[index].valid; i++)
            {
                if(mtl_transtab[i].pageofs == mtpgofs)
                {
                    break;
                }
            }
            
            if(i != offset + mtl_blocktab[index].valid) 
            {
                /* find pageofs in mtl_transtab */
                mtl_transtab[i].ppa = mtl_blocktab[index].used;
                mtl_blocktab[index].used++;
            }
            else 
            {
                /* not find pageofs in mtl_transtab */
                for( i= mtl_pgcnt; i > offset + mtl_blocktab[index].valid; i--)
                {
                    /* move down all of the following record on ram */
                    mtl_transtab[i].pageofs = mtl_transtab[i-1].pageofs;
                    mtl_transtab[i].ppa = mtl_transtab[i-1].ppa;
                }
                mtl_transtab[i].pageofs = mtpgofs;
                mtl_transtab[i].ppa = mtl_blocktab[index].used;
                mtl_blocktab[index].used ++;
                mtl_blocktab[index].valid ++;
                mtl_pgcnt++;
            }
            
            break;
        case DEL_OPERATION:
            /* delete the map block on ram */
            offset = 0;
            for(i = 0; i < index; i++)
            {
                offset += mtl_blocktab[i].valid;
            }
            tempValid = mtl_blocktab[index].valid;
            
            for(i=offset; i < mtl_pgcnt - tempValid; i++) 
            {
                /* move up all of the following record on ram */
                mtl_transtab[i].pageofs = mtl_transtab[i+tempValid].pageofs;
                mtl_transtab[i].ppa = mtl_transtab[i+tempValid].ppa;
            }
            
            for(i = index; i < mtl_blkcnt; i++) 
            {
                /* move up all of the following record on ram */
                mtl_blocktab[i].mtindx = mtl_blocktab[i+1].mtindx;
                mtl_blocktab[i].pba = mtl_blocktab[i+1].pba;
                mtl_blocktab[i].used = mtl_blocktab[i+1].used;
                mtl_blocktab[i].valid = mtl_blocktab[i+1].valid;
            }
            mtl_pgcnt = mtl_pgcnt - tempValid;
            mtl_blkcnt--;
            break;
        
        
        case UPDATE_OPERATION:        	
	       	{
				//uint16_t old_mtlblock;
				//old_mtlblock = mtpgofs;
				
			    for (i=0; i< mtl_blkcnt; i++) 
			    {
			        if (mtl_blocktab[i].mtindx == mtindex) 
			        {
			         	if (mtl_blocktab[i].pba == mtlblock) 
			            {
			            	//updated to new mtlblock
			            	mtl_blocktab[i].pba = new_mtlblock;        		
							return (int)(mtl_blocktab[i].used);
			            }
			        }    		    
			    }        	
				//should not be happened			
				nftl_error("nftl update mtl ram : UPDATE_OPERATION error\n");
				return -1;        	
			}	
        	break;
        	    
        default:
            return -1;
    }
    return 0;
}

static uint8_t nftl_merge_mtlblk(struct mtd_info *mtd, uint8_t ramindex)
{
    int ret=0;
    uint16_t tempmtblock;
    uint16_t i,j;
    
    uint16_t offset = 0;
    uint8_t  mtindex;
    uint16_t startpgofs;
        
    uint8_t factppa[MAX_PAGES_IN_BLOCK];
    uint16_t tempgarbages[2];
	nftl_debug("nftl merge mtlblk()\n");

    mtindex = mtl_blocktab[ramindex].mtindx;
    startpgofs = mtindex * nr_pgsinblk;

    tempmtblock = nftl_get_free_block();
    
    for(i = 0; i < ramindex; i++)
    {
        offset += mtl_blocktab[i].valid;
    }
    
    for(i = 0; i < nr_pgsinblk; i++) 
    {
        factppa[i] = 0xff;
        for(j = 0; j < mtl_blocktab[ramindex].valid; j++)
        {
            if( (startpgofs + i) == mtl_transtab[offset+j].pageofs)
            {
                factppa[i] = mtl_transtab[offset+j].ppa;
            }
        }
    }
    
    /* copy back to MapTab block */
    for(i = 0; i < nr_pgsinblk; i++) 
    {
        if(factppa[i] != 0xff) 
        {
            /* copy the MapTab data from the MTLog block */
            ret = nftl_copyback(mtd, mtl_blocktab[ramindex].pba,factppa[i], tempmtblock, i, MAP_TAB_BLOCK);
            
            if(ret==EIO) return EIO;
            		
            if (ret)
            {
                tempmtblock = nftl_copyback_error(mtd,mtl_blocktab[ramindex].pba,
                                    factppa[i],tempmtblock,i,MAP_TAB_BLOCK);
                ret=0;	//assume we have solved the problem !!
            }
        }
        else
        { 
            /* copy the MapTab data from the old MapTab block */
            ret = nftl_copyback(mtd, mt_pbas[mtindex],i,tempmtblock,i,MAP_TAB_BLOCK);
            if(ret==EIO) return EIO;
            if (ret) 
            {
                tempmtblock = nftl_copyback_error(mtd,mt_pbas[mtindex],
                                    i,tempmtblock,i,MAP_TAB_BLOCK);
                ret=0;	//assume we have solved the problem !!                    
            }
        }
    }

    /* add to the garbage map block */
    tempgarbages[0] = mt_pbas[mtindex];
    tempgarbages[1] = mtl_blocktab[ramindex].pba;

    ret = nftl_addto_gt(mtd, tempgarbages,2);

    /* update mtl_blocktab & mtl_transtab on ram*/
    mt_pbas[mtindex] = tempmtblock;
    ret = nftl_update_mtl_ram(DEL_OPERATION ,ramindex,0x00,0x00,0x00);
    return ret;  
}

static int nftl_update_mt(struct mtd_info *mtd, uint16_t lba, uint16_t pba) 
{
    int ret = 0;
	size_t retlen;
    uint16_t mtblock, mtlblock;
    uint8_t mtpage, mtlpage;
   	// uint8_t databuff[NAND_MAX_PAGESIZE];
    block_info block_info;
    uint8_t *pBS;
    uint8_t ramindex;
    uint8_t mtindex;
    uint8_t mtpgofs;
    uint32_t wordofs;
    uint8_t operation;
		
	#ifndef MT_READCACHE	
	uint8_t *databuff=kmalloc(NAND_MAX_PAGESIZE,GFP_KERNEL);
	if(!databuff)
	{
		nftl_error("malloc memory error\n");
		return 1;
	}
	/*should memset to oxff, otherwise it won't return 0xffff if pda not found*/
	memset(databuff,0xff,NAND_MAX_PAGESIZE);	
	#endif


    mtpgofs = lba / ( mtd->writesize / 2 );
    mtindex = mtpgofs / nr_pgsinblk;
    wordofs = lba % ( mtd->writesize / 2);
    
    //search in ram
    nftl_get_physical_mt(mtpgofs, &mtblock, &mtpage);
  
    
    #ifdef MT_READCACHE
  	if((mtblock == mt_cacheblock) && (mtpage == mt_cachepage))
    {        	
    	//read write cache data to read cache
        //memcpy(databuff, mt_readcache, mtd->writesize);		
        ret=0;
    }
    else
    {           	
        ret = mtd_read(mtd, mtblock, mtpage, &retlen, mt_readcache, NULL, 1);            
		mt_cacheblock = mtblock;
		mt_cachepage = mtpage;            	
	        
    }      
    
    /* update the MapTab data */
    mt_readcache[wordofs*2]=(uint8_t)(pba);
    mt_readcache[wordofs*2+1]=(uint8_t)(pba>>8);
        
    #else
    
    //read the original MapTab data
    ret = mtd_read(mtd, mtblock, mtpage, &retlen, databuff, NULL, 1);    
    
    /* update the MapTab data */
    databuff[wordofs*2]=(uint8_t)(pba);
    databuff[wordofs*2+1]=(uint8_t)(pba>>8);
        
    #endif
        
    
    
    


    /* write to MTLog */
    if(nftl_find_mtl_ram(mtindex,&ramindex) == FIND_FAIL)
    {
        /* the MTLog block is not existed */
        operation = APPEND_OPERATION;
        mtlblock = nftl_get_free_block();
        mtlpage = 0;
    }
    else
    {
        /* the MTLog block is existed */
        operation = INSERT_OPERATION;
        mtlblock = mtl_blocktab[ramindex].pba;
        mtlpage  = mtl_blocktab[ramindex].used;
    }

    /* write data to MTLog block */
    block_info.blocktype = MT_LOG_BLOCK;
    block_info.mtlofs.pageofs = mtpgofs;
    block_info.mtlofs.tabindx = mtindex;
    
    pBS = ((block_info_union *)&block_info)->asbyte;
    
    #ifndef MT_READCACHE	
    
    ret = mtd_write(mtd, mtlblock, mtlpage, &retlen, databuff,pBS,1);

	#else
		
	ret = mtd_write(mtd, mtlblock, mtlpage, &retlen, mt_readcache,pBS,1);	
	mt_cacheblock = mtlblock;
	mt_cachepage = mtlpage;            	
	#endif
    
    if(ret || retlen != mtd->writesize + mtd->oobsize) 
    {    	
    	nftl_msg("nftl update mt error ... \n");    	
        /* change the DataLog */
        if(operation == INSERT_OPERATION) 
        {
        	#ifndef MT_READCACHE	
            mtl_blocktab[ramindex].pba = nftl_write_error(mtd,mtlblock, mtlpage, databuff,pBS,1);
            #else
            mtl_blocktab[ramindex].pba = nftl_write_error(mtd,mtlblock, mtlpage, mt_readcache,pBS,1);
            #endif
            
            mtlpage = mtl_blocktab[ramindex].pba;
        }
        else 
        {
        	#ifndef MT_READCACHE	
            mtlpage = nftl_write_error(mtd, mtlblock, mtlpage, databuff,pBS,1);
            #else
            mtlpage = nftl_write_error(mtd, mtlblock, mtlpage, mt_readcache,pBS,1);
            #endif
        }
        ret=0;	//assume we have solved the problem !!
    }
    /* update mtl_blocktab & mtl_transtab on ram*/
    ret = nftl_update_mtl_ram(operation,ramindex,mtpgofs,mtlblock,0x00);
        
    /* check whether the MTLog block is used up */
    if(mtl_blocktab[ramindex].used == nr_pgsinblk)
    {
        /* merge the MTLog block */
        ret = nftl_merge_mtlblk(mtd, ramindex);
    }
    
    #ifndef MT_READCACHE	
	if(databuff) kfree(databuff);
	#endif
		
    return ret;
}

static uint8_t nftl_find_lpa_in_dl(uint32_t targetlpa,uint16_t *retblk, uint8_t *retpg)
{
    uint16_t i,j;
    uint8_t ret = FIND_OK;
    uint16_t pageofs = 0;/* the page offset in dl_transtab on RAM */
    uint16_t lba = targetlpa / nr_pgsinblk; /* convert to LBA */;
	
    for (i=0; i< dl_blkcnt; i++) 
    {
    	//found logic blk in data log blk
        if (dl_blocktab[i].lba == lba)
        {
            for (j=0;j<dl_blocktab[i].valid;j++) 
            {
            	//found logic page in translation table
                if (dl_transtab[pageofs +j].lpa == targetlpa) 
                {
                    /* find targetlpa on TransTabOnRAM */
                    *retblk = dl_blocktab[i].pba;
                    *retpg = dl_transtab[pageofs +j].ppa;
                    ret = FIND_OK;
                    return ret;
                }
            }
        }
        else
        {
        	//increment offset, skip translation of current data log blk 
            pageofs += dl_blocktab[i].valid;
        }
    }
    ret = FIND_FAIL;
    return ret;
}

static int nftl_update_dl_ram(uint8_t operation,uint16_t index,uint32_t lpa,uint16_t pba) 
{
    uint16_t i;
    uint16_t lba = lpa / nr_pgsinblk;
    uint16_t pageofs = 0;/* the page offset in dl_transtab on RAM */
    uint8_t validtmp;
	    
    switch(operation) 
    {
        case APPEND_OPERATION:      /*the DataLog block is not existed */
            dl_blocktab[dl_blkcnt].used = 1;
            dl_blocktab[dl_blkcnt].valid = 1;
            dl_blocktab[dl_blkcnt].pba = pba;
            dl_blocktab[dl_blkcnt].lba = lba;            
            dl_transtab[dl_pgcnt].lpa = lpa;
            dl_transtab[dl_pgcnt].ppa= 0;
            dl_blkcnt++;
            dl_pgcnt++;
            break;
        case INSERT_OPERATION:      /*the DataLog block is existed */
            for(i=0; i < index;i++)
            {
                pageofs += dl_blocktab[i].valid;
            }
            for(i = pageofs; i< (pageofs + dl_blocktab[index].valid); i++) 
            {
                if(dl_transtab[i].lpa == lpa)
                {
                    break;
                }
            }
            
            if(i != pageofs + dl_blocktab[index].valid) 
            {
                /* find lpa in DataLogOnRAM */
                dl_transtab[i].ppa = dl_blocktab[index].used;
                dl_blocktab[index].used++;
            }
            else 
            {
                /* not find lpa in DataLogOnRAM */
                for(i= dl_pgcnt; i > (pageofs + dl_blocktab[index].valid); i--)
                {
                    /* move down all of the following record on ram */
                    dl_transtab[i].lpa = dl_transtab[i-1].lpa;
                    dl_transtab[i].ppa = dl_transtab[i-1].ppa;
                }
                                
                dl_transtab[i].lpa = lpa;
                dl_transtab[i].ppa = dl_blocktab[index].used;
                dl_blocktab[index].used++;
                dl_blocktab[index].valid++;
                
                dl_pgcnt++;
            
            }
            break;
        case DEL_OPERATION:        /* delete the map block on ram */
            
            for(i = 0; i < index; i++)
            {
                pageofs += dl_blocktab[i].valid;
            }
            validtmp = dl_blocktab[index].valid;
            
            for(i=pageofs ; i < dl_pgcnt - validtmp ; i++) 
            {
                /* move up all of the following record on ram */
                dl_transtab[i].lpa = dl_transtab[i+validtmp].lpa;
                dl_transtab[i].ppa = dl_transtab[i+validtmp].ppa;
            }

            for(i = index; i < dl_blkcnt; i++) 
            {
                /* move up all of the following record on ram */
                dl_blocktab[i].lba = dl_blocktab[i+1].lba;
                dl_blocktab[i].pba = dl_blocktab[i+1].pba;
                dl_blocktab[i].used = dl_blocktab[i+1].used;
                dl_blocktab[i].valid = dl_blocktab[i+1].valid;
            }
                
            dl_pgcnt = dl_pgcnt- validtmp;
            dl_blkcnt--;
            break;
        
        case UPDATE_OPERATION:        	
        	
        	for(i = 0; i < dl_blkcnt; i++) 
        	{
        		if(dl_blocktab[i].lba == lba)
				{
        			dl_blocktab[i].pba = pba;        		
					return (int)(dl_blocktab[i].used);
				}
        	}
			//should not be happened			
			nftl_error("nftl update dl ram : UPDATE_OPERATION error\n");
			return -1;
        	
        	            
        default:
            return -1;
    }
    return 0;
}

static uint8_t nftl_find_lba_in_dl(uint16_t lba,uint16_t *ramindex) 
{
    uint16_t i;
    uint8_t ret;
	    
    ret = FIND_FAIL;
    for(i=0; i< dl_blkcnt; i++)
    {
        if ((dl_blkcnt != 0) && (dl_blocktab[i].lba == lba))
        {
            *ramindex = i;
            ret = FIND_OK;
            return ret;
        }
    }
    *ramindex = dl_blkcnt;
    return ret;
}



static int nftl_lba_to_pba(struct mtd_info *mtd, uint16_t lba, uint16_t *pba)
{
    uint32_t mt_pgofs;   /* page offset in Mapping Table */
    uint32_t byteofs_in_pg;
    uint16_t mt_block;
    uint8_t mt_page;
   	// uint8_t databuff[NAND_MAX_PAGESIZE];
    int ret = 0;
	size_t retlen;
		
	#ifndef MT_READCACHE 	
	uint8_t *databuff=kmalloc(NAND_MAX_PAGESIZE,GFP_KERNEL);
	if(!databuff)
	{
		nftl_error("malloc memory error\n");
		return 1;
	}	
	/*should memset to oxff, otherwise it won't return 0xffff if pda not found*/
	memset(databuff,0xff,NAND_MAX_PAGESIZE);
	#endif

    /* compute the page offset in Mapping Table according to LBA */
    mt_pgofs = lba / (mtd->writesize / 2);
    byteofs_in_pg = (lba % (mtd->writesize / 2)) *2;

	//search in ram only, either in mtl_blocktab[] or mt_pbas[]
    nftl_get_physical_mt(mt_pgofs,&mt_block,&mt_page);
    

	#ifndef MT_READCACHE 	
	ret = mtd_read(mtd, mt_block, mt_page, &retlen, databuff,NULL,1);
	#else
	if(mt_block == mt_cacheblock && mt_page == mt_cachepage)
    {        	
    	//read write cache data to read cache
        //memcpy(databuff, mt_readcache, mtd->writesize);		
        ret=0;
    }
    else
    {           	
        //ret = mtd_read(mtd, mt_block, mt_page, &retlen, databuff,NULL,1);				
        ret = mtd_read(mtd, mt_block, mt_page, &retlen, mt_readcache,NULL,1);				
    	mt_cacheblock = mt_block;
    	mt_cachepage = mt_page;            
		//memcpy(mt_readcache, databuff,mtd->writesize);		
    }
	#endif

    if(!ret) 
    {
        /* read the PBA*/
        #ifndef MT_READCACHE 
        *pba = * ( (uint16_t *) (databuff + byteofs_in_pg) );    		
        #else
        *pba = * ( (uint16_t *) (mt_readcache + byteofs_in_pg) );    		
        #endif
		if(mtd->ecc_stats.corrected > ECC_ERR_LIMIT)
		{
			//operation still allow						
			//we do block move here		
			//find the location of mt
			if(FIND_OK==nftl_find_mtpg_in_mtl(mt_pgofs,&mt_block,&mt_page))
			{
			
				uint16_t new_blk;
				uint8_t replace_page_count;
								
				//nftl_error("nftl lba to pba 1, reach ECC_ERR_LIMIT \n");				
				#if 1
	    		new_blk = nftl_get_free_block();
				if(new_blk==0xFFFF)
	   			{
					nftl_error("have no free block\n");
					return 1;
	     		}			    		
	     		
	     		
	     		replace_page_count = nftl_update_mtl_ram(UPDATE_OPERATION,0,mt_page,mt_block,new_blk);
	     		
				if(replace_page_count<0) 
				{	
					nftl_error("nftl lba to pba 1, update dl ram err \n");
					return 1;
				}
	     		
	     		
				ret = nftl_block_replace(mtd, mt_block, new_blk, replace_page_count);
				if(ret) 
				{	
					nftl_error("nftl lba to pba 1, backreplace err \n");
					return 1;
				}
				
				ret = nftl_addto_gt(mtd, &mt_block,1);
				if(ret) 
				{	
					nftl_error("nftl lba to pba 1, addto gt err \n");
					return 1;
				}		
				nftl_error("nftl lba to pba 1, block move from %04d to %04d\n",mt_block,new_blk);
	     			     			     		
	     		#endif				
			}
			else		
			{				
				uint16_t new_blk;
				uint8_t replace_page_count;
				uint8_t mtindex = mt_pgofs / nr_pgsinblk;					
				
				//nftl_error("nftl lba to pba 2, reach ECC_ERR_LIMIT \n");
				#if 1
	    		new_blk = nftl_get_free_block();
				if(new_blk==0xFFFF)
	   			{
					nftl_error("have no free block\n");
					return 1;
	     		}			    		
	     		    		
    			//we need to update mt_pbas[] only
		        mt_block = mt_pbas[mtindex];
        		mt_page = mt_pgofs % nr_pgsinblk;    			
    			mt_pbas[mtindex] = new_blk;
        	    	    			     		
				ret = nftl_block_replace(mtd, mt_block, new_blk, nr_pgsinblk);
				if(ret) 
				{	
					nftl_error("nftl lba to pba 2, backreplace err \n");
					return 1;
				}
				
				ret = nftl_addto_gt(mtd, &mt_block,1);
				if(ret) 
				{	
					nftl_error("nftl lba to pba 2, addto gt err \n");
					return 1;
				}		
				nftl_error("nftl lba to pba 2, block move from %04d to %04d\n",mt_block,new_blk);
	     		
	     		#endif				
			
			}

			
		}
    }
    if(*pba!=0xffff)
    {	
		if(*pba>nr_blks)
			*pba=0xffff;				
	}
	
	#ifndef MT_READCACHE 
    if(databuff) kfree(databuff);
    #endif	
	 
    return ret;
}


static int nftl_merge_dlblk(struct mtd_info *mtd, uint16_t index) 
{
    int ret = 0;
	size_t retlen;
    uint16_t i,j;
    uint8_t factppa[MAX_PAGES_IN_BLOCK];
    uint16_t lba, pba;
    uint32_t startlpa;
    uint16_t offset = 0;
    uint16_t tempdatablock;
    uint16_t tempgarbages[2];
    uint8_t tempgarbagecnt;
    uint8_t *pBS;
    block_info block_info;
    uint8_t *tempbuf=kmalloc(NAND_MAX_PAGESIZE,GFP_KERNEL);    
    //nftl_debug("nftl merge dlblk()\n");                
    
    lba = dl_blocktab[index].lba;
    startlpa = lba * nr_pgsinblk;
	
	//nftl_dumpmsg("merge dl blk lba = %d \n",lba);	
	//calc dl_blocktab offset, coz we have many dl block
    for(i = 0; i < index; i++)
    {
        if(index != 0)
        {
            offset += dl_blocktab[i].valid;
        }
    }

    for (i = 0; i < nr_pgsinblk; i++) 
    {
        factppa[i] = 0xff;
        for (j=0; j < dl_blocktab[index].valid; j++)
        {
			//find valid ppa in dl block, and store it to factppa[]
			//it will less then nr_pgsinblk
            if ((startlpa + i) == dl_transtab[offset+j].lpa)
            {
                factppa[i] = dl_transtab[offset+j].ppa;
            }
        }
    }
    
	//pba may return 0xffff, it means pba not exists in map table
	//the reason is we are going to create a new data block
    ret = nftl_lba_to_pba(mtd, lba,&pba);

    /* copy data from the DataLog block 
     * or the old Data block to a new Data block */

    /* allocate a free block as the target usr data block*/
    tempdatablock = nftl_get_free_block();

    if(tempdatablock==0xFFFF)//add by kevinwu
	{
		nftl_error("there is no free block for merge\n");
		if(tempbuf)
	 		kfree(tempbuf);
		return -1;
	}

    for(i = 0;i < nr_pgsinblk;i++) 
    {
        if(factppa[i] != 0xff) 
        {			
            /* copy the data from the DataLog block */
            ret = nftl_copyback(mtd, dl_blocktab[index].pba, factppa[i], tempdatablock,i, DATA_BLOCK);
	  		if(ret==EIO) return EIO;
            if (ret) 
            {
            	nftl_msg("nftl merge dlblk error handling ...\n");
                tempdatablock = nftl_copyback_error(mtd,
                                 dl_blocktab[index].pba, factppa[i],
                                 tempdatablock, i, DATA_BLOCK);
				ret=0;	//assume we have solved the problem !!				                                 
		 
            }
        }
        else 
        {
            if(pba != 0xffff) 
            {
                /* copy data from old data block to new datablock */
                ret = nftl_copyback(mtd, pba, i, tempdatablock, i, DATA_BLOCK);
                if(ret==EIO) return EIO;
                if (ret)
                {
                	nftl_msg("nftl merge dlblk error1 ...\n");
                    tempdatablock = nftl_copyback_error(mtd,
                                        pba, i, tempdatablock, i, DATA_BLOCK);
                    ret=0;	//assume we have solved the problem !!				                                 
                }
            }
            else if (i == 0)
            {
                /* some times the 1st page has no valid data, but must make sure
                 * it is a Data block, so mark the spare area to denote. */
                pBS = ((block_info_union *)&block_info)->asbyte;
                block_info.blocktype = DATA_BLOCK;  
                ret = mtd_write(mtd, tempdatablock, i, &retlen, tempbuf, pBS, 1);
                if (ret)
                {
                	nftl_msg("nftl merge dlblk error2 ...\n");
                    tempdatablock = nftl_write_error(mtd, 
                                        tempdatablock, i ,tempbuf, pBS, 1);
                    ret=0;	//assume we have solved the problem !!				                                 
                }
                
            }
        }

       if(tempdatablock==0xFFFF)//add by kevinwu
    	{
    		nftl_error("there is no free block for copy back\n");
			if(tempbuf)
	 			kfree(tempbuf);
			return -1;
    	}
    }
    /* find the garbage blocks */
    tempgarbagecnt = 0;

    /*mark the old DataLog block as garbage block */
    tempgarbages[0] = dl_blocktab[index].pba;
    tempgarbagecnt ++;  
    if(pba != 0xffff) 
    {
        /* add the old data block to the garbage map block */
        tempgarbages[1] = pba;
        tempgarbagecnt ++;
    }
	ret = nftl_addto_gt(mtd, tempgarbages,tempgarbagecnt);    
    /* update the MapTab block */
    ret = nftl_update_mt(mtd, lba,tempdatablock);    
    /* update the dl_blocktab & dl_transtab */
    ret = nftl_update_dl_ram(DEL_OPERATION,index,0x0,0x0);

	if(tempbuf)
		kfree(tempbuf);
    return ret;
}

int nftl_read_logicpage(struct mtd_info *mtd, uint32_t lpa,uint8_t *databuff) 
{

    int ret = 0;
    size_t retlen;
    uint16_t lba, pba;
    uint8_t pageofs;
	
    /* Check lpa is a valid logic page address; */
    if(lpa < 0 || lpa >= logparam.maxlp)
    {
        nftl_error("the logic page is out of the legal page address, lpa = %d \n",lpa);
        return -1;
    }
    
    /*scan dl_transtab to find LPA; */
    if ( nftl_find_lpa_in_dl(lpa, &pba, &pageofs) == FIND_OK ) 
    {
        /* usr data stored on DataLog block */		
        ret = mtd_read(mtd, pba, pageofs, &retlen, databuff, NULL,1);

		
		if(mtd->ecc_stats.corrected > ECC_ERR_LIMIT)
		{			
			//operation still allow, we do block move here			
			uint16_t new_blk;
			uint8_t replace_page_count;
			
			//nftl_error("nftl read logicpage 1 reach ECC_ERR_LIMIT \n");			
			#if 1
    		new_blk = nftl_get_free_block();
			if(new_blk==0xFFFF)
   			{
				nftl_error("have no free block\n");
				return 1;
     		}			    		
			
			//we need to update dl_blocktab.pba only
			//we locate current dl_blocktab.lba and then update dl_blocktab.pba with new_blk
			replace_page_count = nftl_update_dl_ram(UPDATE_OPERATION,0,lpa,new_blk);
			if(replace_page_count<0) 
			{	
				nftl_error("nftl read logicpage 1, update dl ram err \n");
				return 1;
			}

     		//only replace used page
			ret = nftl_block_replace(mtd, pba, new_blk, replace_page_count);
			if(ret) 
			{	
				nftl_error("nftl read logicpage 1, backreplace err \n");
				return 1;
			}
			
			ret = nftl_addto_gt(mtd, &pba,1);
			if(ret) 
			{	
				nftl_error("nftl read logicpage 1, addto gt err \n");
				return 1;
			}		
			nftl_error("nftl read logicpage 1, block move from %04d to %04d\n",pba,new_blk);
			#endif

		}	
		return ret;        
    }
    else 
    {
        /* usr data stored on Data block */
        lba = lpa / nr_pgsinblk;
        /*find PBA from Mapping Table according to LBA; */
       	// ret = nftl_lba_to_pba(mtd, lba, &pba);
		nftl_lba_to_pba(mtd, lba, &pba);//modifid by kevinwu
        if(pba != 0xFFFF)
        {
            pageofs = lpa % nr_pgsinblk;            
            ret = mtd_read(mtd, pba, pageofs, &retlen, databuff, NULL,1);
			
			//if (ret == ECC_ERR_LIMIT)
			if(mtd->ecc_stats.corrected > ECC_ERR_LIMIT)
			{
				//operation still allow				
				//ret = 0;
				uint16_t new_blk;
				
				//nftl_error("nftl read logicpage 2 reach ECC_ERR_LIMIT \n");				
				//we do block move here				
    			new_blk = nftl_get_free_block();
			    if(new_blk==0xFFFF)
   				{
					nftl_error("have no free block\n");
					return 1;
     			}				
				ret = nftl_block_replace(mtd, pba, new_blk, nr_pgsinblk);
				if(ret) 
				{	
					nftl_error("nftl read logicpage 2, backreplace err \n");
					return 1;
				}		
				ret = nftl_update_mt(mtd, lba, new_blk);
				if(ret) 
				{	
					nftl_error("nftl read logicpage 2, update mt err \n");
					return 1;
				}		
								
				ret = nftl_addto_gt(mtd, &pba,1);
				if(ret) 
				{	
					nftl_error("nftl read logicpage 2, addto gt err \n");
					return 1;
				}		
				nftl_error("nftl read logicpage 2, block move from %04d to %04d\n",pba,new_blk);
			}				
            return ret;
        }
        else
        {
            /*  there is not existed physical block */
            memset(databuff, 0xff, mtd->writesize);	 
            return ret;
        }
    }

}


uint8_t nftl_write_logicpage(struct mtd_info *mtd, uint32_t lpa, uint8_t *databuff) 
{

    int ret = 0;
    size_t retlen;
    uint16_t lba;    
    block_info block_info;
    uint8_t *pBS;    
    uint16_t i, index=0, tempindex;    
    uint16_t tempblock;
    uint16_t temppage;
    uint16_t pba;
    uint8_t need_convert = FALSE; /* if need to convert DataLog to Data */
    uint8_t  operation;
    
    
    //nftl_dumpmsg("nftl write logicpage lpa = %d \n",lpa);
        
    /* Compute LBA and LPA ,and SectorOffset by lpa, 
     * formulations are depicted as following. */
    lba = lpa / nr_pgsinblk;

   	//if(dl_blkcnt >= DL_MAX_BLOCKS) 		//stanley, DL_MAX_BLOCKS = 128
    //if(dl_blkcnt >= DL_MAX_BLOCKS/8)//modfied by kevinwu 
    if(dl_blkcnt >= DL_MAX_BLOCKS_DIV4)// try to improve speed a little bit
    {
    	
    	nftl_dumpmsg("merge dl blk ,big dl_blkcnt = %d \n",dl_blkcnt);
        //while(dl_blkcnt > DL_MAX_BLOCKS/2)		//stanley 
	    while(dl_blkcnt > DL_MAX_BLOCKS_DIV8) //modified by kevinwu
        {
            /*find the block which has the maxium used pages */
            tempindex = 0;
            for(i=0; i< dl_blkcnt ;i++)
            {
                if(dl_blocktab[tempindex].used < dl_blocktab[i].used)
                {
                    tempindex = i;
                }
            }   

			//find datalog block has data block
			//coz it will free 2 blocks out
			for(i=0; i< dl_blkcnt ;i++)
			{
				uint16_t mypba;				
				ret=nftl_lba_to_pba(mtd, dl_blocktab[i].lba ,&mypba);
				if(!ret && mypba!=0xffff)
				{
					tempindex=i;
					break;
				}
			}	
			if(i==dl_blkcnt)
			{
				nftl_dumpmsg("merge dl blk cancel 1 lpa = %d \n",lpa);
				break;
			}	
			//end


            /* merge the block */
            nftl_dumpmsg("merge dl blk lpa = %d ; dl_blkcnt = %d ; waste = %d \n",lpa,dl_blkcnt,i);
            ret = nftl_merge_dlblk(mtd, tempindex);
		     if(ret) return -1;
        }
    }

	//26 Aug 08 change DL_MAX_PAGES = 4096 for speed up

   	//if(dl_pgcnt >= DL_MAX_PAGES ) //stanley DL_MAX_PAGES = 1024, //stanley, this cause USB disconnect
	if(dl_pgcnt >= DL_MAX_PAGES_DIV4 ) //this cause cause very slow	
    {		
        /*  merge those DataLog blocks with the most pages */
       	//while(dl_pgcnt > DL_MAX_PAGES/2 )	//stanley, this cause USB disconnect
       	
       	nftl_dumpmsg("merge dl blk , too much dl_pgcnt = %d \n",dl_pgcnt);
	 	while(dl_pgcnt > DL_MAX_PAGES_DIV8 )	//this cause cause very slow		 	
        {
            /*find the block which has the maxium used pages */
            tempindex=0;
            for(i=1; i< dl_blkcnt; i++)
            {
                if(dl_blocktab[tempindex].used < dl_blocktab[i].used)
                {
                    tempindex = i;
                }
            }
			//find datalog block has data block
			//coz it will free 2 blocks out
			for(i=0; i< dl_blkcnt ;i++)
			{
				uint16_t mypba;				
				ret=nftl_lba_to_pba(mtd, dl_blocktab[i].lba ,&mypba);
				if(!ret && mypba!=0xffff)
				{
					tempindex=i;
					break;
				}
			}	
			if(i==dl_blkcnt)
			{
				nftl_dumpmsg("merge dl blk cancel 2 lpa = %d ; dl_pgcnt = %d \n",lpa,dl_pgcnt);					
				break;
			}	
			//end
			nftl_dumpmsg("merge dl blk 2 lpa = %d ; dl_pgcnt = %d ; waste = %d \n",lpa,dl_pgcnt,i);
			ret = nftl_merge_dlblk(mtd, tempindex);    /*merge the block */
			if(ret)	return -1;
        }
    }

    if(nftl_find_lba_in_dl(lba, &index) == FIND_OK) 
    {
        /* write data to corresponding DataLog block */    
        tempblock = dl_blocktab[index].pba;
        temppage = dl_blocktab[index].used;
        
        //nftl_dumpmsg("write data to corresponding DataLog block = %d \n",tempblock);
        
        if(temppage == nr_pgsinblk - 1)
        {
			//DataLog block full, we need to convert it to DataBlk
            uint8_t factppa[MAX_PAGES_IN_BLOCK];
            uint32_t startlpa;
            uint16_t offset;
            int j;

            offset = 0;
            for(i=0;i<index;i++)
            {
                if(index != 0)
                {
                    offset += dl_blocktab[i].valid;
                }
            }

            need_convert = TRUE;
            startlpa = lba * nr_pgsinblk;
            for (i = 0; i < nr_pgsinblk - 1; i++)
            {
                factppa[i] = 0xff;
                for (j = 0; j < dl_blocktab[index].valid; j++)
                {
                    if ((startlpa+i) == dl_transtab[offset+j].lpa)
                    {
                        factppa[i] = dl_transtab[offset+j].ppa;
                    }
                }
                if (factppa[i] != i)
                {
                    need_convert = FALSE;
                    break;
                }
            }
        }
        if (need_convert == TRUE)
        {
            block_info.blocktype = DATA_BLOCK;
            operation = DEL_OPERATION;
        }
        else
        {
            block_info.blocktype = DATA_LOG_BLOCK;
            operation = INSERT_OPERATION;
        }
    }
    else 
    {
        /* allocate a free block as DataLog block */                      
        tempblock = nftl_get_free_block();
        temppage = 0;
        block_info.blocktype = DATA_LOG_BLOCK;        
        operation = APPEND_OPERATION;
        //nftl_dumpmsg("allocate a free block as DataLog block = %d \n",tempblock);
    }

    block_info.lpa = lpa;
    pBS = ((block_info_union *)&block_info)->asbyte;
    ret = mtd_write(mtd, tempblock, temppage, &retlen, databuff, pBS, 1);

    if(ret) 
    {
    	nftl_msg("nftl write logicpage error ... \n");
    	
        /* change the DataLog */
        if(operation == INSERT_OPERATION) 
        {
        	nftl_msg("nftl write logicpage error ...\n");
        	
           	dl_blocktab[index].pba = nftl_write_error(mtd,
                                            tempblock, temppage,databuff,pBS,1);
           	tempblock = dl_blocktab[index].pba;
        }
        else 
        {
            tempblock = nftl_write_error(mtd, tempblock, temppage,databuff,pBS,1);
        }
        ret=0;	//assume we have solved the problem !!
    }

    if(need_convert == TRUE)
    {
        nftl_lba_to_pba(mtd, lba, &pba);
        if(pba != 0xffff) 
        {
            /* add the old data block to the garbage map block */
            ret = nftl_addto_gt(mtd, &pba,1);
        }
        /* update the MapTab block */
        ret = nftl_update_mt(mtd, lba, tempblock);
        if(ret)	return -1;
    }

    /* update the dl_blocktab & dl_transtab */
    nftl_update_dl_ram(operation,index,lpa,tempblock);

    if (dl_blocktab[index].used == nr_pgsinblk)
    {
        /* if the used page of log block is up to the maxmum page , 
         * then merge it  */
         nftl_dumpmsg("merge dl blk (dl full) lpa = %d \n",lpa);
        ret = nftl_merge_dlblk(mtd, index);
	 	if(ret)	return -1;
    }

    if (free_blkcnt <= MINI_FREE_BLOCK)
    {
        tempindex=0;
        for(i=1; i< dl_blkcnt;i++)
        {
            if(dl_blocktab[tempindex].valid < dl_blocktab[i].valid)
            {
                tempindex = i;
            }
        }        


		//find datalog block has data block
		//coz it will free 2 blocks out
		for(i=0; i< dl_blkcnt ;i++)
		{
			uint16_t mypba;				
			ret=nftl_lba_to_pba(mtd, dl_blocktab[i].lba ,&mypba);
			if(!ret && mypba!=0xffff)
			{
				tempindex=i;
				break;
			}
		}	
		if(i==dl_blkcnt)
		{	
			nftl_dumpmsg("merge dl blk cancel 3 lpa = %d \n", dl_blocktab[i].lba);	
			return ret;
		}	
		//end
        
        /* there is not enough free block , then merge the usr data blog  */
        nftl_dumpmsg("merge dl blk 4 lpa = %d \n",lpa);
        ret = nftl_merge_dlblk(mtd, tempindex);
		if(ret) return -1;
    }
    return ret;
}

int NFTL_mount(struct NFTLrecord *s)
{
    /* Initialize the global variable when the flash is power up*/
    block_info blkinfo;
    uint8_t *pBS;
    uint16_t i;
    uint8_t j = 0;
    uint8_t *blocktype;
    int ret = 0;
	size_t retlen;
   	struct mtd_info *mtd = s->mbd.mtd;
   
	//blkinfo = kzalloc(sizeof(block_info), GFP_KERNEL);
	blocktype = kzalloc(MAX_BLOCKS, GFP_KERNEL);   // MAX_BLOCKS=16384
	
    nr_pgsinblk = mtd->erasesize / mtd->writesize;
	nr_blks = s->nb_blocks;

    /* Initialize all the tables on RAM */
    //memset(blockstatus, 0x00, sizeof(uint8_t) * s->nb_blocks/8);
	//set all blocks not free
	memset(blockstatus, 0x00, sizeof(blockstatus));

    memset(gt_pbas, 0xff, sizeof(uint16_t) * GT_MAX_BLOCKS);

    mt_blkcnt = ((s->nb_blocks - 1) * 2) 
                    / (mtd->erasesize) + 1;
 
    memset(mt_pbas, 0xff, sizeof(uint16_t) * mt_blkcnt);
	memset(mtl_transtab,0x00,sizeof(mtl_transtab));

    /* Initialize logparam;*/
	//    logparam.maxlp = (s->nb_blocks - (MINI_FREE_BLOCK + 2*mt_blkcnt + 1)) 
         //   * mtd->erasesize / mtd->writesize;
    logparam.maxlp = s->nb_blocks* mtd->erasesize / mtd->writesize;//test by kevinwu
    logparam.startpb = 0;

	//    s->numvunits = (s->nb_blocks - (MINI_FREE_BLOCK + 2*mt_blkcnt + 1));
    /* Scan the 1st page of each Block;Initialize related information on RAM */
    //pBS = ((block_info_union *)blkinfo)->asbyte;
    //pBS=&blkinfo;
     pBS = ((block_info_union *)&blkinfo)->asbyte;
     
     

	#ifdef MT_READCACHE
	mt_cacheblock = 0xffff;
	mt_cachepage = 0xff;
	memset(mt_readcache,0xff,sizeof(mt_readcache));
	#endif

     
     
    /* 1. get the garbage map block */
    //for(i=0; i< nr_blks; i++) 
	for(i=0; i< s->nb_blocks;i++) 
    {
		
		
        ret = mtd_read(mtd, i, 0, &retlen, NULL, pBS, 1);
		
        if(!ret && retlen == mtd->oobsize) 
        {
            *(blocktype + i) = blkinfo.blocktype;
        }
		else
		{
		    if(blocktype)
				kfree(blocktype); //add by kevinwu
		   nftl_error("read spare area error\n");
		   return -1;
		}
		
		#ifndef MAGUS_LINUX
		if(i > s->nb_blocks -5)
		{
			*(blocktype + i)= BBT_BLOCK;
		}
		#endif


        if(*(blocktype + i) == GARBAGE_TAB_BLOCK && !mtd->block_isbad(mtd, i * mtd->erasesize)) 
        {
            uint8_t gt_index = blkinfo.gtinfo.tabindx;
	   		nftl_debug("Block %d is garbage table block\n", i);
            if (gt_index < GT_MAX_BLOCKS)
            {
                gt_pbas[gt_index] = i;
            }
        }
    }
    
    if(gt_pbas[0] != 0xffff)
    {
        if ((ret |= nftl_get_gar_blks(mtd, gt_pbas, &gt_pgcnt,
                      gar_pbas, &gar_blkcnt)) != 0)
        {
            nftl_error("Read spare error: %d block %d page \n",gar_blkcnt,i);
			 if(blocktype)
				kfree(blocktype); 
            return ret;
        }
    }



    /* 2. scan other valid block */
    for(i = 0; i <nr_blks; i++) 
    {		

		#ifndef MAGUS_LINUX
		if(i > nr_blks -10)
		{
			if(i > nr_blks - 5)
				*(blocktype + i)= BBT_BLOCK;
		}
		if(i==4)
			*(blocktype + i)= BBT_BLOCK;
		#endif

        /* 2.1 Handle Bad block, BBT block and GarbageTab block */
        if(mtd->block_isbad(mtd, i * mtd->erasesize) 
            || *(blocktype + i) == BBT_BLOCK ) //modified by kevinwu
        {            
			nftl_debug("Block %d is bad block\n", i);
			g_badblkcnt++;			
            continue;
        }
                
        if(*(blocktype + i) == GARBAGE_TAB_BLOCK)
        {
            /* ignore Bad block, BBT block and GarbageTab block */
			nftl_debug("Block %d is gt block\n", i);			
            continue;
        }      
        
        /* 2.2 Handle Free block */
        if(*(blocktype + i) == FREE_BLOCK) 
        {
            nftl_set_block_free(i);/* mark the free block */
			nftl_debug("Block %d is free\n", i);	
			
			//mtd_erase(mtd,i);
			//mtd_verify_clear(mtd,i);
								
            continue;
        }
        
        /* 2.2.1 Data block */
        if(*(blocktype + i) == DATA_BLOCK) 
        {          
			nftl_debug("Block %d is data block\n", i);			
            continue;
        }   
               
        /* 2.3 Handle Garbage block */
        if (nftl_is_gar_blk(i))
        {
            /* ignore Garbage block */
			nftl_debug("Block %d is garbage block\n", i);
            continue;
        }
        
        /* 2.4 Handle MapTab block */
        if(*(blocktype + i) == MAP_TAB_BLOCK)
        {		
            ret = mtd_read(mtd, i, 0, &retlen, NULL, pBS, 1);
			nftl_debug("Block %d is MapTab block\n", i);
            if(!ret && (retlen == mtd->oobsize))
            {
                if( blkinfo.mtspare.tabindx < mt_blkcnt )
                {
                    /*  valid MapTab block  */
                    mt_pbas[blkinfo.mtspare.tabindx] = i;
                    if(blkinfo.mtspare.tabindx == 0)
                    {
                        lastfree = blkinfo.mtspare.lastfree;
                    }
                }
                else
                {
                    nftl_error("the format of nftl is error\n");
                    nftl_error("The number of MapTab block is out of the span, "
                                "mt_blkcnt = %d , blkinfo.mtspare.tabindx = %d\n", 
                                mt_blkcnt,blkinfo.mtspare.tabindx);
					 if(blocktype)
						kfree(blocktype); 	
		             return 0x01;
                }
            }
			else
			{
				 if(blocktype)
					kfree(blocktype); 
				return ret;
			}
            continue;
        }
        
        /* 2.5 Handle MTLog block */
        if(*(blocktype + i) == MT_LOG_BLOCK) 
        {	
            mtl_blockinfo *ptab = &(mtl_blocktab[mtl_blkcnt]);
			nftl_debug("Block %d is MTLog block\n", i);
            ret = mtd_read(mtd, i, 0, &retlen, NULL, pBS, 1);
            if(!ret && (retlen == mtd->oobsize))
            {
                if(blkinfo.mtlofs.tabindx < mt_blkcnt) 
                {
                    /*  valid MTLog block  */
                    ptab->pba = i;
                    ptab->mtindx = blkinfo.mtlofs.tabindx;
                    ptab->valid = 0;
                    ptab->used = 0;
                }
                else
                {
                    nftl_error("the format of nftl is error\n");
                    nftl_error("The number of LUT log block is out of the span, "
                                    "mt_blkcnt = %d \n",mt_blkcnt);
				if(blocktype)
					kfree(blocktype); 	
                    return 0x01;
                }
            }
			else
			{
				if(blocktype)
					kfree(blocktype); 
				return ret;
			}
            
            /* scan the valid MTLog block, to construct the mtl_transtab*/
            while(1) 
            {
                if(ptab->used >= nr_pgsinblk)   //add by zoe
                {
                    break;
                }
                ret = mtd_read(mtd, i, ptab->used, &retlen, NULL, pBS, 1);
                if(!ret && (retlen == mtd->oobsize))
                {
                    if(blkinfo.mtlofs.tabindx != 0xff 
                        && ptab->used < nr_pgsinblk)
                    {
                        uint16_t offset = mtl_pgcnt - ptab->valid;
                    
                        for(j=0; j < ptab->valid; j++)
                        {
                            if(mtl_transtab[offset+j].pageofs 
                                                == blkinfo.mtlofs.pageofs)
                            {
                                /* find the repeated page on MTLog block */
                                break;
                            }
                        }
                        offset = offset + j;
                        mtl_transtab[offset].pageofs = blkinfo.mtlofs.pageofs;
                        mtl_transtab[offset].ppa = ptab->used;
						
                                
                        ptab->used++;
                        if (j == ptab->valid) 
                        {
                            /* not repeated page on MTLog block */
                            mtl_pgcnt++;
                            ptab->valid++;
                        }
                    }
                    else
                    {
                        break; /* it is free page */
                    }
                }
                else
                {
                    nftl_error("Read spare error: MTLog block = %d page = %d \n",
                            i,ptab->used);

					if(blocktype)
						kfree(blocktype); 
		                    return ret;
                }
            }
            mtl_blkcnt++;
            continue;
        }/* End of MT_LOG_BLOCK */
        
        /* 2.6 Handle DataLog block */
        if((*(blocktype + i) == DATA_LOG_BLOCK) && !(nftl_is_gar_blk(i))) 
        {
            /* read the last page of block to confirm if it is Data block */
			
            ret = mtd_read(mtd, i , (nr_pgsinblk - 1), &retlen, NULL, pBS, 1);
			if (ret || retlen != mtd->oobsize)
			{
				 if(blocktype)
				kfree(blocktype); 
				return ret;
			}
            if(blkinfo.blocktype != DATA_BLOCK) 
            {
                /* If the last page is marked as DATA_BLOCK,it is Data block;
                 * If not, it is a valid DataLog block.
                 * To construct the dl_blocktab and dl_transtab
                 */               
                dl_blockinfo *ptab = &(dl_blocktab[dl_blkcnt]);
				
				nftl_debug("Block %d is DataLog block\n", i);               
                ptab->pba = i;
                ptab->used = ptab->valid = 0;
        
                while(1) 
                {
					//blkinfo is current blk , current page info
					//ptab[] pointing to global dl_blocktab[current dl_blk] 
                    if(ptab->used >= nr_pgsinblk) //add by zoe                    
                        break;                    
                    ret = mtd_read(mtd, i, ptab->used, &retlen, NULL, pBS, 1);
                    if(!ret && (retlen == mtd->oobsize))
                    {
                        if(blkinfo.lpa != 0xffffffff && ptab->used < nr_pgsinblk) 
                        {
                            uint16_t offset = dl_pgcnt - ptab->valid;
                  
                            for(j=0; j < ptab->valid; j++)
                            {
                                if(ptab->valid==0 || dl_transtab[offset+j].lpa==blkinfo.lpa)
                                {
                                    /* find a repeated page on DataLog block */
                                    break;
                                }
                            }
                            offset += j;
                            dl_transtab[offset].lpa = blkinfo.lpa;
                            dl_transtab[offset].ppa = ptab->used;
                
                            ptab->used++;
                            if(ptab->used < 2)
                            {
                                /*update the LBA  on the dl_blocktab */
                                ptab->lba = blkinfo.lpa / nr_pgsinblk;
                            }
                            if (j == ptab->valid)
                            {
                                /* not repeated page on DataLog block */
                                dl_pgcnt++;
                                ptab->valid++;
                            }
                        }
                        else
                        {
                            break; /* it is free page */
                        }
                    }
                    else
                    {
                        nftl_error("Read spare error: DataLog block = %d, "
                                        "page = %d \n", i, ptab->used);
						 if(blocktype)
							kfree(blocktype); 
			                        return ret;
                    }
                }
            
                nftl_debug("scan the DataLog block: DATA_LOG_BLOCK = %d, "
                                    "dl_pgcnt = %d\n", ptab->pba, dl_pgcnt);
                dl_blkcnt++;
            }
            else
            	nftl_debug("Block %d is data block\n", i);	
            continue;
        }/* End of DATA_LOG_BLOCK */
		
    }/* End of for(i=0; i< s->nb_blocks ; i++) */
	    

	 if(blocktype)
		kfree(blocktype); //add by kevinwu

 	//g_storagesize=(nr_blks-g_badblkcnt-MINI_FREE_BLOCK -2*mt_blkcnt -10)*mtd->erasesize;//add by kevinwu
	//g_storagesize=(nr_blks - g_badblkcnt - nr_blks/4)*mtd->erasesize;
	g_storagesize=(nr_blks - g_badblkcnt - nr_blks/8 )*mtd->erasesize;
	
	
	nftl_debug("nftl g_storagesize = %d\n",g_storagesize);
	
	if(g_storagesize<500)
	{
		nftl_error("MTD partition is too small.\n");	
		return -1;
	}	
	
	/* 3. if MapTab blocks is not exsit, should be create them in the first time*/
    if(mt_pbas[0] == 0xffff || mt_pbas[mt_blkcnt-1] == 0xffff)
    {
        
        if (mtl_blkcnt == 0)
        {
    
            uint8_t databuff[NAND_MAX_PAGESIZE];
	     	uint8_t nr_mtpage;
            uint8_t mtindex;
            uint8_t mtpgofs;
			
            memset(databuff, 0xff, mtd->writesize);
			nr_mtpage = ((nr_blks - 1)* 2)/mtd->writesize + 1;


            blkinfo.blocktype =MAP_TAB_BLOCK;
            for(i = 0; i <  nr_mtpage; i++) 
			/******************/
			//i=0;
            {
                mtindex = i / nr_pgsinblk;
                mtpgofs = i % nr_pgsinblk;
                if(mtpgofs == 0)
                {
                    lastfree = 0;
                    mt_pbas[mtindex] = nftl_get_free_block();
                    if (mt_pbas[mtindex]== 0xffff)
                    {
                        nftl_error("no free block found\n");
						/*
					   if(databuff)
					   	kfree(databuff);*/
			   	
                        return -1;
                    }
                }
                blkinfo.mtspare.tabindx = mtindex;
                if(i == 0)
                {
                    blkinfo.mtspare.lastfree = lastfree;
                }
                nftl_debug("Write new nftl mt block at block=%d, page=%d \n",mt_pbas[mtindex],mtpgofs);
                ret = mtd_write(mtd, mt_pbas[mtindex], mtpgofs, &retlen,
				        databuff, pBS, 1);
                if(ret || (retlen != mtd->oobsize + mtd->writesize))
                {
                	nftl_error("NFTL_mount error ... \n");
                	
                    mt_pbas[mtindex] = nftl_write_error(mtd, 
                            mt_pbas[mtindex], mtpgofs, databuff, pBS, 1);
                    ret=0;	//assume we have solved the problem !!       
                }
            }
			/*
		  	 if(databuff)
		   	kfree(databuff);*/
			
        }
        else
        {
            nftl_error("MapTab block lost\n");
        }
    }
    nftl_msg("NFTL_mount exist: %d\n", ret);
		
	//readboot(s);
	
	debug_nftl_global_info();
	//nand_info(s);

    return ret;
}

