/**

    @file sms_dev_ctrl.c	
    
    @to swap between lcd and tv
    


	@notice Copyright (c), 2005-2007 Siano Mobile Silicon, Inc.
    
    @notice This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3 as
    published by the Free Software Foundation;
    
    Software distributed under the License is distributed on an "AS
    IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
    implied.
 */


#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>		/* printk() */
#include <linux/slab.h>			/* kmalloc() */
#include <linux/fs.h>			/* everything... */
#include <linux/errno.h>		/* error codes */
#include <linux/types.h>		/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>		/* O_ACCMODE */
#include <linux/cdev.h>
#include <asm/io.h>
#include <linux/device.h>
#include <asm/gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/delay.h>

#include <asm/system.h>			/* cli(), *_flags */
#include <asm/uaccess.h>		/* copy_*_user */
#include <linux/timer.h>

// IMPORTANT: when moving to production replace GPL with proprietary license
MODULE_LICENSE ( "Dual BSD/GPL" );


#define GET_GPIO_VAL 0
#define SET_GPIO_VAL 1
#define REQ_GPIO_ISR 2
#define REL_GPIO_ISR 3
#define SET_GPIO_INPUT 4
#define SET_GPIO_OUTPUT 5
#define SET_GPIO_MULTIPLEX_FUNC 6 
#define REQUEST_ISR 7
#define GET_ISR_INFO 8
/*
* Parameters which can be set at load time.
*/
//#define GPIO_DEBUG

#ifdef GPIO_DEBUG
#define dbg(format,args...) printk(KERN_ERR"gpio.c:%s() : "format,__FUNCTION__,##args);
#else
#define dbg(format, arg...)
#endif

#define err(format,args...) printk("gpio.c:%s() : "format,__FUNCTION__,##args);

#define DEVICE_NAME "gpio_dev"

typedef struct {
struct cdev* gpio_cdev;
struct fasync_struct *async_queue;
struct list_head gpio_isr_head;
//struct timer_list delay_timer;
} gpio_dev_st;


typedef struct{
struct list_head node;
int irq_type;
int gpio_no;
int irq_no;
int irq_flag; //1 irq belong to gpio_no just occur
struct delayed_work dwork;
}gpio_isr_st;
static int gpio_dev_major = 0;
static int gpio_dev_minor = 0;
static dev_t gpio_dev_no;
static gpio_dev_st* gpio_interface_dev;


module_param ( gpio_dev_major, int, S_IRUGO );



static int gpio_open ( struct inode *inode, struct file *filp );
static int gpio_fasync(int fd,struct file* filp,int mod);
static int gpio_release ( struct inode *inode, struct file *filp );
 void gpio_cleanup_module ( void );
 int gpio_init_module ( void );
static int gpio_setup_cdev(gpio_dev_st *dev);


#if 1
#define DELAY_JIFF (HZ>>1)
#if 0
void delayed_timer_handle(unsigned long arg)
{
	gpio_dev_st* dev =  gpio_interface_dev;
	gpio_isr_st* gpio_isr;
	int val,flag = 0;
	int irq = arg;
	dbg("enter...,irq:%d\n",irq);
	list_for_each_entry(gpio_isr,&dev->gpio_isr_head,node)
	{
		if(gpio_isr->irq_no == irq)
		{
			switch(gpio_isr->irq_type){
					case IRQ_TYPE_EDGE_FALLING:
							val = gpio_get_value(gpio_isr->gpio_no);
							if(val == 0)
								flag = 1;
						break;
						case IRQ_TYPE_EDGE_RISING:
							val = gpio_get_value(gpio_isr->gpio_no);
							if(val == 1)
								  flag = 1;
							break;
					default:
						err("not supported yet...\n");
						enable_irq(irq);
						return ;

			};
			if(flag == 1)
			{
				gpio_isr->irq_flag = 1;
				dbg("get interrupt from gpio %d\n",gpio_isr->gpio_no);
				if(dev->async_queue)
		            kill_fasync(&dev->async_queue,SIGIO,POLL_IN);
			}
			enable_irq(irq);
			return;
		}
	}
	enable_irq(irq);
	err("irq %d isn't registered\n",irq);
	return;
}
#endif
static  void delayed_work_handle(struct work_struct* work)
{
		int flag = 0,val;
			gpio_isr_st* gpio_isr = container_of(work,gpio_isr_st,dwork.work);
			gpio_dev_st* dev =  gpio_interface_dev;
			switch(gpio_isr->irq_type){
					case IRQ_TYPE_EDGE_FALLING:
							val = gpio_get_value(gpio_isr->gpio_no);
							if(val == 0)
								flag = 1;
						break;
						case IRQ_TYPE_EDGE_RISING:
							val = gpio_get_value(gpio_isr->gpio_no);
							if(val == 1)
								  flag = 1;
							break;
					default:
						err("not supported yet...\n");
						enable_irq(gpio_isr->irq_no);
						return ;

			};
			if(flag == 1)
			{
				gpio_isr->irq_flag = 1;
				dbg("get interrupt from gpio %d\n",gpio_isr->gpio_no);
				if(dev->async_queue)
		            kill_fasync(&dev->async_queue,SIGIO,POLL_IN);
			}
			enable_irq(gpio_isr->irq_no);
			return;
}
static irqreturn_t gpio_isr_handle(int irq, void* devid)
{
	gpio_dev_st* dev = (gpio_dev_st*)devid;
	gpio_isr_st* gpio_isr;
	disable_irq(irq);
//ev->cur_irq = irq;
	list_for_each_entry(gpio_isr,&dev->gpio_isr_head,node)
	{
		if(gpio_isr->irq_no == irq)
		{
			schedule_delayed_work(&gpio_isr->dwork,DELAY_JIFF);
			dbg("exit with delayed work,gpio_isr:0x%x\n",gpio_isr);
			return IRQ_HANDLED;
		}
	}
	enable_irq(irq);
	dbg("exit with irq not found...\n");
	return IRQ_HANDLED;
}
#endif
/*! 
	ioctl for the control channel's device instance

	\param[in]		inode: inode of the device instance
	\param[in]      filp: pointer to the file structure of the device instance
	\param[in]		cmd: command number
	\param[in]		arg: optional argument for the command. (may be a pointer or a long integer)
	\return		0 on success, negative error code otherwise.
*/

