#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gpio_ctrl.h"
int gpio_usage(int argc, char** argv);
void gpio_cb(void);
void input_isr(int port,int pin);
#define IRQ_TYPE_EDGE_RISING    0x00000001  /* Edge rising type */
#define IRQ_TYPE_EDGE_FALLING   0x00000002  /* Edge falling type */
static int port,pin;
int main(int argc,char* argv[])
{
	int val,direction,isr,trigger;
	char ch;
	sscanf(argv[1],"%d",&direction);
	sscanf(argv[2],"%d",&port);
	sscanf(argv[3],"%d",&pin);
	sscanf(argv[4],"%d",&isr);
	printf("direction:%d,port:%d,pin:%d,isr:%d\n",direction,port,pin,isr);
	if(isr == 1)
	{
		if(argc < 5)
		{
			printf("gpio_cfg direction gpio_port gpio_pin isr_enable [trigger]");
			return -1;
		}
		sscanf(argv[5],"%d",&trigger);
		printf("trigger type:%d\n",trigger);
	}

	//direction: 0 in 1out
	init_gpio_lib();
	switch(direction){
		case 0:
#if 1
				set_gpio_input(port,pin);
				val = get_gpio_value(port,pin);
				printf("value -- port %d pin %d : %d\n",port,pin,val);
				if(isr == 1)
				{
					register_gpio_callback(port,pin,trigger,gpio_cb);
					
				}
#else
				input_isr(4,21);
				input_isr(4,23);
				input_isr(4,24);
#endif
				break;
		case 1:
				sscanf(argv[4],"%d",&val);
				set_gpio_output(port,pin,val);
				break;
	};
	do{
		printf("wait for exit... : \n");
		scanf("%c",&ch);
	}while(ch != 'e');
	unregister_gpio_callback(port,pin);
	exit_gpio_lib();
	
}
void input_isr(int port,int pin)
{
		int val;
		int trigger = 1;
		int gpio[2];
				set_gpio_input(port,pin);
				val = get_gpio_value(port,pin);
				printf("value -- port %d pin %d : %d\n",port,pin,val);
				gpio[0] = port;
				gpio[1] = pin;
				register_gpio_callback(port,pin,trigger,gpio_cb);
					

}

void gpio_cb(void)
{
	printf("enter gpio_cb...\n");
	printf("exit gpio_cb...\n");
}
int gpio_usage(int argc, char** argv)
{
	if(!strncmp(argv[0],"--help",6))
	{
		printf("gpio_cfg direction gpio_port gpio_pin isr_enable [trigger]");
		return 0;
	}
	if(argc < 4)
	{
		printf("gpio_cfg direction gpio_port gpio_pin isr_enable [trigger]");
		return -1;
	}
}
