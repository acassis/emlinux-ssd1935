//*****************************************************************************
//
// lirc_edb9315.c - Driver that records pulse- and pause-lengths
//                  (space-lengths) between GPIO events on the consumer IR
//                  receiver on the EDB9315 board.
//
// Copyright (c) 2004 Cirrus Logic, Inc.
//
// Based on the lirc_serial driver.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//*****************************************************************************

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <linux/version.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/serial_reg.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/poll.h>

#include <asm/system.h>
#include <asm/segment.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/fcntl.h>

#include "drivers/lirc.h"

#define ENABLE_USBBLOCKING	0
#define ENABLE_LEDBLINKING	0

#ifdef CONFIG_ARCH_EP93XX
#define ENABLE_FIQ			0
#endif

#if (ENABLE_FIQ==1)
#include <linux/kernel_stat.h>
#include <asm/fiq.h>
#include <asm/arch/irqs.h>
#endif

#define LIRC_DRIVER_NAME "lirc_edb9315"

/* A long pulse code from a remote might take upto 300 bytes.  The
   daemon should read the bytes as soon as they are generated, so take
   the number of keys you think you can push before the daemon runs
   and multiply by 300.  The driver will warn you if you overrun this
   buffer.  If you have a slow computer or non-busmastering IDE disks,
   maybe you will need to increase this.  */

/* This MUST be a power of two!  It has to be larger than 1 as well. */

#define RBUF_LEN 512

static int major = LIRC_MAJOR;

static DECLARE_WAIT_QUEUE_HEAD(lirc_wait_in);

static spinlock_t lirc_lock = SPIN_LOCK_UNLOCKED;

static struct timeval lasttv = {0, 0};

static lirc_t rbuf[RBUF_LEN];
volatile static int rbh=0, rbt=0;

#ifdef CONFIG_ARCH_EP93XX
#if ENABLE_LEDBLINKING == 1
static void toggle_powerled()
{
	unsigned int pfdr = inl(GPIO_PFDR);
	if(pfdr & 0x0002)
		pfdr &= (~0x0002);
	else
		pfdr |= 0x0002;
	outl(pfdr, GPIO_PFDR);
}

static void turnon_powerled()
{
	outl(inl(GPIO_PFDR)&(~0x0002), GPIO_PFDR);
}

static turnoff_powerled()
{
	outl(inl(GPIO_PFDR)|0x0002, GPIO_PFDR);
}
#endif
#endif

#if (ENABLE_FIQ == 1)
extern void lirc_fiq_begin;
extern void lirc_fiq_end;
static unsigned char fiq_stack[1024];
static struct fiq_handler fiqh = { NULL, "GPIO_Ir", NULL, NULL };
void
lirc_fiq_handler(void)
{
	__asm__ __volatile__ ("\n\
lirc_fiq_begin:\n\
		stmdb	r13!, {r0-r7, lr}\n\
		ldr	r0, =irq_handler\n\
		mov	lr, pc\n\
		mov	pc, r0\n\
		ldmia	r13!, {r0-r7, lr}\n\
		subs	pc, lr, #4\n\
		.ltorg\n\
lirc_fiq_end:");
}

#endif


static void inline rbwrite(lirc_t l)
{
	unsigned int nrbt;

	nrbt=(rbt+1) & (RBUF_LEN-1);
	//printk("%s():rbh: %d rbt: %d\n", __PRETTY_FUNCTION__, rbh, rbt);
	if(nrbt==rbh)      /* no new signals will be accepted */
	{
//#ifdef DEBUG
		printk(KERN_WARNING  LIRC_DRIVER_NAME  ": Buffer overrun\n");
//#endif
		rbh = rbt = 0; // reset buffer
		return;
	}
	rbuf[rbt]=l;
	rbt=nrbt;
#if defined( CONFIG_ARCH_EP93XX ) && (ENABLE_USBBLOCKING == 1)
	unsigned int HCstatus = inl(HCINTERRUPTENABLE);
	outl(HCstatus|0x80000013, HCINTERRUPTENABLE);
	//printk("New key: 0x%08x\n", l);
#endif
#if ENABLE_LEDBLINKING == 1	
	turnon_powerled();
#endif		
}

