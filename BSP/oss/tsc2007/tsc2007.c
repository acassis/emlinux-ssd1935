/*
 *  linux/drivers/i2c/chips/tsc2007.c
 *
 *  Copyright (C) 2005 Bill Gatliff <bgat at billgatliff.com>
 *  Changes for 2.6.20 kernel by Nicholas Chen <nchen at cs.umd.edu>
 *  Modified for SSD1933 by eric Wang <eric.wang at u-media.com.tw>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  Driver for TI's TSC2007 I2C Touch Screen Controller
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/bcd.h>
#include <linux/list.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */

#include <asm/gpio.h>

static int stop = 0;
static int ls_value;
static unsigned short normal_i2c[] = {0x40, I2C_CLIENT_END };
struct proc_dir_entry *ls_entry;
extern struct proc_dir_entry proc_root;

static ssize_t ls_read(struct file * file, const char * buf,size_t count, loff_t *ppos)
{
	if (copy_to_user(buf, &ls_value, 4)) {
		return -EFAULT;
	}
	return 4;
}
struct file_operations proc_light_sensor = 
{
        read:		ls_read,
};
#define MAGUS_GPIO_TSC2007	GPIO_NUM(3, 11)

I2C_CLIENT_INSMOD_1(tsc2007);

#define DRIVER_NAME "tsc2007"

enum tsc2007_pd {
  PD_POWERDOWN = 0, /* penirq */
  PD_POWERUP = 1, /* no penirq */
};

enum tsc2007_m {
  M_12BIT = 0,
  M_8BIT = 1
};

enum tsc2007_cmd {
  MEAS_TEMP0 = 0,
  MEAS_AUX = 2,
  MEAS_TEMP1 = 4,
  ACTIVATE_NX_DRIVERS = 8,
  ACTIVATE_NY_DRIVERS = 9,
  ACTIVATE_YNX_DRIVERS = 10,
  SETUP 	= 11,
  MEAS_XPOS = 12,
  MEAS_YPOS = 13,
  MEAS_Z1POS = 14,
  MEAS_Z2POS = 15
};

#define TSC2007_CMD(cn,pdn,m) (((cn) << 4) | ((pdn) << 2) | ((m) << 1))

#define ADC_MAX ((1 << 12) - 1)

struct tsc2007_data {
  struct i2c_client client;
  struct device_driver driver;
  struct input_dev *idev;
  struct semaphore sem;
  struct task_struct *tstask;
  struct completion tstask_completion;
  enum tsc2007_pd pd;
  enum tsc2007_m m;
  int penirq;

  int temp0;
  int temp1;
};

static int tsc2007_i2c_detect (struct i2c_adapter *adapter, int address, int kind);


static int tsc2007_read(struct tsc2007_data *data,
                         enum tsc2007_cmd cmd,
                         enum tsc2007_pd pd,
                         int *val)
{
  char c;
  char d[2];
  int ret;
 
  //c = TSC2007_CMD(cmd, pd, data->m);
  c = TSC2007_CMD(cmd, 0, data->m);

  ret = i2c_master_send(&data->client, &c, 1);
  if (ret <= 0) goto err;

  udelay(10);

  ret = i2c_master_recv(&data->client, d, data->m == M_12BIT ? 2 : 1);
  if (ret <= 0) goto err;

  if (val)
    {
      *val = d[0];
      *val <<= 4;
      if (data->m == M_12BIT)
        *val += (d[1] >> 4);
    }
  udelay(10);
#if defined(CONFIG_I2C_DEBUG_CHIP)
  printk(KERN_ERR "%s: val[%x] = %d\n",
         __FUNCTION__, cmd, (((int)d[0]) << 8) + d[1]);
#endif

  return 0;
 err:
  if (!ret)
  {
  	ret = -ENODEV;
	//printk("tsc2007 error!!\n");
  }
  return ret;
}

static inline int tsc2007_read_temp0 (struct tsc2007_data *d, enum tsc2007_pd pd, int *t)
{
  return tsc2007_read(d, MEAS_TEMP0, pd, t);
}

static inline int tsc2007_read_temp1 (struct tsc2007_data *d, enum tsc2007_pd pd, int *t)
{
  return tsc2007_read(d, MEAS_TEMP1, pd, t);
}

static inline int tsc2007_read_xpos (struct tsc2007_data *d, enum tsc2007_pd pd, int *x)
{
  return tsc2007_read(d, MEAS_XPOS, pd, x);
}

static inline int tsc2007_read_ypos (struct tsc2007_data *d, enum tsc2007_pd pd, int *y)
{
  return tsc2007_read(d, MEAS_YPOS, pd, y);
}

static inline int tsc2007_read_pressure (struct tsc2007_data *d, enum tsc2007_pd pd, int *p)
{
  	return tsc2007_read(d, MEAS_Z1POS, pd, p);
}
static inline int tsc2007_read_pressure1(struct tsc2007_data *d, enum tsc2007_pd pd, int *p)
{
  	return tsc2007_read(d, MEAS_Z2POS, pd, p);
}

