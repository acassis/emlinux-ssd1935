/*======================================================================================
*
* piuk.c 	- PIU driver API
* Configuration:  SLOT 0 (COM0/REP0) is working as MASTER mode: ARM sends command to dsp and dsp sends reply to ARM
*		  SLOT 1 (COM1/REP1) is working as SLAVE mode: DSP sends command to ARM and ARM sends reply to DSP
*		  SLOT 2 (REP2) is working in slave mode: DSP sends command to ARM and would not expect any reply
*		  Only enable ARM's interrupts for the event when DSP sends command/reply to ARM (RxWRHIE)
* REMARKS:
* 1) COMx/REPx registers are both readable and writable for ARM and DSP.
* But from the INTMASK (interrupt enable register) point of view:
*		 COMx registers are read-only for DSP, write-only for ARM (when interrupt)
*		 REPx registers are write-only for DSP, read-only for ARM (when interrupt)
* 2) When call piu_wr32() and piu_rd32(), the user should be responsible to ensure {param[in] slot} should be
* less than PIU_MAX_SLOTS
*
* Version 1.1
* Author:	Shao Wei
* Date:		15 Oct 2007
*
========================================================================================*/

/*****************************************************************************************
clean 0712 ALSA SUPPORT 
flag : 	STANDALONE_AUDIO_SUPPORT
*****************************************************************************************/


#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/moduleparam.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/fcntl.h>
#include <asm/siginfo.h>
#include <asm/uaccess.h>
#include <asm/irq.h>

#ifndef PIU_SEMAPHORE_MODE_COMPLETE
#include <asm/semaphore.h>
#ifdef PIU_ENABLE_TIMEOUT
#include <linux/timer.h>
#endif
#else
#include <linux/completion.h>
#endif


#if IO_MAP == 3
#warning "using ebm interface"
#include "ebm_mem.h"
#include "io.h"
#else
#include <asm/io.h>
#endif

#if INTC == 1
#include "intck.h"
#define request_irq	intc_req
#define free_irq	intc_free
#define enable_irq	intc_enable
#define disable_irq	intc_disable
#endif

#include "piur.h"
#include "piu.h"
#include "piu_types.h"
#include "piuk.h"

/*
//clean 071218 for piu test
#include <linux/file.h>
struct file *file;
mm_segment_t oldfs;
int len;
#ifndef __user
#define __user
#endif
//endclean
*/

//clean 071219 
int piu_module_user;  // current user numbers of piu_module

uint32_t	addr = 0xd0132000;		/* base register address */
uint32_t	mem_addr = 0x90811000;		/* memory base address for shared information storage */
uint32_t	major = 0;
uint32_t	minor = 0;
int		irq = 0;

module_param(irq, uint, 0);
module_param(addr, uint, 0);
module_param(major, uint, 0);
module_param(minor, uint, 0);
/* Memory map shared by ARM and DSP */
module_param(mem_addr, uint, 0);

#define dbg	printk

#define DRV_NAME	"SSL_PIU"
#define PIU_MASTER_SLOT		0			// use COM0, REP0
#define PIU_SLAVE_SLOT		1			// use COM1, REP1
#define PIU_TX_MEM	PIU_INTERNAL_MEM
#define PIU_RX_MEM	PIU_INTERNAL_MEM
#define PIU_MSG_IN	(PIU_WR_RPY0_HINT << PIU_SLAVE_SLOT)
#define	PIU_ACK_IN	(PIU_WR_RPY0_HINT << PIU_MASTER_SLOT)

#ifdef PIU_ENABLE_TIMEOUT
#define ACK_TIMEOUT	HZ
#endif

#ifdef PIU_NONBLOCK
#warning "compile PIU nonblock driver (channel 2 used for dsp2arm nonblocking cmd)"
#define PIU_NONBLOCK_SLOT	2
#define PIU_NONBLOCK_MSG_IN	(PIU_WR_RPY0_HINT << PIU_NONBLOCK_SLOT)
#endif

struct sslpiu_dev
{
	piu_t	piu_hw;
	int	irq;
	
	void	*mem_addr;
	spinlock_t	lock;
	
#ifndef PIU_SEMAPHORE_MODE_COMPLETE	
	struct semaphore mutex;
#ifdef PIU_ENABLE_TIMEOUT
	struct timer_list timer;
#endif
#else	
	struct completion	complete;
#endif

	struct fasync_struct	*fasync_p;
	struct cdev	cdev;
	uint32_t	rtsignal;
#ifdef PIU_NONBLOCK
	/* to differentiate between blocking and non-blocking call */
	uint32_t	status;		// for piulib use -> 1:  intr from piu_rx (blocking call); 2: intr from piu_rx_s (nonblocking call)
	/* sometimes both blocking and non-blking request come at the same time, so this would indicate which one to serve
 first. For this case, it indicates that we would serve the blocking call first and then the non-blocking request */
	int8_t		block_first;
	int8_t		raise_block;
	int8_t		raise_nonblock;
#endif
};