static void inline frbwrite(lirc_t l)
{
	/* simple noise filter */
	static lirc_t pulse=0L,space=0L;
	static unsigned int ptr=0;
	
	if(ptr>0 && (l&PULSE_BIT))
	{
		pulse+=l&PULSE_MASK;
		//if(pulse>250)
		if(pulse>9000)
		{
			rbwrite(space);
			rbwrite(pulse|PULSE_BIT);
			//printk("%s(): not Ir noise, pulse: %d\n", __PRETTY_FUNCTION__,pulse);
			ptr=0;
			pulse=0;
		}
#if defined( CONFIG_ARCH_EP93XX ) && (ENABLE_USBBLOCKING == 1)
		unsigned int HCstatus = inl(HCINTERRUPTENABLE);
		outl(HCstatus|0x80000013, HCINTERRUPTENABLE);
#endif
		else
			printk("%s(): Ir noise\n", __PRETTY_FUNCTION__);
		return;
	}
	if(!(l&PULSE_BIT))
	{
		if(ptr==0)
		{
			if(l>20000)
			{
				space=l;
				ptr++;
#if defined( CONFIG_ARCH_EP93XX ) && (ENABLE_USBBLOCKING == 1)
				unsigned int HCstatus = inl(HCINTERRUPTENABLE);
				outl(HCstatus|0x80000013, HCINTERRUPTENABLE);
#endif
				return;
			}
		}
		else
		{
			if(l>20000)
			{
				space+=pulse;
				if(space>PULSE_MASK) space=PULSE_MASK;
				space+=l;
				if(space>PULSE_MASK) space=PULSE_MASK;
				pulse=0;
#if defined( CONFIG_ARCH_EP93XX ) && (ENABLE_USBBLOCKING==1)
				unsigned int HCstatus = inl(HCINTERRUPTENABLE);
				outl(HCstatus|0x80000013, HCINTERRUPTENABLE);
#endif
				return;
			}
			rbwrite(space);
			rbwrite(pulse|PULSE_BIT);
			ptr=0;
			pulse=0;
		}
	}
	rbwrite(l);
}

static void irq_handler(int i, void *blah, struct pt_regs *regs)
{
	struct timeval tv;
	int state;
	long deltv;
	lirc_t data;
	
#if (ENABLE_FIQ==1)
	/*
	 * Increment the count of interrupts.
	 */
	kstat.irqs[smp_processor_id()][IRQ_GPIO]++;
#endif
	
#if defined( CONFIG_ARCH_EP93XX	) && (ENABLE_USBBLOCKING==1)
	unsigned int HCstatus = inl(HCINTERRUPTENABLE);
	//printk("HCstatus: 0x%08x\n", HCstatus);
	HCstatus = inl(HCINTERRUPTDISABLE);
	outl(HCstatus | 0x80000000, HCINTERRUPTDISABLE);
#endif
#if ENABLE_LEDBLINKING == 1	
	turnoff_powerled();
#endif		
	/* get current time */
	do_gettimeofday(&tv);
#ifdef CONFIG_ARCH_EP9315
	state = inw(GPIO_PBDR) & 0x20 ? 1 : 0;
#elif defined (CONFIG_ARCH_EP9302)
	state = inw(GPIO_PADR) & 0x01 ? 1 : 0;
#endif
	deltv = tv.tv_sec - lasttv.tv_sec;
	if(deltv > 15)
	{
#ifdef DEBUG
		printk(KERN_WARNING LIRC_DRIVER_NAME
		       ": AIEEEE: %d %d %lx %lx %lx %lx\n",
		       dcd,1,
		       tv.tv_sec,lasttv.tv_sec,
		       tv.tv_usec,lasttv.tv_usec);
#endif
		data = PULSE_MASK; /* really long time */
	}
	else
	{
		data = (lirc_t)((deltv * 1000000) + tv.tv_usec -
				lasttv.tv_usec);
	};

	if((tv.tv_sec < lasttv.tv_sec) ||
	   ((tv.tv_sec == lasttv.tv_sec) &&
	    (tv.tv_usec < lasttv.tv_usec)))
	{
#ifdef DEBUG
		printk(KERN_WARNING LIRC_DRIVER_NAME
		       ": AIEEEE: your clock just jumped "
		       "backwards\n");
		printk(KERN_WARNING LIRC_DRIVER_NAME
		       ": %lx %lx %lx %lx\n",
		       tv.tv_sec, lasttv.tv_sec,
		       tv.tv_usec, lasttv.tv_usec);
#endif
		data = PULSE_MASK;
	}

	frbwrite(state ? (data | PULSE_BIT) : data);

	lasttv = tv;
#ifdef CONFIG_ARCH_EP9315
	outw(0x00, GPIO_BINTEN);
	outw(state ? 0x00 : 0x20, GPIO_BINTTYPE2);
	outw(0xff, GPIO_BEOI);
	outw(0x20, GPIO_BINTEN);
#elif defined (CONFIG_ARCH_EP9302)
	outw(0x00, GPIO_AINTEN);
	outw(state ? 0x00 : 0x01, GPIO_AINTTYPE2);
	outw(0xff, GPIO_AEOI);
	outw(0x01, GPIO_AINTEN);	
#endif
#if ENABLE_FIQ==1
	wake_up(&lirc_wait_in);
#else
	wake_up_interruptible(&lirc_wait_in);
#endif
}