static int gpio_ioctl(struct inode *inode, struct file *filp,
				  unsigned int cmd, unsigned long arg)
{
	gpio_dev_st *dev = filp->private_data;
	int *args = (int*)arg;
	int gpio_no;
	int irq,ret,irq_type;
	unsigned long gpio_mode;
	gpio_isr_st* gpio_isr;
	switch(cmd){
		case GET_GPIO_VAL:
	//		dbg("get gpio value:port:%d,pins:%d\n",args[0],args[1]);
			args[1] = gpio_get_value(args[0]);
			break;
		case SET_GPIO_VAL:
	//		dbg("set gpio value:port:%d,pins:%d,val:%d\n",args[0],args[1],args[2]);
			gpio_set_value(args[0],args[1]);	
			break;
		case SET_GPIO_INPUT:
	//		dbg("set input direction: port: %d pins: %d\n",args[0],args[1]);
			gpio_direction_input(args[0]);
				break;
		case SET_GPIO_OUTPUT:
	//		dbg("set output direction: port:%d pins: %d val: %d\n",args[0],args[1],args[2]);
			gpio_direction_output(args[0],args[1]);
				break;
		case REQUEST_ISR:
		{
		gpio_isr_st* gpio_isr_new ;
		if(args[0] == 1)
		{

			err("args[]:%d %d %d\n",args[0],args[1],args[2]);
			irq = gpio_to_irq(args[1]);
			switch(args[2]){
				case 0:
					irq_type = IRQ_TYPE_EDGE_FALLING;
					break;
				case 1:
					irq_type = IRQ_TYPE_EDGE_RISING;
					break;
				default:
					err("not support irq type\n");
					return -EINVAL;
					break;
			};
			set_irq_type(irq,irq_type);
			ret = request_irq(irq,gpio_isr_handle,IRQF_SHARED,"gpio",dev);
			if(ret)
			{
				printk(KERN_ERR"gpio:request_irq err\n");
				return -1;
			}
			gpio_isr_new = (gpio_isr_st*)kmalloc(sizeof(gpio_isr_st),GFP_KERNEL);
			gpio_isr_new->gpio_no = args[1];
			gpio_isr_new->irq_no = irq;
			gpio_isr_new->irq_type = irq_type;
			gpio_isr_new->irq_flag = 0;
			INIT_DELAYED_WORK(&gpio_isr_new->dwork,delayed_work_handle);
			list_add_tail(&gpio_isr_new->node,&dev->gpio_isr_head);
		}else if(args[0] == 0)
		{
			gpio_no = args[1];
			list_for_each_entry(gpio_isr,&dev->gpio_isr_head,node){
				if(gpio_isr->gpio_no == gpio_no)
				{
					dbg("release irq,gpio_no:%d\n",gpio_no);	
					free_irq(gpio_isr->irq_no,dev);
					list_del(&gpio_isr->node);
					kfree(gpio_isr);
					return 0;
				}
			}
			err("release irq err\n");
			return -1;
		}
		}
			break;
		case GET_ISR_INFO:
			gpio_no = args[0];
			list_for_each_entry(gpio_isr,&dev->gpio_isr_head,node){
				if(gpio_isr->gpio_no == gpio_no)
				{
					if(gpio_isr->irq_flag == 1)
					{
							args[1] = 1;
							gpio_isr->irq_flag = 0;
					}
					else
							args[1] = 0;
					return 0;
				}
			}
			args[1] = 0;
			break;
		case SET_GPIO_MULTIPLEX_FUNC:
			gpio_mode = (args[0] | (args[1] << GPIO_PORT_SHIFT)) & GPIO_PF;
			gpio_fn(gpio_mode);
			break;
		default:
			printk(KERN_ERR"not support yet...\n");
			return -EINVAL;

	};

	return 0;

}