typedef struct
{
	uint32_t	rx_len;
	uint32_t	rx_type;
	uint8_t		rx_array[PIU_RX_MEM];
	uint32_t	tx_len;
	uint32_t	tx_type;
	uint8_t		tx_array[PIU_TX_MEM];
#ifdef PIU_NONBLOCK
	uint32_t	len_s;
	uint32_t	type_s;
	uint8_t		array_s[PIU_INTERNAL_MEM];
#endif
}
piu_mem_t, *piu_mem_p;

typedef enum
{
	PIU_MSG_OUT	=	(1 << (PIU_WR_COM0_CINT_BIT + PIU_MASTER_SLOT)),
	PIU_ACK_OUT	=	(1 << (PIU_WR_COM0_CINT_BIT + PIU_SLAVE_SLOT))
}
piu_out_type_e;


static struct sslpiu_dev	piu_dev;
static uint32_t	last_cmd = 0;				// latest cmd sent by piu


/* APIs */

int piu_tx(uint32_t msg, piu_msg_p body)
{
	volatile piu_mem_p	mem;
#ifdef PIU_SEMAPHORE_MODE_COMPLETE
	int ret;
#endif
	
	if (body->len > PIU_TX_MEM)
	{
		dbg("tx err - msg body overflow. Pls also ensure the PIU share memory address is correct. \n");
		return -1;
	}

#ifndef PIU_SEMAPHORE_MODE_COMPLETE
	
	if (down_interruptible(&piu_dev.mutex))			// lock piu send channel for a single process
	{
		dbg("tx err - busy, not available\n");
		return -ERESTARTSYS;
	}
//dbg("mutex down\n");
#ifdef PIU_ENABLE_TIMEOUT
	piu_dev.timer.expires = jiffies + ACK_TIMEOUT;
	mod_timer(&piu_dev.timer, jiffies + ACK_TIMEOUT);
#endif

#endif	/* PIU_SEMAPHORE_MODE_COMPLETE */

	// copy from user-space to shared mem
	mem = (volatile piu_mem_p)piu_dev.mem_addr;
	
	// copy from user-space to shared mem
	mem->tx_len = body->len;
	mem->tx_type = body->type;
	memcpy(mem->tx_array, body->p, mem->tx_len);
	last_cmd = msg;		
	piu_wr32(&(piu_dev.piu_hw), PIU_MASTER_SLOT, 1, msg);

#ifdef PIU_SEMAPHORE_MODE_COMPLETE

#ifdef PIU_ENABLE_TIMEOUT
	ret = wait_for_completion_interruptible_timeout(&piu_dev.complete, ACK_TIMEOUT);
	if (ret==0)		// time out
	{
		//piu_clr_stat(PIU_MSG_OUT);
		dbg("tx warning - ack timeout, aborted!\n");
	}
	else if (ret == -ERESTARTSYS)
	{
		dbg("tx err - terminate by user while waiting for receiver's ack\n");
		return -ERESTARTSYS;
	}
#else
	ret = wait_for_completion_interruptible(&piu_dev.complete);
	if (ret)
	{
		dbg("tx err - terminate by user while waiting for receiver's ack\n");
		return -ERESTARTSYS;
	}
#endif

#endif	/* PIU_SEMAPHORE_MODE_COMPLETE */
	
	return 0;
}

EXPORT_SYMBOL(piu_tx);

//clean 071210 simple version of piu_tx, not in use
static int tx_nb_sem=1;
int piu_tx_nb(uint32_t msg, piu_msg_p body)
{
	volatile piu_mem_p	mem;
	
	if (body->len > PIU_TX_MEM)
	{
		dbg("tx err - msg body overflow. Pls also ensure the PIU share memory address is correct. \n");
		return -1;
	}

        if (tx_nb_sem != 1) return -1;
	tx_nb_sem = 0;
	// copy from user-space to shared mem
	mem = (volatile piu_mem_p)piu_dev.mem_addr;
	
	// copy from user-space to shared mem
	mem->tx_len = body->len;
	mem->tx_type = body->type;
	memcpy(mem->tx_array, body->p, mem->tx_len);
	last_cmd = msg;		
	piu_wr32(&(piu_dev.piu_hw), PIU_MASTER_SLOT, 1, msg);

	tx_nb_sem = 1;	
	return 0;
}
EXPORT_SYMBOL(piu_tx_nb);


