/* Linux driver for NAND Flash Translation Layer      */
/* (c) 1999 Machine Vision Holdings, Inc.             */
/* Author: David Woodhouse <dwmw2@infradead.org>      */
/* $Id: nftlcore.c,v 1.20 2008/09/03 07:39:41 jackylam Exp $ */

/*
  The contents of this file are distributed under the GNU General
  Public License version 2. The author places no additional
  restrictions of any kind on it.
 */


#ifdef MAGUS_LINUX 
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/hdreg.h>
#include <linux/kmod.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nftl.h>
#include <linux/mtd/blktrans.h>


//#undef NFTL_DEBUG
#define NFTL_DEBUG
#define nftl_writedebug(fmt, arg ...)  //printk(KERN_ERR fmt, ##arg)

#ifdef NFTL_DEBUG
#define nftl_debug(fmt, arg ...)  printk(KERN_ERR "NFTLCORE - " fmt, ##arg)
#else
#define nftl_debug(fmt, arg ...) 
#endif

#define nftl_error(fmt, arg ...)  printk(KERN_ERR fmt, ##arg)
#define nftl_msg(fmt, arg ...)  printk( KERN_INFO fmt, ##arg)

static int partition=7;
module_param(partition, int, 4);

uint16_t l_writeblock = 0xffff;
uint8_t l_writepage = 0xff;
uint8_t write_buffer[NAND_MAX_PAGESIZE];

#else


#include "dummy.h"

struct mtd_info *mtd={0};
static int partition=3;


uint16_t l_writeblock = 0xffff;
uint8_t l_writepage = 0xff;
int l_writepart = 0xff;
uint8_t write_buffer[NAND_MAX_PAGESIZE];


#define nftl_error		printk
#define nftl_msg		printk
#define nftl_writedebug printk

#endif


/* maximum number of loops while examining next block, to have a
   chance to detect consistency problems (they should never happen
   because of the checks done in the mounting */



//It should depens on Nand Page size, should equal to  mtd->writesize
#ifdef MAGUS_LINUX 
#define SECTORSIZE		4096
//#define SECTORSIZE	512
#else
#define SECTORSIZE		512   
#endif

static uint8_t read_buffer[NAND_MAX_PAGESIZE];
static uint16_t l_readblock = 0xffff;
static uint8_t l_readpage 	= 0xff;
static int l_writepart = 0xff;

#define MT_READCACHE 

#ifdef MT_READCACHE
extern uint16_t mt_cacheblock;
extern uint8_t mt_cachepage;
#endif

extern uint16_t nr_blks;
extern uint8_t nr_pgsinblk;
extern uint32_t g_storagesize;

extern int nftl_read_logicpage(struct mtd_info *mtd,uint32_t lpa,uint8_t *databuff);
extern int nftl_write_logicpage(struct mtd_info *mtd, uint32_t lpa, uint8_t *databuff);
extern void nand_info(struct NFTLrecord *s);
extern  int nftl_reclaim_garbages(struct mtd_info *mtd);
extern void debug_nftl_global_info();
extern int NFTL_mount(struct NFTLrecord *s);
static int nftl_flush(struct mtd_blktrans_dev *dev);