static int lirc_open(struct inode *ino, struct file *filep)
{
	int result;
	unsigned long flags;
	struct pt_regs regs;
	
	spin_lock(&lirc_lock);
	if(MOD_IN_USE)
	{
		spin_unlock(&lirc_lock);
		return -EBUSY;
	}
	
	/* initialize timestamp */
	do_gettimeofday(&lasttv);
#if (ENABLE_FIQ==1)
	if(claim_fiq(&fiqh)){
		printk(KERN_ERR LIRC_DRIVER_NAME ": cannot claim FIQ\n");
		return -EBUSY;
	}
#endif
	result = request_irq(IRQ_GPIO, irq_handler, SA_INTERRUPT,
			     LIRC_DRIVER_NAME, NULL);
	switch(result)
	{
	case -EBUSY:
		printk(KERN_ERR LIRC_DRIVER_NAME ": IRQ %d busy\n", IRQ_GPIO);
		spin_unlock(&lirc_lock);
		return -EBUSY;
	case -EINVAL:
		printk(KERN_ERR LIRC_DRIVER_NAME
		       ": Bad irq number or handler\n");
		spin_unlock(&lirc_lock);
		return -EINVAL;
	default:
#ifdef DEBUG
		printk(KERN_INFO LIRC_DRIVER_NAME ": Interrupt %d obtained\n",
		       IRQ_GPIO);
#endif
		break;
	};

	/* finally enable interrupts. */
#if (ENABLE_FIQ == 1)
	regs.ARM_sp = (long)fiq_stack + sizeof(fiq_stack);
	set_fiq_regs(&regs);
	
	set_fiq_handler(&lirc_fiq_begin, &lirc_fiq_end-&lirc_fiq_begin);
	
	// Set interrupt as FIQ
	outl(inl(VIC1INTSELECT) | (1 << (IRQ_GPIO - 32)),
		 VIC1INTSELECT);
#endif
	save_flags(flags); cli();
	
#ifdef CONFIG_ARCH_EP9315
	outw(inw(GPIO_PBDDR) & ~0x20, GPIO_PBDDR);
	outw(0x00, GPIO_AINTEN);
	outw(0x00, GPIO_BINTEN);
	outw(0x00, GPIO_AINTTYPE1);
	outw(0x20, GPIO_BINTTYPE1);
	outw(0x00, GPIO_AINTTYPE2);
	outw((inw(GPIO_PBDR) & 0x20) ^ 0x20, GPIO_BINTTYPE2);
	outw(0xff, GPIO_AEOI);
	outw(0xff, GPIO_BEOI);
	outw(0x20, GPIO_BINTEN);
#elif defined (CONFIG_ARCH_EP9302)
	outw(inw(GPIO_PADDR) & ~0x01, GPIO_PADDR);
	outw(0x00, GPIO_AINTEN);
	outw(0x00, GPIO_BINTEN);
	outw(0x01, GPIO_AINTTYPE1);
	outw(0x00, GPIO_BINTTYPE1);
	outw((inw(GPIO_PADR) & 0x01) ^ 0x01, GPIO_AINTTYPE2);
	outw(0x00, GPIO_BINTTYPE2);
	outw(0xff, GPIO_AEOI);
	outw(0xff, GPIO_BEOI);
	outw(0x01, GPIO_AINTEN);	
#endif
	restore_flags(flags);

	/* Init read buffer pointers. */
	rbh = rbt = 0;
	
	MOD_INC_USE_COUNT;
	spin_unlock(&lirc_lock);
	return 0;
}