static uint32_t  piu_rx_msg(void)
{
#ifdef PIU_NONBLOCK
	uint32_t	status = 0;
	
	if (piu_dev.block_first)
	{
		status = 1;
	}
	else if (piu_dev.raise_nonblock)
	{
		status = 2;
	}
	else if (piu_dev.raise_block)
	{
		status = 1;
	}
	
	if (status == 2)
	{
		status = piu_rd32(&(piu_dev.piu_hw), PIU_NONBLOCK_SLOT, 0);
	}
	else
	{
		status = piu_rd32(&(piu_dev.piu_hw), PIU_SLAVE_SLOT, 0);
	}
	return status;
#else
	return piu_rd32(&(piu_dev.piu_hw), PIU_SLAVE_SLOT, 0);
#endif
}


int piu_rx(uint32_t *msg, piu_msg_p body)
{
	volatile piu_mem_p	mem;
	
#ifdef PIU_NONBLOCK
	uint32_t	flags, status = 0;
	
	mem = (volatile piu_mem_p)piu_dev.mem_addr;
	
//printk("[%d %d %d] ", piu_dev.raise_block, piu_dev.raise_nonblock, piu_dev.block_first);
	spin_lock_irqsave(&piu_dev.lock, flags);
	if (piu_dev.block_first)	// raise block first
	{
		status = 1;
		piu_dev.status &= ~1;
		piu_dev.block_first = 0;
		piu_dev.raise_block = 0;
	}
	else if (piu_dev.raise_nonblock)	// else raise nonblock first
	{
		status = 2;
		piu_dev.status &= ~2;
		piu_dev.raise_nonblock = 0;
	}
	else if (piu_dev.raise_block)
	{
		status = 1;
		piu_dev.status &= ~1;
		piu_dev.raise_block = 0;
	}
		
	spin_unlock_irqrestore(&piu_dev.lock, flags);
//printk("s=%x\n", status);
	
	if (status == 1)
	{
		if (mem->rx_len > PIU_RX_MEM)
                {
                        dbg("rx err - msg body overflow\n");
			return -1;
                }
		body->len = mem->rx_len;
		body->type = mem->rx_type;
			memcpy(body->p, mem->rx_array, body->len);
		*msg = piu_rd32(&(piu_dev.piu_hw), PIU_SLAVE_SLOT, 0);
		// ack
		piu_wr32(&(piu_dev.piu_hw), PIU_SLAVE_SLOT, 1, *msg);
//up(&piu_dev.mutex);		// prevent a single process to call piu_tx before get ack
		piu_int_ena(&(piu_dev.piu_hw), PIU_MSG_IN);
	}
	else if (status == 2)
	{
		if (mem->len_s > PIU_RX_MEM)
		{
			dbg("rx err - msg body overflow\n");
			return -1;
		}
		body->len = mem->len_s;
		body->type = mem->type_s;
		memcpy(body->p, mem->array_s, body->len);
		*msg = piu_rd32(&(piu_dev.piu_hw), PIU_NONBLOCK_SLOT, 0);
		piu_int_ena(&(piu_dev.piu_hw), PIU_NONBLOCK_MSG_IN);
	}
#else
	
	mem = (volatile piu_mem_p)piu_dev.mem_addr;
	
	if (mem->rx_len > PIU_RX_MEM)
	{
		dbg("rx err - msg body overflow\n");
		return -1;
	}
	body->len = mem->rx_len;
	body->type = mem->rx_type;
	memcpy(body->p, mem->rx_array, body->len);
	*msg = piu_rd32(&(piu_dev.piu_hw), PIU_SLAVE_SLOT, 0);
	// ack
	piu_wr32(&(piu_dev.piu_hw), PIU_SLAVE_SLOT, 1, *msg);
	//piu_int_ena(&(piu_dev.piu_hw), PIU_MSG_IN);
#endif
	return 0;
}

EXPORT_SYMBOL(piu_rx);


/* register char device */

static uint32_t piu_stat(void)
{
	return piu_status(&(piu_dev.piu_hw));
}

static void piu_clr_stat(uint32_t stat)
{
	piu_clr_status(&(piu_dev.piu_hw), stat);
}

#ifndef PIU_SEMAPHORE_MODE_COMPLETE
#ifdef PIU_ENABLE_TIMEOUT
static void piu_release_tx(unsigned long arg)
{
	struct sslpiu_dev *t = (struct sslpiu_dev *)arg;	

	piu_clr_stat( PIU_MSG_OUT);	// in case DSP's piu is has disabled MSG_IN interrupt (if no callback is registered with DSP)
	piu_int_ena(&t->piu_hw, PIU_MSG_IN);	// in case none of process is register with the message id
	up(&t->mutex);			// prevent a single process to call piu_tx before get ack
//dbg("mutex up for timeout\n");
}
#endif
#endif