void nftl_add_mtd(struct mtd_blktrans_ops *tr, struct mtd_info *mtd)
{	
		
	struct NFTLrecord *nftl;
	unsigned long temp;
	
	#if 1
	//stanley test multi nftl partition, it will try to add 2 parition
	//but it is not allowed!!, because there're many global varible in NFTL, they cannot be shared.
	if(mtd->index!=partition)
	{	
		return;
	}	
	#else
	
	if(mtd->index!=4) 		
		if(mtd->index!=partition) 	
			return;
				
	#endif

	if (mtd->type != MTD_NANDFLASH)
		return;
	if (!mtd->block_isbad) 
	{
		return;
	}
	
	nftl = kzalloc(sizeof(struct NFTLrecord), GFP_KERNEL);
	if (!nftl) {
		nftl_error(KERN_WARNING "NFTL: out of memory for data structures\n");
		return;
	}

	nftl->mbd.mtd = mtd;
	nftl->mbd.devnum = -1;

	nftl->mbd.tr = tr;
	
	
	nftl_msg("Chip size %d kbytes, erasesize %d bytes, writesize %d bytes, oobsize %d bytes\n",
	mtd->size, mtd->erasesize, mtd->writesize, mtd->oobsize);

    nftl->EraseSize = mtd->erasesize;
    nftl->nb_blocks = mtd->size / mtd->erasesize;
    if (NFTL_mount(nftl) )
	{
		nftl_error(KERN_WARNING "NFTL: could not mount device\n");
		kfree(nftl);
		return;
    }
    nftl->numvunits = (mtd->size - 1*1024*1024) / mtd->erasesize;
    nftl->mbd.size  = nftl->numvunits * (mtd->erasesize / SECTORSIZE);

	/* OK, it's a new one. Set up all the data structures. */

	/* Calculate geometry */
	nftl->cylinders = 1024;
	nftl->heads = 16;
	

	temp = nftl->cylinders * nftl->heads;
	nftl->sectors = nftl->mbd.size / temp;
	if (nftl->mbd.size % temp) {
		nftl->sectors++;
		temp = nftl->cylinders * nftl->sectors;
		nftl->heads = nftl->mbd.size / temp;

		if (nftl->mbd.size % temp) {
			nftl->heads++;
			temp = nftl->heads * nftl->sectors;
			nftl->cylinders = nftl->mbd.size / temp;
		}
	}

	#if 0
	if (nftl->mbd.size != nftl->heads * nftl->cylinders * nftl->sectors) {
		/*
		  Oh no we don't have
		   mbd.size == heads * cylinders * sectors
		*/
		printk(KERN_WARNING "NFTL: cannot calculate a geometry to "
		       "match size of 0x%lx.\n", nftl->mbd.size);
		printk(KERN_WARNING "NFTL: using C:%d H:%d S:%d "
			"(== 0x%lx sects)\n",
			nftl->cylinders, nftl->heads , nftl->sectors,
			(long)nftl->cylinders * (long)nftl->heads *
			(long)nftl->sectors );
	}
	#endif 
	
	#ifdef MAGUS_LINUX
	if (add_mtd_blktrans_dev(&nftl->mbd)) 
	{
		//		kfree(nftl->ReplUnitTable);
		//		kfree(nftl->EUNtable);
		kfree(nftl);
		return;
	}
	#endif
		
	nftl_debug(KERN_INFO "NFTL: Found new nftl%c\n", nftl->mbd.devnum + 'a');
	
}




static uint8_t nftl_read(struct mtd_info *mtd, uint16_t block, uint8_t page,uint8_t *buffer, uint16_t nr_page)
{	
    uint8_t ret = 0;
    uint32_t lpa;   
        
    lpa = block * nr_pgsinblk + page;  
    ret = nftl_read_logicpage(mtd, lpa,buffer);
    
    return ret;
}


uint8_t nftl_write(struct mtd_info *mtd, uint16_t block, uint8_t page,uint8_t *buffer, uint16_t nr_page)
{
    uint8_t ret = 0;
    uint32_t lpa;        

        
    lpa = block * nr_pgsinblk + page;
    ret = nftl_write_logicpage(mtd, lpa,buffer);
        		
    return ret;
}

