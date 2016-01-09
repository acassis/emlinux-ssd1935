#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include "gpio_ctrl.h"

#define dbg(fmt,args...) printf("gpio %s() "fmt,__FUNCTION__,##args)

#define err(fmt,args...) printf("err: gpio %s() "fmt,__FUNCTION__,##args)

void signal_action();

#define GET_GPIO_VAL 0
#define SET_GPIO_VAL 1
#define REL_GPIO_ISR 3
#define SET_GPIO_INPUT 4
#define SET_GPIO_OUTPUT 5
#define SET_GPIO_MULTIPLEX_FUNC 6
#define REQUEST_ISR 7
#define GET_ISR_INFO 8


#define GPIO_PINS				5
#define GPIO_NUM(port, pin)	(((port) << GPIO_PINS) | (pin))
//pthread_mutex_t dev_mutex;
static int open_num = 0;
static int gpio_fd;

#define MAX_IRQ_NUM 20
typedef struct{
int gpio_no;
cb_func_ptr cb_func;
}gpio_irq_st;

typedef struct{
gpio_irq_st irq_array[MAX_IRQ_NUM];
int top_index;
}irq_management_st;

static irq_management_st irq_management;

void init_gpio_lib()
{
	int oflags;
	if(open_num == 0)
	{
		/*
		system("rmmod gpio;rm -rf /dev/gpio_dev");
		if(system("insmod /usr/local/lib/module/gpio.ko") == -1)
		{
			printf("insmod err\n");
			return;
		} */
		if(system("major=`cat /proc/devices | grep gpio_dev | cut -c1-3`;mknod /dev/gpio_dev c $major 0") == -1)
		{
			printf("mknod err\n");
			return;
		}

				gpio_fd = open("/dev/gpio_dev",O_RDWR|S_IRUSR|S_IWUSR);
				signal(SIGIO,signal_action);
  				fcntl(gpio_fd,F_SETOWN,getpid());
  				oflags = fcntl(gpio_fd,F_GETFL);
  				fcntl(gpio_fd,F_SETFL,oflags|FASYNC);
				irq_management.top_index = -1;
		}
		open_num++;
		printf("init_gpio(): gpio_open_num:%d,gpio_fd:%d\n",open_num,gpio_fd);
}
void set_gpio_input(int port,int pins)
{
#if 0
	int args[2];
	args[0] = port;
	args[1] = pins;
#endif
//	printf("port:%d pins:%d,gpio_fd:%d\n",args[0],args[1],gpio_fd);
	int gpio_no = GPIO_NUM(port,pins);
	ioctl(gpio_fd,SET_GPIO_INPUT,&gpio_no);
	
}
void set_gpio_output(int port,int pins, int val)
{
#if 0
	int args[3];
	args[0] = port;
	args[1] = pins;
	args[2] = val;
#endif

	int gpio_no = GPIO_NUM(port,pins);
//	printf("port:%d pins:%d val:%d,gpio_fd:%d\n",args[0],args[1],args[2],gpio_fd);
	ioctl(gpio_fd,SET_GPIO_OUTPUT,&gpio_no);
}
void set_gpio_multiplexed(int port,int pins)
{
#if 0
	int args[2];
	args[0] = port;
	args[1] = pins;
#endif

	int gpio_no = GPIO_NUM(port,pins);
	ioctl(gpio_fd,SET_GPIO_MULTIPLEX_FUNC,&gpio_no);
}
void exit_gpio_lib()
{
	//pthread_mutex_lock(&dev_mutex);
	int i;
	int args[2];
	open_num--;
	int num = irq_management.top_index;
	if(open_num == 0)
	{
		for(i=0; i<= num; i++)
		{
				args[0] = 0; // will release irq
				args[1] = irq_management.irq_array[i].gpio_no;
				printf("unregister isr callback,gpio_no:%d\n",args[1]);
				sleep(1);
				if(ioctl(gpio_fd,REQUEST_ISR,args) < 0)
				{
					err("ioctl err\n");
					return;
				}
				irq_management.irq_array[i].cb_func = NULL;
				irq_management.top_index--;
		}
		close(gpio_fd);
		//system("rmmod gpio && rm -rf /dev/gpio");
		gpio_fd = -1;
	}
	printf("gpio exit.. open_num:%d\n",open_num);
//	printf("exit_gpio: gpio_open_num:%d,gpio_fd:%d\n",open_num,gpio_fd);
//	pthread_mutex_unlock(&dev_mutex);
}
void set_gpio_value(int port,int pins,int val)
{
	int args[2];
#if 0
	args[0] = port;
	args[1] = pins;
	args[2] = val;
#endif
	args[0] = GPIO_NUM(port,pins);
	args[1] = val;
//	printf("port:%d pins:%d val:%d,gpio_fd:%d\n",args[0],args[1],args[2],gpio_fd);
	ioctl(gpio_fd,SET_GPIO_VAL,args);
}
int get_gpio_value(int port,int pins)
{
	int args[2];
#if 0
	args[0] = port;
	args[1] = pins;
#endif
	args[0] = GPIO_NUM(port,pins);
//	printf("port:%d pins:%d,gpio_fd:%d\n",args[0:],args[1],gpio_fd);
	ioctl(gpio_fd,GET_GPIO_VAL,args);
	return args[1];
}
#if  1

int register_gpio_callback(int port,int pins,int type,cb_func_ptr cb_func)
{
	int args[3];
	if(cb_func == NULL)
	{
		printf("Back function ptr\n");
		return -1;
	}
	if(irq_management.top_index < MAX_IRQ_NUM - 1)
	{
			irq_management.top_index++;
			args[0] = 1; //will request isr
			args[1] = GPIO_NUM(port,pins);
			args[2] = type;
			irq_management.irq_array[irq_management.top_index].gpio_no = GPIO_NUM(port,pins);
			irq_management.irq_array[irq_management.top_index].cb_func  = cb_func;
			dbg("register callback function,gpio_no:%d\n",args[1]);
			ioctl(gpio_fd,REQUEST_ISR,args);
			return 0;
	}else
			return -1;
}

int unregister_gpio_callback(int port, int pin)
{
	int gpio_no = GPIO_NUM(port,pin);
	int i,j;
	int args[2];
	for(i=0; i<=irq_management.top_index; i++)
	{
		if(irq_management.irq_array[i].gpio_no == gpio_no)
		{
			args[0] = 0; // will release irq
			args[1] = gpio_no;
			dbg("unregister callback function,gpio_no:%d\n",gpio_no);
			if(ioctl(gpio_fd,REQUEST_ISR,args) < 0)
			{
				err("ioctl err\n");
				return -1;
			}
			for(j=i;j<irq_management.top_index;j++)
					irq_management.irq_array[j] = irq_management.irq_array[j+1];
			irq_management.top_index--;
			return 0;
		}
	}
	err("unable to match port %d pin %d\n",port,pin);
	return -1;
}
void signal_action()
{
	int i;
	int irq_info[2];
	for(i=0; i <= irq_management.top_index; i++)
	{
		irq_info[0] = irq_management.irq_array[i].gpio_no;
		ioctl(gpio_fd,GET_ISR_INFO,irq_info);
		if(irq_info[1] == 1)
		{
			/*
			 	send msg to uplayer
			 */
			dbg("notify the uplayer\n");
			irq_management.irq_array[i].cb_func();
		}
	}
}
#endif