//clean 071210 alsa support
#define SSL_STANDALONE_AUDIO_SUPPORT		


#ifdef SSL_STANDALONE_AUDIO_SUPPORT
typedef void (*alsa_cb)(void*);
alsa_cb magus_pcm_callback_func;
alsa_cb magus_audio_ctl_cb;
void alsa_cb_register(alsa_cb cb)
{
	magus_pcm_callback_func = cb;
}
EXPORT_SYMBOL(alsa_cb_register);

void audio_ctl_cb_register(alsa_cb cb)
{
        magus_audio_ctl_cb=cb;
}
EXPORT_SYMBOL(audio_ctl_cb_register);

void alsa_piu_enable(int flag)
{
	int i;
	if (flag == 1)
	{
		//clean 071219 user num of this module
		if ( piu_module_user ==0) // currently no user, need init
		{
			piu_int_ena(&(piu_dev.piu_hw), PIU_ACK_IN);
			piu_int_ena(&(piu_dev.piu_hw), PIU_MSG_IN);
			#ifdef PIU_NONBLOCK
			piu_int_ena(&(piu_dev.piu_hw), PIU_NONBLOCK_MSG_IN);
			#endif
		}
		piu_module_user++;
		//printk("alsa open : piu module_user number: %d\n",piu_module_user);
		//endclean
	}
	else if (flag == 0)
	{
		//clean 071219 user num of this module
		piu_module_user--;
		//printk("alsa close : piu module_user number: %d\n",piu_module_user);
		if ( piu_module_user ==0) // no user any more
		{
			piu_int_dis(&(piu_dev.piu_hw), PIU_ACK_IN);
			piu_int_dis(&(piu_dev.piu_hw), PIU_MSG_IN);
			#ifdef PIU_NONBLOCK
			piu_int_dis(&(piu_dev.piu_hw), PIU_NONBLOCK_MSG_IN);
			#endif
		}
		//endclean
	}
}
EXPORT_SYMBOL(alsa_piu_enable);
#endif

/* module driver */