int nftl_writesect(struct mtd_blktrans_dev *mbd, unsigned long sect, char *buffer)
{
   struct NFTLrecord *nftl = (void *)mbd;
   struct mtd_info *mtd = nftl->mbd.mtd;
   
    uint16_t block_num = (sect * SECTORSIZE) / mtd->erasesize;
    uint32_t block_offset = (sect * SECTORSIZE) % mtd->erasesize;
    uint8_t page_num = block_offset / mtd->writesize;
    uint16_t page_offset = block_offset % mtd->writesize;
    int16_t count = 0;
    uint16_t leftsize = SECTORSIZE;
    uint16_t writesize = 0;       
    
    /*if(l_writepart != mtd->index)
    {    	
    	l_writepart   = mtd->index;
    	nftl_flush(mbd);
    }    
	nftl_error("w1.%d",mtd->index);*/
		
    if(block_num == l_writeblock && page_num == l_writepage)	//hit cache
    {
    	//nftl_debug("nftl_writesect: hit cache\n");
    	//nftl_writedebug("nftl_writesect: hit cache\n");
        count = mtd->writesize - page_offset;
        count = SECTORSIZE > count ? count : SECTORSIZE;

		memcpy(write_buffer + page_offset, buffer, count);		//write max 512 to cache
        
        leftsize -= count;        
        if (leftsize == 0)
        {
            //nftl_debug("io_write: 1 return %d\n", count);	
            //leave without writing
            return 0;
        }        
        //Never go here in FAT32!!
        buffer += count;
        writesize += count;
		
		nftl_writedebug("nftl_writesect: hit cache, continue write\n");
        l_writepage = page_num;
        page_num ++;
        if (page_num >= nr_pgsinblk)
        {
            page_num=0;
            l_writeblock = block_num;
            block_num ++;
            if(block_num > nr_blks)
            {
                nftl_error("io_write: incomplete write return %d\n", count);
                return -1;
            }
        }
        page_offset = 0;
    }
	
	//if cache not hit, write old cache first, 
    if(l_writeblock != 0xffff && l_writepage != 0xff)
    {    	
    	//ret = nftl_write_logicpage(mtd, l_writeblock * nr_pgsinblk + l_writepage,buffer);                		
        if (nftl_write(mtd, l_writeblock, l_writepage, write_buffer, 1))
        {        
			return -1;
        }
        //is it necessary ??
        //memset(write_buffer, 0xff, mtd->writesize);
    }
	

	//not page allign (not 4K allign), only seldom happened in linux 
    if(page_offset != 0)
    {    	
    	nftl_writedebug("nftl_writesect: not page allign:%d\n",page_offset);
		//read current page to write buffer
        count = mtd->writesize - page_offset;
        count = leftsize > count ? count : leftsize;
        if (nftl_read(mtd, block_num, page_num, write_buffer, 1))
        {        	
			return -1;
        }

		//copy data to write buffer
        memcpy(write_buffer + page_offset, buffer, count);
        
        leftsize -= count;        
        if(leftsize == 0)		//if completed, write data next time
        {
            l_writepage = page_num;
            l_writeblock = block_num;
            return 0;
        } 
        //Never go here!!
        
        writesize += count;
        buffer += count;
		nftl_writedebug("nftl_writesect: not page allign, continue write \n");
		
		//write data if cache not enough
        if (nftl_write(mtd, block_num, page_num, write_buffer, 1))
        {        		
			return -1;
        }
        l_writepage = 0xff;
        l_writeblock = 0xffff;
        page_num ++;
        if(page_num >= nr_pgsinblk)
        {
            page_num = 0;
            block_num ++;
            if(block_num > nr_blks)
            {
                nftl_error("io_write: incomplete write return %d\n", count);
                return -1;
            }
        }
    }
	
	//it depens on SECTORSIZE macro
	//do writing if write size >= page size
    while(leftsize >= mtd->writesize)
    {
    	
    	nftl_writedebug("nftl_writesect: loop to write n\n");
        memcpy(write_buffer, buffer, mtd->writesize);
        buffer += mtd->writesize;
        count += mtd->writesize;
        if (nftl_write(mtd, block_num, page_num, write_buffer, 1))
        {        	
			return -1;
        }
        leftsize -= mtd->writesize;
        writesize += mtd->writesize;
        l_writepage = page_num;
        page_num ++;
        if(page_num >= nr_pgsinblk)
        {
            page_num = 0;
            l_writeblock = block_num;
            block_num ++;
            if(block_num > nr_blks)
            {
                nftl_error("io_write: incomplete write return %d\n", count);
                return -1;
            }
        }
    }

	//give up cache ??
	//coz we have completed writing a whole page??
	#if 0
    if(leftsize == 0)
    {
        l_writepage = 0xff;
        l_writeblock = 0xffff;
        return 0;
    }
    #endif
	
	//copy current page out
	//but the page may not exists in Map table, it will return all 0xff.
	//always die here, coz ECC error during read 
	//nftl_writedebug("copy current page to write buffer, \n");
    if (nftl_read(mtd, block_num, page_num, write_buffer, 1))
    {
    	nftl_error("nftl_writesect() error: caused by nftl_read() error, logical block=%d, logical page=%d\n",block_num,page_num);
		return -1;
    }
	
	//nftl_debug("nftl_writesect: normal page allign wirte\n");	
	//copy remain data to write buffer, and write it next time
    memcpy(write_buffer, buffer, leftsize);
    l_writepage = page_num;
    l_writeblock = block_num;

    return 0;
}