static inline int tsc2007_read_aux (struct tsc2007_data *d, enum tsc2007_pd pd, int *t)
{
  return tsc2007_read(d, MEAS_AUX, pd, t);
}

static inline int tsc2007_powerdown (struct tsc2007_data *d)
{
  /* we don't have a distinct powerdown command,
     so do a benign read with the PD bits cleared */
  return tsc2007_read(d, MEAS_AUX, PD_POWERDOWN, 0);
}

static inline int tsc2007_powerup (struct tsc2007_data *d)
{
  /* we don't have a distinct powerdown command,
     so do a benign read with the PD bits cleared */
  return tsc2007_read(d, MEAS_AUX, PD_POWERUP, 0);
}

void tsc2007_init_client (struct i2c_client *client)
{
  struct tsc2007_data *data = i2c_get_clientdata(client);

  data->pd = PD_POWERUP;
  data->m = M_8BIT;
  return;
}

#define GPIO_TEST_PIN	GPIO_NUM(3, 5)
#define AVG_TIMES	5
static int tsc2007ts_thread (void *v)
{
  struct tsc2007_data *d = v;
  struct task_struct *tsk = current;
  int pendown=0;
  
  d->tstask = tsk;

  daemonize(DRIVER_NAME "tsd");
//  allow_signal(SIGKILL);

  complete(&d->tstask_completion);

  gpio_direction_output(GPIO_TEST_PIN, 0);
  gpio_direction_input(MAGUS_GPIO_TSC2007);
//  while (!signal_pending(tsk)){
  while (!stop){
      unsigned int i, x, y, p, p1, temp_x[AVG_TIMES], temp_y[AVG_TIMES];
     
      d->pd = PD_POWERUP;      
	  x = y = 0;
	  //p = gpio_get_value(MAGUS_GPIO_TSC2007);
		
	
	  tsc2007_read_aux(d, PD_POWERUP, &ls_value);	 
   	  tsc2007_read_pressure(d, PD_POWERUP, &p);	 
   	  //tsc2007_read_pressure1(d, PD_POWERUP, &p1);
   	  if(p > 32) 
   	  {
		for(i=0;i<AVG_TIMES;i++)
		{
	      	tsc2007_read_xpos(d, PD_POWERUP, &temp_y[i]);
    	  	tsc2007_read_ypos(d, PD_POWERUP, &temp_x[i]);			
		}		
					//sort the result
		for(i=0;i<AVG_TIMES;i++)
		{
			for(x=i;x<AVG_TIMES;x++)
			{
				if(temp_x[i]>temp_x[x])
				{
					y = temp_x[i];
					temp_x[i] = temp_x[x];
					temp_x[x] = y;
				}
				if(temp_y[i]>temp_y[x])
				{
					y = temp_y[i];
					temp_y[i] = temp_y[x];
					temp_y[x] = y;
				}
			}
			//printk("X: %d, y:%d\n",temp_x[i], temp_y[i]);
		}
		if(temp_x[AVG_TIMES-1] - temp_x[0] > 64 ||
		   temp_y[AVG_TIMES-1] - temp_y[0] > 64)
		{
			continue;
		}  
		x = y = 0;
		for(i=1;i<AVG_TIMES-1;i++)
		{
			x += temp_x[i];	y += temp_y[i];
		}
		x /= (AVG_TIMES-2); 	y /= (AVG_TIMES-2);		 	  
		tsc2007_read_pressure(d, PD_POWERUP, &p);	 		
      }
	  
	  if (p > 32) 
	  {
		pendown = 1;

		input_report_abs(d->idev, ABS_X, x);
		input_report_abs(d->idev, ABS_Y, 4096-y);
		input_report_abs(d->idev, ABS_PRESSURE, p);
		input_report_key(d->idev, BTN_TOUCH, 1);
		input_sync(d->idev);
		//gpio_set_value(GPIO_TEST_PIN, 1);
	  } else if (pendown == 1) 
	  {
		//printk("ts key up\n");
		pendown = 0;
		input_report_key(d->idev, BTN_TOUCH, 0);
		input_report_abs(d->idev, ABS_PRESSURE, 0);
		input_sync(d->idev);
		//gpio_set_value(GPIO_TEST_PIN, 0);
	  }
	  schedule_timeout_interruptible(msecs_to_jiffies(10));

		//printk("ts: %d %d %d, pd:%d\n", p, p1, i, pendown);	  	  
    }
     
  d->tstask = NULL;
  complete_and_exit(&d->tstask_completion, 0);
}

static int tsc2007_idev_open (struct input_dev *idev)
{
  struct tsc2007_data *d = idev->private;
  int ret = 0;

  if (down_interruptible(&d->sem))
    return -EINTR;

  if (d->tstask)
    panic(DRIVER_NAME "tsd already running (!). abort.");

  stop = 0;
  ret = kernel_thread(tsc2007ts_thread, d, CLONE_KERNEL);
  if (ret >= 0)
  {
      wait_for_completion(&d->tstask_completion);
      ret = 0;
   }

  up(&d->sem);
  return ret;
}