static void piu_handler(void *ctx, piu_evt_e e)
{
	struct sslpiu_dev	*t = (struct sslpiu_dev *)ctx;
	
	switch (e)
	{
		case PIU_ACK_IN:
		{
			uint32_t	ack;
			ack = piu_rd32(&(t->piu_hw), PIU_MASTER_SLOT, 0);
			if ( ack != last_cmd )
			{
				dbg("ack err - expecting %d, get %d", last_cmd, ack);
			}
#ifndef PIU_SEMAPHORE_MODE_COMPLETE
	
#ifdef PIU_ENABLE_TIMEOUT
			del_timer(&piu_dev.timer);
#endif
			up(&t->mutex);		// prevent a single process to call piu_tx before get ack
//dbg("mutex up\n");
	
#else
			complete(&t->complete);
#endif		
			break;
		}
		case PIU_MSG_IN:
		{

#ifdef SSL_STANDALONE_AUDIO_SUPPORT

{
	uint32_t msg;
	struct piu_protl	m;
	volatile piu_mem_p	mem;
	msg = piu_rd32(&(piu_dev.piu_hw), PIU_SLAVE_SLOT, 0);
	if (msg == 2)
	{
		mem = (volatile piu_mem_p)piu_dev.mem_addr;
		if (mem->rx_len > PIU_RX_MEM)
		{
			dbg("rx err - msg body overflow\n");
			return ;
		}
		memcpy(m.msg_body.p, mem->rx_array, mem->rx_len);
		// ack
		piu_wr32(&(piu_dev.piu_hw), PIU_SLAVE_SLOT, 1, msg);
		//piu_rx(&(m.msg), &(m.msg_body));
                if(magus_audio_ctl_cb != 0)
                magus_audio_ctl_cb(m.msg_body.p);
                else printk("null pointer magus_audio_ctl_cb\n");
		
		break;
	}
}
		
/*
{

        uint32_t msg;
        struct piu_protl        m;



        unsigned int tmp_status=piu_dev.status;
        int8_t   tmp_block_first= piu_dev.block_first;
        int8_t   tmp_raise_block= piu_dev.raise_block;
        int8_t   tmp_raise_nonblock=piu_dev.raise_nonblock;
        piu_dev.raise_block =1;
        piu_dev.raise_nonblock=0;
        piu_dev.block_first=0;
        piu_dev.status |= 1;
        piu_int_dis(&(t->piu_hw), PIU_MSG_IN);


//printk("receiving non-blocking msg\n");
        msg = piu_rx_msg();
        if (msg == 2)  //PIU AUDIO CONTROL
        {
//printk("receive alsa tdm msg\n");
                //printk("alsa response\n");
                piu_rx(&(m.msg), &(m.msg_body));
//printk("%u,%u,%u\n",(unsigned char)m.msg_body.p[0],(unsigned int)m.msg_body.p[4],(unsigned int)m.msg_body.p[8]);
                if(magus_audio_ctl_cb != 0)
                magus_audio_ctl_cb(m.msg_body.p);
                else printk("null pointer magus_audio_ctl_cb\n");

        piu_dev.status=tmp_status;
        piu_dev.block_first=tmp_block_first;
        piu_dev.raise_block=tmp_raise_block;
        piu_dev.raise_nonblock=tmp_raise_nonblock;

        break;
        }
//restore all status

        piu_dev.status=tmp_status;
        piu_dev.block_first=tmp_block_first;
        piu_dev.raise_block=tmp_raise_block;
        piu_dev.raise_nonblock=tmp_raise_nonblock;

        piu_int_ena(&(t->piu_hw), PIU_MSG_IN);

}
*/
#endif

			if (t->fasync_p)
			{
//printk("drv1-%d %d %d\n", piu_dev.raise_block, piu_dev.raise_nonblock, piu_dev.block_first);
				piu_int_dis(&(t->piu_hw), PIU_MSG_IN);

//down_interruptible(&t->mutex);			// lock piu send channel for a single process

#ifdef PIU_NONBLOCK
				piu_dev.status |= 1;
				piu_dev.raise_block = 1;
				if (piu_dev.raise_nonblock==0)		// msg-in first
				{
					piu_dev.block_first = 1;
				}
				else
				{
					piu_dev.block_first = 0;
				}
#endif
				kill_fasync(&t->fasync_p, t->rtsignal, POLL_IN);
			}
			break;
		}
#ifdef PIU_NONBLOCK
		case PIU_NONBLOCK_MSG_IN:
		{
/*
//clean 071218
set_fs(KERNEL_DS); 
len = file->f_op->write(file, (unsigned char __user *)"PIU_NONBLOCK_MSG_IN\n",20,&file->f_pos); 
set_fs(oldfs);
*/
//clean 071217 modify
#ifdef SSL_STANDALONE_AUDIO_SUPPORT		
{
	uint32_t msg;
	struct piu_protl	m;
	char was_disabled=0;
	//if (piu_mask_chk(&(t->piu_hw),PIU_NONBLOCK_MSG_IN))
	//	piu_int_dis(&(t->piu_hw), PIU_MSG_IN);
	//else was_disabled = 1;
	//piu_int_dis(&(t->piu_hw), PIU_NONBLOCK_MSG_IN);

	//piu_int_dis(&(t->piu_hw), PIU_NONBLOCK_MSG_IN);
	//printk("receiving non-blocking msg\n");
	msg = piu_rd32(&(piu_dev.piu_hw), PIU_NONBLOCK_SLOT, 0);
	if (msg == 5)
	{
	//printk("receive alsa tdm msg\n");

		volatile piu_mem_p	mem;
		mem = (volatile piu_mem_p)piu_dev.mem_addr;
		if (mem->len_s > PIU_RX_MEM)
		{
			dbg("rx err - msg body overflow\n");
			return ;
		}
		memcpy(m.msg_body.p, mem->array_s, mem->len_s);
		//piu_rx(&(m.msg), &(m.msg_body));
		if (magus_pcm_callback_func != 0)
		magus_pcm_callback_func(m.msg_body.p);
		else printk("null pointer magus_pcm_callback_func\n");
		
		//if (!was_disabled)	piu_int_ena(&(t->piu_hw), PIU_MSG_IN);
		//piu_int_ena(&(t->piu_hw), PIU_NONBLOCK_MSG_IN);

		break;
	}
//	else
//	{
//		if (!was_disabled)	piu_int_ena(&(t->piu_hw), PIU_MSG_IN);
//		piu_int_ena(&(t->piu_hw), PIU_NONBLOCK_MSG_IN);
//	}

}
#endif 

			if (t->fasync_p)
			{
				piu_int_dis(&(t->piu_hw), PIU_NONBLOCK_MSG_IN);
				piu_dev.status |= 2;
				piu_dev.raise_nonblock = 1;
				if (piu_dev.raise_block==0)		// nonblk_msg-in first
				{
					piu_dev.block_first = 0;
				}
				else
				{
					piu_dev.block_first = 1;
				}
//printk("drv2-%d %d %d\n", piu_dev.raise_block, piu_dev.raise_nonblock, piu_dev.block_first);
				kill_fasync(&t->fasync_p, t->rtsignal, POLL_IN);
			}
			break;
		}
#endif
	}

}