int nftl_readsect(struct mtd_blktrans_dev *mbd, unsigned long sect, char *buffer)
{

	struct NFTLrecord *nftl = (void *)mbd;
	struct mtd_info *mtd = nftl->mbd.mtd;

    uint16_t block_num = (sect * SECTORSIZE) / mtd->erasesize;
    uint32_t block_offset = (sect * SECTORSIZE) % mtd->erasesize;
    uint8_t page_num = block_offset / mtd->writesize;
    uint16_t page_offset = block_offset % mtd->writesize;
    int16_t count=0;
	
    /*if(l_writepart!=mtd->index)
    {    	
    	l_writepart   = mtd->index;
    	nftl_flush(mbd);
    }	
	nftl_error("r1.%d",mtd->index);
	*/
	
    if(page_offset != 0)	//not align page, read page from offset
    {
    	//nftl_debug("nftl_readsect: not page allign\n");
        count = mtd->writesize - page_offset; 				//count = data len in page
        count = SECTORSIZE > count ? count : SECTORSIZE;	//if count >512 count=512
        	
        if(block_num == l_writeblock && page_num == l_writepage)
        {
        	//if hit write cache
        	//read write cache data to read cache
            memcpy(read_buffer, write_buffer, mtd->writesize);		
        }
        else if(block_num != l_readblock || page_num != l_readpage)
        {   
        	//both read/write cahce missedm, read nand to read cache        	
            if(nftl_read(mtd, block_num, page_num, read_buffer, 1))	
				return -EIO;
        }
        
        l_readblock = block_num;
        l_readpage = page_num;
        //copy data to out buffer
        memcpy(buffer, read_buffer + page_offset, count);
        
        buffer += count;
        page_num ++;
        if(page_num >= nr_pgsinblk)
        {
            page_num = 0;
            block_num ++;
            if(block_num > nr_blks)
            {
                nftl_error("io_read: incomplete read return %d\n", count);
                return -1;
            }
        }
    }
    
    
    //case of page align, that's read 1st 512 only
    while(count < SECTORSIZE) 	//SECTORSIZE=512, always do 1 once
    {
    	    	
        if(block_num == l_writeblock && page_num == l_writepage)
        {
            memcpy(read_buffer, write_buffer, mtd->writesize);
        }
        else if(block_num != l_readblock || page_num != l_readpage)
        {
            if (nftl_read(mtd, block_num, page_num, read_buffer, 1))
				return -EIO;
        }
        
        l_readblock = block_num;
        l_readpage = page_num;
        if((SECTORSIZE - count) < mtd->writesize)		//it is always ture
        {
            //nftl_debug("io_read: partial read %d %d\n", SECTORSIZE-count, mtd->writesize);
            memcpy(buffer, read_buffer, SECTORSIZE - count);
            buffer += SECTORSIZE - count;
            count = SECTORSIZE;
        }
        else
        {
            memcpy(buffer, read_buffer, mtd->writesize);
            buffer += mtd->writesize;
            count += mtd->writesize;
        }
        page_num++;
        if(page_num >= nr_pgsinblk)
        {
            page_num=0;
            block_num++;
            if(block_num > nr_blks)
            {
                nftl_error("io_read: incomplete read return %d\n", count);
                return -1;
            }
        }
    }
	return 0;
}



int nftl_writesects(struct mtd_blktrans_dev *mbd, unsigned long sect, char *buffer,int count)
{
   	struct NFTLrecord *nftl = (void *)mbd;
	struct mtd_info *mtd = nftl->mbd.mtd;
	
	
	/*nftl_error("w4.%d",mtd->index);
    if(l_writepart!=mtd->index)
    {    	
    	l_writepart   = mtd->index;
    	nftl_flush(mbd);
    }*/	
	
    if(l_writeblock != 0xffff && l_writepage != 0xff)
    {
    	//nftl_error("NFTL write flush, write blk : %d page : %d \n", l_writeblock,l_writepage);
        nftl_write(mtd, l_writeblock, l_writepage, write_buffer, 1);
        l_writepage = 0xff;
        l_writeblock = 0xffff;        
    }	
	
	if(((count==8) && (mtd->writesize == 4096)) || 
	   ((count==4) && (mtd->writesize == 2048))) 
			return nftl_write_logicpage(mtd, sect,buffer);						
	else
		return 1;
}


int nftl_readsects(struct mtd_blktrans_dev *mbd, unsigned long sect, char *buffer,int count)
{
	struct NFTLrecord *nftl = (void *)mbd;
	struct mtd_info *mtd = nftl->mbd.mtd;    
		
	/*nftl_error("r4.%d",mtd->index);	
    if(l_writepart!=mtd->index)
    {    	
    	l_writepart   = mtd->index;
    	nftl_flush(mbd);
    }*/	
	
    if(l_writeblock != 0xffff && l_writepage != 0xff)
    {
    	//nftl_error("NFTL read flush, write blk : %d page : %d \n", l_writeblock,l_writepage);
        nftl_write(mtd, l_writeblock, l_writepage, write_buffer, 1);
        l_writepage = 0xff;
        l_writeblock = 0xffff;        
    }
    
    	
	if(((count==8) && (mtd->writesize == 4096)) || 
	   ((count==4) && (mtd->writesize == 2048))) 
		return nftl_read_logicpage(mtd, sect ,buffer);	
	else
		return 1;
}