/**************************************************************/
/*
* Open and close
*/
/*! opens a device instance (channel)

	\param[in]	inode: pointer to the filesystem inode of the device instance
	\param[in] 	filp: pointer to the file structure of the device instance
	\return		0 on success.
*/
static int gpio_open ( struct inode *inode, struct file *filp )
{
	dbg("enter...\n");
	filp->private_data = gpio_interface_dev;

	return 0;					/* success */
}
static int gpio_fasync(int fd,struct file* filp,int mod)
{
	gpio_dev_st *dev = filp->private_data;
	return fasync_helper(fd,filp,mod,&dev->async_queue);
}


/*! closes a device instance (channel)

	\param[in]		inode: pointer to the filesystem inode of the device instance
	\param[in] 	filp: pointer to the file structure of the device instance
	
	\return		0 on success.
*/
static int gpio_release ( struct inode *inode, struct file *filp )
{
	gpio_dev_st *dev;
	dev = filp->private_data;
	fasync_helper(-1, filp, 0, &dev->async_queue);
	return 0;					//nothing to release
}


//! this structure stores pointer to device operations for registration with the kernel
static struct file_operations gpio_fops = 
{
	.owner = THIS_MODULE,
	.open = gpio_open,
	.release = gpio_release,
	.fasync = gpio_fasync,
	.ioctl = gpio_ioctl,
};

/*! module termination function

\return		void
*/


void gpio_cleanup_module ( void )
{
			
	gpio_isr_st* gpio_isr;	
	cdev_del(gpio_interface_dev->gpio_cdev);
	unregister_chrdev_region(gpio_dev_no,1);
	dbg("1\n");
	list_for_each_entry(gpio_isr,&gpio_interface_dev->gpio_isr_head,node){
			dbg("release irq,gpio_no:%d\n",gpio_isr->gpio_no);	
			free_irq(gpio_isr->irq_no,gpio_interface_dev);
			list_del(&gpio_isr->node);
			kfree(gpio_isr);
	}
	dbg("2\n");	
	kfree(gpio_interface_dev);
	dbg("3\n");
	gpio_interface_dev = NULL;
	dbg("exit\n");
}



/*! module initialization

\return	error status
*/

int gpio_init_module ( void )
{
	int ret = -1;
	gpio_interface_dev = (gpio_dev_st*)kzalloc(sizeof(gpio_dev_st),GFP_KERNEL);
	if(gpio_interface_dev == NULL)
	{
		printk(KERN_ERR"not enough memory...\n");
		return -ENOMEM;
	}
	if(gpio_dev_major)
	{
		gpio_dev_no = MKDEV(gpio_dev_major,gpio_dev_minor); 
		ret = register_chrdev_region(gpio_dev_no,1,DEVICE_NAME);
	}
	else
	{
		ret= alloc_chrdev_region(&gpio_dev_no,gpio_dev_minor,1,DEVICE_NAME);
		dbg("after alloc_chrdev_region\n");
		gpio_dev_major = MAJOR(gpio_dev_no);
	}
	if(ret < 0)
	{
		printk(KERN_ERR"can't register the major num!\n");
		return -1;
	}
	gpio_setup_cdev(gpio_interface_dev);
	INIT_LIST_HEAD(&gpio_interface_dev->gpio_isr_head);	
//	init_timer(&gpio_interface_dev->delay_timer);
//	gpio_interface_dev->delay_timer.function = delay_timer_handle;
	dbg("exit...\n");
	return 0;

}

static int gpio_setup_cdev(gpio_dev_st *dev)
{
	int ret;
	dev->gpio_cdev = cdev_alloc();
	if(NULL == dev->gpio_cdev)
	{
		dbg("can't request the memory!\n");
		return -ENOMEM;
	}
	
	cdev_init(dev->gpio_cdev,&gpio_fops);
	dev->gpio_cdev->owner = THIS_MODULE;
	dev->gpio_cdev->ops = &gpio_fops;
	ret = cdev_add(dev->gpio_cdev,gpio_dev_no,1);
	if(ret < 0)
	{
		printk(KERN_ERR"can't add cdev.2\n");
		return ret;
	}
}
/*! inform the kernel linker which are the module's entry point */
module_init ( gpio_init_module );
module_exit ( gpio_cleanup_module );