//static unsigned int irqcount = 0;
#if LINUX_VERSION >= 020620
static irqreturn_t sslpiu_irq(int irq, void *dev_id)
#else
static irqreturn_t sslpiu_irq(int irq, void *dev_id, struct pt_regs *regs)
#endif
{
	int	ret;
	unsigned long flags;
	struct sslpiu_dev	*t = dev_id;
//printk("\nirqcount=%u\n",irqcount++);
//printk("\nstart sslpiu_irq\n");
	spin_lock_irqsave(&t->lock, flags);
	ret = piu_isr(&t->piu_hw);
	spin_unlock_irqrestore(&t->lock, flags);
//printk("end sslpiu_irq\n");
	return ret ? IRQ_HANDLED : IRQ_NONE;
}


static int sslpiu_fasync(int fd, struct file *file, int mode)
{
	struct sslpiu_dev	*dev = file->private_data;
	return fasync_helper(fd, file, mode, &dev->fasync_p);
}

static int sslpiu_open(struct inode *inode, struct file *file)
{
	struct sslpiu_dev	*dev;
	dev = container_of(inode->i_cdev, struct sslpiu_dev, cdev);
	file->private_data = dev;
//clean 071219 user num of this module
//	piu_int_ena(&(dev->piu_hw), PIU_ACK_IN);
if ( piu_module_user ==0) //currently no user 
{
	piu_int_ena(&(dev->piu_hw), PIU_ACK_IN);
	piu_int_ena(&(dev->piu_hw), PIU_MSG_IN);
	#ifdef PIU_NONBLOCK
	piu_int_ena(&(dev->piu_hw), PIU_NONBLOCK_MSG_IN);
	#endif
}
else
{
	piu_int_ena(&(dev->piu_hw), PIU_NONBLOCK_MSG_IN);

}
piu_module_user++;
//printk("open : piu module_user number: %d\n",piu_module_user);
//endclean
	return 0;

}

static int sslpiu_release(struct inode *inode, struct file *file)
{
//clean 071219
	struct sslpiu_dev	*dev;
	dev = file->private_data ;

	sslpiu_fasync(-1, file, 0);
//clean 071219 user num of this module
piu_module_user--;
//printk("close : piu module_user number: %d\n",piu_module_user);
if ( piu_module_user ==0) // no user any more
{
	piu_int_dis(&(dev->piu_hw), PIU_ACK_IN);
	piu_int_dis(&(dev->piu_hw), PIU_MSG_IN);
	#ifdef PIU_NONBLOCK
	piu_int_dis(&(dev->piu_hw), PIU_NONBLOCK_MSG_IN);
	#endif
}
else
{
	piu_int_ena(&(dev->piu_hw), PIU_NONBLOCK_MSG_IN);

}
//endclean
	return 0;
}