static int lirc_close(struct inode *node, struct file *file)
{	unsigned long flags;
	
	save_flags(flags);cli();
#if (ENABLE_FIQ==1)
	outl(inl(VIC1INTSELECT) & ~(1 << (IRQ_GPIO - 32)),
		 VIC1INTSELECT);
#endif
	outw(0x00, GPIO_AINTEN);
	outw(0x00, GPIO_BINTEN);

	restore_flags(flags);
	
	free_irq(IRQ_GPIO, NULL);
	
#if (ENABLE_FIQ==1)
	release_fiq(&fiqh);
#endif
	
#ifdef DEBUG
	printk(KERN_INFO  LIRC_DRIVER_NAME  ": freed IRQ %d\n", IRQ_GPIO);
#endif
	
	MOD_DEC_USE_COUNT;
	
	return 0;
}

static unsigned int lirc_poll(struct file *file, poll_table * wait)
{
	poll_wait(file, &lirc_wait_in, wait);
	if (rbh != rbt)
		return POLLIN | POLLRDNORM;
	return 0;
}

static ssize_t lirc_read(struct file *file, char *buf,
			 size_t count, loff_t * ppos)
{
	int n=0,retval=0;
	DECLARE_WAITQUEUE(wait,current);
	
	if(count%sizeof(lirc_t)) return(-EINVAL);
	
	add_wait_queue(&lirc_wait_in,&wait);
#if (ENABLE_FIQ!=1)
	current->state=TASK_INTERRUPTIBLE;
#endif
	while (n < count)
	{
		if (rbt != rbh) {
		/*if( ((rbt>rbh) && ((rbt-rbh)>71)) || 
			((rbt<rbh) && ((RBUF_LEN-(rbh-rbt))>71)) ) { */
			copy_to_user((void *) buf+n,
				     (void *) &rbuf[rbh],sizeof(lirc_t));
			rbh = (rbh + 1) & (RBUF_LEN - 1);
			n+=sizeof(lirc_t);
			//printk("%s():rbh: %d rbt: %d\n", __PRETTY_FUNCTION__, rbh, rbt);
		} else {
			if (file->f_flags & O_NONBLOCK) {
				retval = -EAGAIN;
				break;
			}
			if (signal_pending(current)) {
				retval = -ERESTARTSYS;
				break;
			}
			schedule();
#if (ENABLE_FIQ!=1)
			current->state=TASK_INTERRUPTIBLE;
#endif
		}
	}
	remove_wait_queue(&lirc_wait_in,&wait);
#if (ENABLE_FIQ!=1)
	current->state=TASK_RUNNING;
#endif
	return (n ? n : retval);
}

static ssize_t lirc_write(struct file *file, const char *buf,
			 size_t n, loff_t * ppos)
{
	return(-EBADF);
}

static int lirc_ioctl(struct inode *node,struct file *filep,unsigned int cmd,
		      unsigned long arg)
{
        int result;
	unsigned long value;
	
	switch(cmd)
	{
	case LIRC_GET_FEATURES:
		result=put_user(LIRC_CAN_REC_MODE2,
				(unsigned long *) arg);
		if(result) return(result);
		break;
		
	case LIRC_GET_REC_MODE:
		result=put_user(LIRC_MODE_MODE2, (unsigned long *) arg);
		if(result) return(result);
		break;
		
	case LIRC_SET_REC_MODE:
		result=get_user(value, (unsigned long *) arg);
		if(result) return(result);
		if(value!=LIRC_MODE_MODE2) return(-ENOSYS);
		break;
		
	default:
		return(-ENOIOCTLCMD);
	}
	return(0);
}

static struct file_operations lirc_fops =
{
	read:    lirc_read,
	write:   lirc_write,
	poll:    lirc_poll,
	ioctl:   lirc_ioctl,
	open:    lirc_open,
	release: lirc_close
};

#ifdef MODULE

MODULE_AUTHOR("Cirrus Logic, Inc.");
MODULE_DESCRIPTION("Infra-red receiver driver for the EDB9315.");
MODULE_LICENSE("GPL");

EXPORT_NO_SYMBOLS;

int init_module(void)
{
	if (register_chrdev(major, LIRC_DRIVER_NAME, &lirc_fops) < 0) {
		printk(KERN_ERR  LIRC_DRIVER_NAME
		       ": register_chrdev failed!\n");
		return -EIO;
	}
	return 0;
}

void cleanup_module(void)
{
	unregister_chrdev(major, LIRC_DRIVER_NAME);
}

#endif