static void nftl_remove_dev(struct mtd_blktrans_dev *dev)
{
	struct NFTLrecord *nftl = (void *)dev;
    struct mtd_info *mtd = nftl->mbd.mtd;
	
	nftl_msg("NFTL: remove_dev (i=%d)\n", dev->devnum);
	
    if(l_writeblock != 0xffff && l_writepage != 0xff)
    {
    	nftl_error("NFTL dev, write blk : %d page : %d \n", l_writeblock,l_writepage);
        nftl_write(mtd, l_writeblock, l_writepage, write_buffer, 1);
    }
    else    	    
    	nftl_error("NFTL flush unnecessary");
		
	/*****************************************************/
	
	//debug_nftl_global_info();
	//nftl_reclaim_garbages(mtd);//add by kevinwu
	//nand_info(nftl);
	
	/********************************************************/

	del_mtd_blktrans_dev(dev);
	//	kfree(nftl->ReplUnitTable);
	//	kfree(nftl->EUNtable);
	kfree(nftl);
}

static int nftl_getgeo(struct mtd_blktrans_dev *dev,  struct hd_geometry *geo)
{
	struct NFTLrecord *nftl = (void *)dev;
	geo->heads 		= nftl->heads;
	geo->sectors 	= nftl->sectors;
	geo->cylinders 	= nftl->cylinders;
	geo->start 		= g_storagesize;		
	return 0;
}

static int nftl_flush(struct mtd_blktrans_dev *dev)
{	
	
	struct NFTLrecord *nftl = (void *)dev;
	struct mtd_info *mtd = nftl->mbd.mtd;
	
	//mt_cacheblock=0xffff;
	//mt_cachepage=0xffff;
			
    if(l_writeblock != 0xffff && l_writepage != 0xff)
    {
    	//nftl_error("NFTL flush, write blk : %d page : %d \n", l_writeblock,l_writepage);
        nftl_write(mtd, l_writeblock, l_writepage, write_buffer, 1);
        l_writepage = 0xff;
        l_writeblock = 0xffff;        
    }
    //else    	    
    //	nftl_error("NFTL flush unnecessary\n");
	
}

/****************************************************************************
 *
 * Module stuff
 *
 ****************************************************************************/

#ifdef MAGUS_LINUX


static int nftl_open(struct mtd_blktrans_dev *mbd)
{
	
	nftl_error("nftl open\n");
}


static struct mtd_blktrans_ops nftl_tr = {
	.name			= "nftl",
	.major			= NFTL_MAJOR,
	.part_bits		= 0,
	.blksize 		= SECTORSIZE,
	.getgeo			= nftl_getgeo,
	.readsect		= nftl_readsect,	
	.writesect 		= nftl_writesect,				
	.readsects		= nftl_readsects,	
	.writesects		= nftl_writesects,			
	.add_mtd		= nftl_add_mtd,
	.remove_dev		= nftl_remove_dev,
	.owner			= THIS_MODULE,
	.flush			= nftl_flush,		
	//.open			= nftl_open,
	
};



static int __init init_nftl(void)
{
	nftl_tr.name = KBUILD_MODNAME ;
	nftl_msg("SSL NFTL driver:$Revision: 1.4\n");
   	if(partition<3)
   	{
   		nftl_error("the first 3 partion for kernel,please select other partition for nftl\n");
		return -1;
   	}
	nftl_tr.major += partition ;
	nftl_debug("nand partition %d\n",partition);

	return register_mtd_blktrans(&nftl_tr);
}

static void __exit cleanup_nftl(void)
{
	deregister_mtd_blktrans(&nftl_tr);
	nftl_msg(KERN_INFO "NFTL driver: cleanup\n");
}

module_init(init_nftl);
module_exit(cleanup_nftl);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kevinwu <kevinfkwu@solomon-systech.com>> et al.");
MODULE_DESCRIPTION("Support code for NAND Flash Translation Layer, used on SSL Nand Controller");
#endif