static int sslpiu_ioctl(struct inode *inode, struct file *file,
	unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	
#if 0	
	if (_IOC_TYPE(cmd) != SSL_PIU_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > SSL_PIU_IOC_MAXNR) return -ENOTTY;
	
	if (_IOC_DIR(cmd) & _IOC_READ)
	{
		ret = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	}
	if (_IOC_DIR(cmd) & _IOC_WRITE)
	{
		ret = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	}
	if (ret)
	{
		return -EFAULT;
	}
#endif	

	switch(cmd) 
	{
		case PIU_IOCTL_TX:
		{
			struct piu_protl	m;
			
			if (copy_from_user(&m, (struct piu_protl *)arg, sizeof(struct piu_protl)))
			{
				return -EFAULT;
			}
			ret = piu_tx(m.msg, &(m.msg_body));
			if (ret)
			{
				return (ret == -ERESTARTSYS) ? -ERESTARTSYS : -EFAULT;
			}
			break;
		}
		case PIU_IOCTL_RX:
		{	
			struct piu_protl	m;

			ret = piu_rx(&(m.msg), &(m.msg_body));
			if (ret)
			{
				return -EFAULT;
			}
			if (copy_to_user((struct piu_protl *)arg, &m, sizeof(struct piu_protl)))
			{
				return -EFAULT;
			}
			break;
		}
		case PIU_IOCTL_RX_MSG:
		{
			uint32_t msg;
			msg = piu_rx_msg();
			if (copy_to_user((uint32_t *)arg, &msg, sizeof(uint32_t)))
			{
				return -EFAULT;
			}
			break;
		}
		case PIU_IOCTL_STATUS:
		{
			uint32_t	stat;
			
			stat = piu_stat();
			if (copy_to_user((uint32_t *)arg, &stat, sizeof(uint32_t)))
			{
				return -EFAULT;
			}
			break;
		}
		case PIU_IOCTL_CLR_STATUS:
		{
			uint32_t	stat;
			
			if (copy_from_user(&stat, (uint32_t *)arg, sizeof(uint32_t)))
			{
				return -EFAULT;
			}
			piu_clr_stat(stat);
			break;
		}
		case PIU_IOCTL_ENA:
		{
			
		   	 piu_dev.status = 0;
	                 piu_dev.raise_block = 0;
                   	 piu_dev.raise_nonblock= 0 ;
			 piu_dev.block_first = 0;
			piu_int_ena(&(piu_dev.piu_hw), PIU_MSG_IN);
			piu_int_ena(&(piu_dev.piu_hw), PIU_ACK_IN);

			break;
		}
		case PIU_IOCTL_DIS:
		{
			piu_int_dis(&(piu_dev.piu_hw), PIU_MSG_IN);
			piu_int_dis(&(piu_dev.piu_hw), PIU_ACK_IN);
			piu_dev.status = 0;
	                piu_dev.raise_block = 0;
        	        piu_dev.raise_nonblock= 0 ;
                	piu_dev.block_first = 0;

			break;
		}
		case PIU_IOCTL_IRQENA:
		{
			enable_irq(irq);
			break;
		}
		case PIU_IOCTL_IRQDIS:
		{
			disable_irq(irq);
			break;
		}
#ifdef PIU_NONBLOCK
		case PIU_IOCTL_RTSIGNO:
		{
			if (copy_from_user(&(piu_dev.rtsignal), (uint32_t *)arg, sizeof(uint32_t)))
			{
				return -EFAULT;
			}
			break;
		}
		case PIU_IOCTL_NONBLOCK_ENA:
			piu_dev.status = 0;
                    	piu_dev.raise_block = 0;
                    	piu_dev.raise_nonblock= 0 ;
                    	piu_dev.block_first = 0;
			piu_int_ena(&(piu_dev.piu_hw), PIU_NONBLOCK_MSG_IN);
			break;
		case PIU_IOCTL_NONBLOCK_DIS:
			piu_dev.status = 0;
                    	piu_dev.raise_block = 0;
                    	piu_dev.raise_nonblock= 0 ;
                    	piu_dev.block_first = 0;
			piu_int_dis(&(piu_dev.piu_hw), PIU_NONBLOCK_MSG_IN);
			break;
/*
		case PIU_IOCTL_NONBLOCK_RX:
		{
			// dsp send sth
			struct piu_protl	m;
			volatile piu_mem_p mem = (volatile piu_mem_p)piu_dev.mem_addr;
			
			m.msg_body.len = mem->len_s;
			m.msg_body.type = mem->type_s;
			memcpy(m.msg_body.p, mem->array_s, m.msg_body.len);
			m.msg = piu_rd32(&(piu_dev.piu_hw), PIU_NONBLOCK_SLOT, 0);
			if (copy_to_user((struct piu_protl *)arg, &m, sizeof(struct piu_protl)))
			{
				return -EFAULT;
			}

			break;
		}
		case PIU_IOCTL_NONBLOCK_RX_MSG:
		{
			uint32_t msg;
			
			msg = piu_rd32(&(piu_dev.piu_hw), PIU_NONBLOCK_SLOT, 0);
			if (copy_to_user((uint32_t *)arg, &msg, sizeof(uint32_t)))
			{
				return -EFAULT;
			}

			break;
		}
*/
#endif
		default:
		{
			return -ENOIOCTLCMD;
		}
	}

	return 0;
}


static struct file_operations sslpiu_fops = {
	.owner   = THIS_MODULE,	
	.ioctl	 = sslpiu_ioctl,
	.fasync	 = sslpiu_fasync,
	.open    = sslpiu_open,
	.release = sslpiu_release,
};


