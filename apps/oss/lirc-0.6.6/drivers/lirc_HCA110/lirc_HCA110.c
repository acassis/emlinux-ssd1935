//*****************************************************************************
//
// lirc_HCA110.c - Driver that records pulse- and pause-lengths
//                  (space-lengths) between GPIO events on the consumer IR
//                  receiver on the STR8131-based HCA110 board
//
// Copyright (c) 2007 U-MEDIA Communications, Inc.
//
// Based on the Cirrus Logic's EDB93xx series lirc driver.
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


#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <asm/uaccess.h>

#include <linux/ctype.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
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

#include <asm/arch/star_misc.h>
#include <asm/arch/star_gpio.h>
#include <asm/arch/star_timer.h>
#include <asm/arch/irqs.h>

#include <drivers/lirc.h>


#define LIRC_DRIVER_NAME "lirc_HCA110"
#define GPIO_IRIX_PIN	(0x01<<21)

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

volatile static int isheader=0;
volatile static int header_width=0;
volatile static int last_high = 0;

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
}

static void inline frbwrite(lirc_t l)
{
	/* simple noise filter */
	static lirc_t pulse=0L,space=0L;
	static unsigned int ptr=0;
	
	if(ptr>0 && (l&PULSE_BIT))
	{
		pulse+=l&PULSE_MASK;
		if(pulse>1000)
		{
			rbwrite(space);
			rbwrite(pulse|PULSE_BIT);
			//printk("%s(): not Ir noise, pulse: %d, space: %d\n", __PRETTY_FUNCTION__,pulse, space);
			ptr=0;
			pulse=0;
		}
		else{
			//printk("%s(): Ir noise, pulse: %d\n", __PRETTY_FUNCTION__, pulse);
		}
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
//#define DEBUG
//#define MIRROR_IR_SIGNAL 1
#define MIRROR_IR_SIGNAL_OUTPUT_BIT	(23)
static irqreturn_t irq_handler(int i, void *blah, struct pt_regs *regs)
{
	struct timeval tv;
	int state;
	long deltv;
	int data;
	
	if(GPIOB_INTERRUPT_RAW_STATUS_REG & GPIO_IRIX_PIN){
		GPIOB_INTERRUPT_CLEAR_REG = GPIO_IRIX_PIN;
		state = (GPIOB_DATA_INPUT_REG & GPIO_IRIX_PIN) != 0;
		// detect IRIX pin interrupts
		//printk("GPIO interrupt\n");
		/* get current time */
		//do_gettimeofday(&tv);
	
		TIMER3_CONTROL_REG = 0; // stop timer
		/*
		deltv = tv.tv_sec - lasttv.tv_sec;
		data = (int)((deltv * 1000000) + tv.tv_usec -
						lasttv.tv_usec);
		 */
		deltv = TIMER3_CONTROL_REG & 0x0000FFFF;
		// TIMER3's resolution is 10 us per tick (100KHz)
		data = TIMER3_COUNTER_LOW_REG*10;
		//printk("Timer3 H:0x%08x L: 0x%08x\n", TIMER3_CONTROL_REG, TIMER3_COUNTER_LOW_REG);
		// re-activate timer
		TIMER3_CONTROL_REG = 1<<TIMER3_RESET_BIT_INDEX;
		TIMER3_CONTROL_REG = 1<<TIMER3_ENABLE_BIT_INDEX;
		if(deltv > 0 || data >= 16000000)
		{
#ifdef DEBUG
			printk(KERN_WARNING LIRC_DRIVER_NAME
				   ": AIEEEE: %d %d %lx %lx %lx %lx\n",
				   deltv,1,
				   tv.tv_sec,lasttv.tv_sec,
				   tv.tv_usec,lasttv.tv_usec);
#endif
			data = PULSE_MASK; /* really long time */
		}
#if 1
		if(data!=PULSE_MASK){
			if(state){
				if(data<600)
					data = 600;
			}
			else{
				if(data<480)
					data = 520;
			}
		}
		if(data == PULSE_MASK ||( data>30000 && state == 0)){
			if(!isheader){
				isheader = 1; // header start
				header_width = 0;
			}
			frbwrite(state ? (data | PULSE_BIT) : data);
			wake_up_interruptible(&lirc_wait_in);
		}
		else if(data != PULSE_MASK && isheader && (data>0 &&data<10000)){
			header_width += data;
			if(state == 0){
				last_high = data;
			
				if(header_width>10000){
					if(last_high > 4500){
						if(header_width>12000)
							last_high = 4500;
						else
							last_high = 2200;
					}
					frbwrite((header_width-last_high| PULSE_BIT));
					frbwrite(last_high);
					wake_up_interruptible(&lirc_wait_in);
					//printk("header low: %d high: %d\n", header_width-last_high, last_high);
					isheader = 0;
					header_width = 0;
					last_high = 0;
				}
			}
		}
		else
#endif
		{
			/*if((tv.tv_sec < lasttv.tv_sec) ||
			((tv.tv_sec == lasttv.tv_sec) &&
			 (tv.tv_usec < lasttv.tv_usec))) */
			if(data<0)
			{
#ifdef DEBUG
				printk(KERN_WARNING LIRC_DRIVER_NAME
					   ": AIEEEE: your clock just jumped "
					   "backwards\n");
				printk(KERN_WARNING LIRC_DRIVER_NAME
					   ": %ld %ld %ld %ld\n",
					   tv.tv_sec, lasttv.tv_sec,
					   tv.tv_usec, lasttv.tv_usec);
#endif
				data = PULSE_MASK;
			}
			
			frbwrite(state ? (data | PULSE_BIT) : data);
			wake_up_interruptible(&lirc_wait_in);
		}
		lasttv.tv_sec = tv.tv_sec;
		lasttv.tv_usec = tv.tv_usec;
		
#if MIRROR_IR_SIGNAL == 1
		if(state)
			GPIOA_DATA_BIT_SET_REG = (0x01<<MIRROR_IR_SIGNAL_OUTPUT_BIT);
		else
			GPIOA_DATA_BIT_CLEAR_REG = (0x01<<MIRROR_IR_SIGNAL_OUTPUT_BIT);
#endif	 
	}
	
	return IRQ_HANDLED;
}

static int mod_count = 0;

static int lirc_open(struct inode *ino, struct file *filep)
{
	int result;
	unsigned long flags;
	struct pt_regs regs;
	
	spin_lock(&lirc_lock);
	if(mod_count > 1)
	{
		spin_unlock(&lirc_lock);
		return -EBUSY;
	}
	
	/* initialize timestamp */
	//do_gettimeofday(&lasttv);
	// clear timer 3
	TIMER3_CONTROL_REG = 1<<TIMER3_RESET_BIT_INDEX;
	
	result = request_irq(INTC_GPIO_EXTERNAL_INT_BIT_INDEX, irq_handler, SA_INTERRUPT,
			     LIRC_DRIVER_NAME, NULL);

	switch(result)
	{
	case -EBUSY:
		printk(KERN_ERR LIRC_DRIVER_NAME ": IRQ %d busy\n", INTC_GPIO_EXTERNAL_INT_BIT_INDEX);
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
		       INTC_GPIO_EXTERNAL_INT_BIT_INDEX);
#endif
		break;
	};

	/* finally enable interrupts. */
	save_flags(flags); cli();

	/* STR8131 GPIO config*/
	// disable GPIO interrupt
	GPIOB_INTERRUPT_ENABLE_REG = GPIOB_INTERRUPT_ENABLE_REG&(~GPIO_IRIX_PIN);
	// set GPIOB pin 21 to be GPIO
	MISC_GPIOB_PIN_ENABLE_REG = MISC_GPIOB_PIN_ENABLE_REG & (~GPIO_IRIX_PIN);
	// configure it to be input
	GPIOB_DIRECTION_REG = GPIOB_DIRECTION_REG&(~GPIO_IRIX_PIN);
	// edge trig
	GPIOB_INTERRUPT_TRIGGER_METHOD_REG = GPIOB_INTERRUPT_TRIGGER_METHOD_REG&(~GPIO_IRIX_PIN);
	// triggered by both edge
	GPIOB_INTERRUPT_TRIGGER_BOTH_EDGES_REG|=GPIO_IRIX_PIN;
	// enable bounce
	GPIOB_BOUNCE_ENABLE_REG|=GPIO_IRIX_PIN;
	// clear interrupt mask
	GPIOB_INTERRUPT_MASK_REG = GPIOB_INTERRUPT_MASK_REG&(~GPIO_IRIX_PIN);
	// clear interrupt status
	GPIOB_INTERRUPT_CLEAR_REG = GPIO_IRIX_PIN;
	// adjust interrupt priority
	INTC_INTERRUPT_PRIORITY_4_REG = 0; // highest priority
	// enable interrupt
	GPIOB_INTERRUPT_ENABLE_REG|=GPIO_IRIX_PIN;
#if MIRROR_IR_SIGNAL == 1
	MISC_GPIOA_PIN_ENABLE_REG &= ~(0x01<<MIRROR_IR_SIGNAL_OUTPUT_BIT);
	GPIOA_DIRECTION_REG |= (0x01<<MIRROR_IR_SIGNAL_OUTPUT_BIT);
#endif
	
	// enable timer 3
	TIMER3_CONTROL_REG = 1<<TIMER3_ENABLE_BIT_INDEX;
	
	restore_flags(flags);

	/* Init read buffer pointers. */
	rbh = rbt = 0;
	
	mod_count ++;
	spin_unlock(&lirc_lock);
	return 0;
}

static int lirc_close(struct inode *node, struct file *file)
{	unsigned long flags;
	
	save_flags(flags);cli();
	
	GPIOB_INTERRUPT_ENABLE_REG = GPIOB_INTERRUPT_ENABLE_REG&(~GPIO_IRIX_PIN);
	
	restore_flags(flags);
	
	free_irq(INTC_GPIO_EXTERNAL_INT_BIT_INDEX, NULL);
	
	
#ifdef DEBUG
	printk(KERN_INFO  LIRC_DRIVER_NAME  ": freed IRQ %d\n", INTC_GPIO_EXTERNAL_INT_BIT_INDEX);
#endif
	
	mod_count --;
	
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

	current->state=TASK_INTERRUPTIBLE;

	while (n < count)
	{
		if (rbt != rbh) {
		/*if( ((rbt>rbh) && ((rbt-rbh)>71)) || 
			((rbt<rbh) && ((RBUF_LEN-(rbh-rbt))>71)) ) { */
			copy_to_user((void *) buf+n,
				     (void *) &rbuf[rbh],sizeof(lirc_t));
			//rbh = (rbh + 1) & (RBUF_LEN - 1);
			rbh ++;
			rbh %= RBUF_LEN;
			n+=sizeof(lirc_t);
			/*printk("%s():rbuf[%d]: 0x%08x\n", __PRETTY_FUNCTION__, 
				   rbh, rbuf[rbh]); */
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

			current->state=TASK_INTERRUPTIBLE;

		}
	}
	remove_wait_queue(&lirc_wait_in,&wait);

	current->state=TASK_RUNNING;
	/*if(n)
		printk("Return %d bytes, rbh: %d rbt: %d\n", n, rbh, rbt);
	*/
	//printk("header width: %d\n", header_width);
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

MODULE_AUTHOR("U-MEDIA Communications, Inc.");
MODULE_DESCRIPTION("Infra-red receiver driver for the HCA110.");
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
