#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <asm/uaccess.h>
#include <linux/ioport.h> 

#include <asm/hardware.h>
#include <asm/system.h>
#include <asm/segment.h>
#include <asm/io.h>
#include "ssl_bl.h"
#include "os.h"
#include <asm/gpio.h>

#define DEVCOUNT	4

static struct cdev *cdev_p;
static dev_t dev;
static int openCnt;
static volatile uint32_t *pwm_map_reg_addr;

static int getmem (unsigned long addr, unsigned long len, char *name)
{
	if (check_mem_region (addr, len) == 0)
    {
		request_mem_region (addr, len, name);
        return 0;
    }

    return -EBUSY;
}

static int relmem (unsigned long addr, unsigned long len)
{
    release_mem_region (addr, len);
    return 0;
}


static int sslbl_ioctl(struct inode *inode, struct file *filp,
			     unsigned int cmd, unsigned int arg)
{	
	uint32_t ctr;
	unsigned int data;
	ctr = pwm_map_reg_addr[PWM_CTR];
	
	switch (cmd) 
	{
		case SSLBL_INTENSITY_SET:
		{
		    get_user(data,(int __user *)arg);
			data &= 0xff;
			if(data>0)
			{
				data = 256-data;
				gpio_set_value(BACKLIGHT_GPIO_PIN, 1);
			}
			else
			{
				gpio_set_value(BACKLIGHT_GPIO_PIN, 0);
				data = 0;
			}		
	        ctr = ctr & 0xff00ffff;
            ctr |= data<<16;
		    pwm_map_reg_addr[PWM_CTR] = ctr;

		    break;
		}
		case SSLBL_INTENSITY_GET:
		{
		    ctr = ctr & 0x00ff0001;
		    ctr = (256 - (ctr>>16)) & 0xff;
		    put_user(ctr,(int __user *)arg);
		    break;
		}		
		case SSLBL_FREQ_DIV_SET:
		{
			get_user(data,(int __user *)arg);
			data &= 0xff;
	        ctr = ctr & 0x00ffffff;
			ctr |= (1<<1);	// use APB clock
            ctr |= data<<24;
		    pwm_map_reg_addr[PWM_CTR] = ctr;			
			
			break;
		}
		default:
		    return -ENOTTY;	
	}
	return 0;
}


static int sslbl_open(struct inode *inode, struct file *filp)
{
	int minor;
	uint32_t cfg;
	uint32_t ctr;
	minor = MINOR(inode->i_rdev);
	if (minor < DEVCOUNT) 
	{
		if (openCnt > 0) 
		{
			return -EALREADY;
		} 
		else 
		{
			openCnt++;
			if ( openCnt == 1 )
			{
				cfg = pwm_map_reg_addr[PWM_CFG];
				//cfg |= PWM_CFG_RST | PWM_CFG_EN;
				cfg |= PWM_CFG_EN;
		
				pwm_map_reg_addr[PWM_CFG] = cfg;
				pwm_map_reg_addr[PWM_CFG] = 0x00000001;

				ctr = pwm_map_reg_addr[PWM_CTR];
				ctr |= PWM_CTR_HOST | PWM_CTR_EN;
				pwm_map_reg_addr[PWM_CTR] = ctr;
			}
			return 0;
		}
	}
	return -ENOENT;
}

static int sslbl_close(struct inode *inode, struct file *filp)
{
	int minor;

	minor = MINOR(inode->i_rdev);
	
	if (minor < DEVCOUNT) 
	{
		openCnt--;
		printk("backlight_clos,default the register\n");		
	}
	return 0;
}

static struct file_operations sslbl_fops = {
	.owner = THIS_MODULE,
	.open = sslbl_open,
	.release = sslbl_close,
	.ioctl = sslbl_ioctl,
};

static int __init sslbl_init(void)
{
	int ret;
	uint32_t ctr;
	unsigned int data;
	
	openCnt = 0;

	if (getmem (MAGUS_IO_PWM, 0x20, "backlight") < 0)
	{
			printk(KERN_ERR "getmem fail\n");
			relmem(MAGUS_IO_PWM,0x20);
            return -EBUSY;
	}
	pwm_map_reg_addr = ioremap_nocache(MAGUS_IO_PWM,0x20);
		
	
	if ((ret = alloc_chrdev_region(&dev, 0, DEVCOUNT, "backlight")) < 0) 
	{
		printk(KERN_ERR \
				"backlight: Couldn't alloc_chrdev_region, ret=%d\n",ret);
		return 1;
	}
	gpio_direction_output(BACKLIGHT_GPIO_PIN, 1);

	ctr = pwm_map_reg_addr[PWM_CTR];
	ctr |= (256 - 250)<<16;
	pwm_map_reg_addr[PWM_CTR] = ctr;
	
	cdev_p = cdev_alloc();
	cdev_p->ops = &sslbl_fops;
	ret = cdev_add(cdev_p, dev, DEVCOUNT);
	if (ret) 
	{
		printk(KERN_ERR \
				"backlight: Couldn't cdev_add, ret=%d\n", ret);
		return 1;
	}

	return 0;
}

static void __exit sslbl_exit(void)
{
	cdev_del(cdev_p);
	unregister_chrdev_region(dev, DEVCOUNT);
	iounmap(pwm_map_reg_addr);
	relmem(MAGUS_IO_PWM,0x20);
}

module_init(sslbl_init);
module_exit(sslbl_exit);

MODULE_LICENSE("GPL");