// initialization: void piu_srv_init(void *base)
static int __init sslpiu_init(void)
{	
	unsigned long	flags;
	void 		*start, *start_mem;
	dev_t		devno;
	int		ret = 0;


	if (!addr || !mem_addr)
	{
		printk(KERN_ERR "sslpiu: init err - addr or mem_addr\n");
		return -ENXIO;
	}

	memset(&piu_dev, 0, sizeof(struct sslpiu_dev));
	

#if IO_MAP == 1
	start = ioremap_nocache(addr, sizeof(piu_reg_t));
	if (!start)
	{
		printk(KERN_ERR "sslpiu: init err - ioremap\n");
		return -EBUSY;
	
	}
//	printk("PIU reg virt addr 0x%x, phy addr 0x%x\n", start, addr);
	start_mem = ioremap_nocache(mem_addr, sizeof(piu_mem_t));
	if (!start_mem)
	{
		printk(KERN_ERR "sslpiu: init err - ioremap\n");
		ret = -EBUSY;
		goto l_unmap1;
	}
//	printk("PIU memory virt addr 0x%x, phy addr 0x%x\n", start_mem, mem_addr);
#else
	start = (void *)addr;
	start_mem = (void *)mem_addr;
#endif
	piu_dev.mem_addr = start_mem;
	piu_dev.piu_hw.base = start;
	piu_dev.irq = irq;
//printk("piu ioremap\n");

	piu_dev.piu_hw.evt = piu_handler;
	piu_dev.piu_hw.ctx = (void *)&piu_dev;
	
	
#ifndef	PIU_SEMAPHORE_MODE_COMPLETE
	init_MUTEX(&piu_dev.mutex);
#else
	init_completion(&piu_dev.complete);
#endif	// PIU_SEMAPHORE_MODE_COMPLETE
//printk("init mutex\n");


	piu_init(&piu_dev.piu_hw);
//	piu_int_ena(&(piu_dev.piu_hw), PIU_ACK_IN);
//printk("piu_init\n");
	
	spin_lock_init(&piu_dev.lock);
	spin_lock_irqsave(&piu_dev.lock, flags);
	
	ret = request_irq(piu_dev.irq, sslpiu_irq, IRQF_DISABLED | IRQF_SHARED, DRV_NAME, &piu_dev);
	if (ret)
	{
		printk(KERN_ERR "init err - request_irq\n");
		goto l_firq1;
	}
	if (major)
	{
		devno = MKDEV(major, minor);
		register_chrdev_region(devno, 1, DRV_NAME);
	}
	else
	{
		ret = alloc_chrdev_region(&devno, minor, 1, DRV_NAME);
		major = MAJOR(devno);
	}
	if (ret<0)
	{
		printk(KERN_ERR "sslpiu: cannot get major number %d\n", major);
		goto l_firq;
	}
	cdev_init(&piu_dev.cdev, &sslpiu_fops);
	piu_dev.cdev.owner = THIS_MODULE;
	//piu_dev.cdev.ops= &sslpiu_fops;
	ret = cdev_add(&piu_dev.cdev, devno, 1);
	if (ret)
	{
		printk(KERN_NOTICE "Error %d adding piu\n", ret);
		goto l_unreg;
	}

#ifndef	PIU_SEMAPHORE_MODE_COMPLETE

#ifdef PIU_ENABLE_TIMEOUT
	init_timer(&piu_dev.timer);
	piu_dev.timer.data = (unsigned long)&piu_dev;
	piu_dev.timer.function = piu_release_tx;
#endif

#endif
	piu_dev.rtsignal = SIGRTMIN+3;		// SIGRTMIN = 32, but libc's SIGRTMIN=35
	spin_unlock_irqrestore(&piu_dev.lock, flags);

//clean 071218
//file = filp_open ("/tmp/piu.log", O_CREAT|O_WRONLY|O_APPEND,0600); 

//clean 071219 init user number
piu_module_user = 0;  // current user numbers of piu_module
//printk("init piu_module_user = %d\n",piu_module_user);

dbg("PIU driver loaded - mem @ 0x%08X\n", mem_addr);
	return 0;
	
l_unreg:
	unregister_chrdev_region(devno, 1);
l_firq:	
	free_irq(piu_dev.irq, &piu_dev);
l_firq1:
	spin_unlock_irqrestore(&piu_dev.lock, flags);
#if IO_MAP == 1
	iounmap(start_mem);
l_unmap1:
	iounmap(start);
#endif
	return ret;
}

static void __exit sslpiu_exit(void)
{
	unsigned long	flags;
	dev_t	devno;
	
	devno = MKDEV(major, minor);
	spin_lock_irqsave(&piu_dev.lock, flags);
	
#ifndef	PIU_SEMAPHORE_MODE_COMPLETE

#ifdef PIU_ENABLE_TIMEOUT
	del_timer(&piu_dev.timer);
#endif

#endif
	cdev_del(&piu_dev.cdev);
	unregister_chrdev_region(devno, 1);
	disable_irq(piu_dev.irq);
	free_irq(piu_dev.irq, &piu_dev);
	
#if IO_MAP == 1
	iounmap(piu_dev.piu_hw.base);
	iounmap(piu_dev.mem_addr);
#endif
	spin_unlock_irqrestore(&piu_dev.lock, flags);

//clean 071218
//filp_close(file,NULL);

dbg("PIU driver released\n");
}

module_init(sslpiu_init);
module_exit(sslpiu_exit);


MODULE_DESCRIPTION(DRV_NAME);
MODULE_AUTHOR("Shao Wei, Solomon Systech Ltd");
MODULE_LICENSE("GPL");