static void tsc2007_idev_close (struct input_dev *idev)
{
	struct tsc2007_data *d = idev->private;
	down_interruptible(&d->sem);
	if(d->tstask){
//      send_sig(SIGKILL, d->tstask, 1);
		stop = 1;
		wait_for_completion(&d->tstask_completion);
	}

	up(&d->sem);
	return;
}

static int tsc2007_driver_register (struct tsc2007_data *data)
{
  struct input_dev *idev;

  init_MUTEX(&data->sem);
  init_completion(&data->tstask_completion);
  
  idev = input_allocate_device();
  data->idev = idev;

  idev->private = data;
  idev->name = DRIVER_NAME;
  idev->evbit[0] = BIT(EV_ABS);
  idev->open = tsc2007_idev_open;
  idev->close = tsc2007_idev_close;
  idev->absbit[(long)(ABS_X)] = BIT(ABS_X);
  idev->absbit[(long)(ABS_Y)] = BIT(ABS_Y);
  idev->absbit[(long)(ABS_PRESSURE)] = BIT(ABS_PRESSURE);
  input_set_abs_params(idev, ABS_X, 0, ADC_MAX, 0, 0);
  input_set_abs_params(idev, ABS_Y, 0, ADC_MAX, 0, 0);
  input_set_abs_params(idev, ABS_PRESSURE, 0, 0, 0, 0);

  input_register_device(idev);
  
  ls_entry = create_proc_entry("lightsensor", S_IRUSR, &proc_root);
  ls_entry->proc_fops = &proc_light_sensor;  

  return 0;
}

static int tsc2007_i2c_attach_adapter(struct i2c_adapter *adapter)
{
  printk(KERN_INFO "tsc2007 i2c touch screen controller\n");
  return i2c_probe(adapter, &addr_data, tsc2007_i2c_detect);
}

static int tsc2007_i2c_detach_client(struct i2c_client *client)
{
  int err;  
  struct tsc2007_data *d = i2c_get_clientdata(client);

  //free_irq(d->penirq,d);
  input_unregister_device(d->idev);

  if ((err = i2c_detach_client(client))) {
    dev_err(&client->dev, "Client deregistration failed, "
            "client not detached.\n");
    return err;
  }

  return 0;
}

static struct i2c_driver tsc2007_driver = {
  .driver = {
	  .owner        = THIS_MODULE,
	  .name         = DRIVER_NAME,
   },
  .attach_adapter    = tsc2007_i2c_attach_adapter,
  .detach_client    = tsc2007_i2c_detach_client,
  //  .command              = tsc2007_command,
};

static int tsc2007_i2c_detect (struct i2c_adapter *adapter, int address, int kind)
{
  struct i2c_client *new_client;
  struct tsc2007_data *data;

  int err = 0;
  const char *name = "";

  if (!i2c_check_functionality(adapter,
                               I2C_FUNC_SMBUS_BYTE_DATA
                               | I2C_FUNC_I2C
                               | I2C_FUNC_SMBUS_WORD_DATA))
    goto exit;

  data = kcalloc(1, sizeof(*data), GFP_KERNEL);
  if (!data) {
    err = -ENOMEM;
    goto exit;
  }

  new_client = &data->client;
  i2c_set_clientdata(new_client, data);
  new_client->addr = 0x4b;//address;
  new_client->adapter = adapter;
  new_client->driver = &tsc2007_driver;
  new_client->flags = 0;

  /* TODO: I'm pretty sure I'm not dealing with kind correctly */
  if (kind == 0 /* identification */ || kind < 0 /* detection */)
      kind = tsc2007;

  if (kind == tsc2007)
    name = DRIVER_NAME;

  /* try a command, see if we get an ack;
     if we do, assume it's our device */
  //printk(KERN_INFO "%s: probing address 0x%x\n", __FUNCTION__, address);

  err = tsc2007_powerdown(data);

  if (err >= 0)
    {
      strlcpy(new_client->name, name, I2C_NAME_SIZE);
      err = i2c_attach_client(new_client);
      if (err) goto exit_free;

      tsc2007_init_client(new_client);

      err = tsc2007_driver_register(data);
      if (err < 0) goto exit_free;
     
      printk(KERN_INFO "%s: device address 0x%x attached.\n",
             __FUNCTION__, new_client->addr);
      return 0;
    }
  /* failure to detect when everything else is ok isn't an error */
  else err = 0;

 exit_free:
  kfree(new_client);
 exit:
  return err;
}

static int __init tsc2007_init(void)
{
  return i2c_add_driver(&tsc2007_driver);
}

static void __exit tsc2007_exit(void)
{
  i2c_del_driver(&tsc2007_driver);
  remove_proc_entry("lightsensor",&proc_root);
}


MODULE_AUTHOR("Bill Gatliff <bgat at billgatliff.com>");
MODULE_DESCRIPTION("tsc2007 Touch Screen Controller driver");
MODULE_LICENSE("GPL");

module_init(tsc2007_init);
module_exit(tsc2007_exit);
