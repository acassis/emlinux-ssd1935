#include <stdio.h>
#include <linux/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include "gpio_ctrl.h"
////////////////////////////////////////////////////////////////
#define IO_PORT		3
// Output pins
#define WIFI_POWER_PIN					0
#define AMP_POWER_PIN					1
#define USB_SWITCH_FUNCTION_PIN			2
#define USB_SWITCH_EN_PIN				3
#define WM8750_CLOCK_PIN				4
#define DEBUG_LED_PIN					5
#define SD_CARD_POWER_PIN				6

// Input pins
#define CHARGER_FULL_INDICATOR_PIN		8
#define NO_BATTERY_INDICATOR_PIN		9
#define KEY_VOL_DOWN_PIN				12
#define KEY_VOL_UP_PIN					13
#define KEY_VOL_POWER_PIN				14
#define KEY_VOL_MUTE_PIN				15
#define DC5V_PLUGIN_INDICATOR_PIN		16
#define	WIFI_OVER_CURRENT_INDICATOR_PIN	17


void HCA700_IO_init()
{
	init_gpio_lib();
	set_gpio_output(IO_PORT,WIFI_POWER_PIN, 1);
	set_gpio_output(IO_PORT,AMP_POWER_PIN, 1);
	set_gpio_output(IO_PORT,USB_SWITCH_EN_PIN, 0);
	//set_gpio_output(IO_PORT,USB_SWITCH_FUNCTION_PIN, 1);
	set_gpio_output(IO_PORT,WM8750_CLOCK_PIN, 1);
	set_gpio_output(IO_PORT,DEBUG_LED_PIN, 1);
	set_gpio_output(IO_PORT,SD_CARD_POWER_PIN, 0);

	set_gpio_input(IO_PORT,CHARGER_FULL_INDICATOR_PIN);
	set_gpio_input(IO_PORT,NO_BATTERY_INDICATOR_PIN);
	set_gpio_input(IO_PORT,KEY_VOL_DOWN_PIN);
	set_gpio_input(IO_PORT,KEY_VOL_UP_PIN);
	set_gpio_input(IO_PORT,KEY_VOL_POWER_PIN);
	set_gpio_input(IO_PORT,KEY_VOL_MUTE_PIN);
	set_gpio_input(IO_PORT,DC5V_PLUGIN_INDICATOR_PIN);
	set_gpio_input(IO_PORT,WIFI_OVER_CURRENT_INDICATOR_PIN);
/*	
	HCA700_wifi_power_on();
	HCA700_AMP_power_on();
	HCA700_USB_switch_on();
	HCA700_USB_function_NONE();
	HCA700_WM8750_on();
	HCA700_SD_card_power_on();
*/	
}

unsigned int HCA700_get_key()
{
	unsigned int ret;
	ret  = get_gpio_value(IO_PORT,KEY_VOL_DOWN_PIN);
	ret |= get_gpio_value(IO_PORT,KEY_VOL_UP_PIN) << 1;
	ret |= get_gpio_value(IO_PORT,KEY_VOL_POWER_PIN) << 2;
	ret |= get_gpio_value(IO_PORT,KEY_VOL_MUTE_PIN) << 3;
	return ret;
}

unsigned int HCA700_get_battery()
{
	unsigned int ret;
	ret = get_gpio_value(IO_PORT,DC5V_PLUGIN_INDICATOR_PIN);
	ret |= get_gpio_value(IO_PORT,CHARGER_FULL_INDICATOR_PIN)<<1;
	ret |= get_gpio_value(IO_PORT,NO_BATTERY_INDICATOR_PIN)<<2;	
}

void HCA700_wifi_power_on()
{
	set_gpio_value(IO_PORT,WIFI_POWER_PIN, 1);
}
void HCA700_wifi_power_off()
{
	set_gpio_value(IO_PORT,WIFI_POWER_PIN, 0);
}
void HCA700_AMP_power_on()
{
	set_gpio_value(IO_PORT,AMP_POWER_PIN, 1);
}
void HCA700_AMP_power_off()
{
	set_gpio_value(IO_PORT,AMP_POWER_PIN, 0);
}
void HCA700_USB_switch_off()
{
	set_gpio_value(IO_PORT,USB_SWITCH_EN_PIN, 1);
}
void HCA700_USB_switch_on()
{
	set_gpio_value(IO_PORT,USB_SWITCH_EN_PIN, 0);
}
void HCA700_USB_function_WIFI()
{
	set_gpio_value(IO_PORT,USB_SWITCH_FUNCTION_PIN, 1);
}
void HCA700_USB_function_NONE()
{
	set_gpio_value(IO_PORT,USB_SWITCH_FUNCTION_PIN, 0);	
}
void HCA700_WM8750_on()
{
	set_gpio_value(IO_PORT,WM8750_CLOCK_PIN, 1);
}
void HCA700_WM8750_off()
{
	set_gpio_value(IO_PORT,WM8750_CLOCK_PIN, 0);
}
void HCA700_DEBUG_LED_on()
{
	set_gpio_value(IO_PORT,DEBUG_LED_PIN, 1);
}
void HCA700_DEBUG_LED_off()
{
	set_gpio_value(IO_PORT,DEBUG_LED_PIN, 0);
}
void HCA700_SD_card_power_on()
{
	set_gpio_value(IO_PORT,SD_CARD_POWER_PIN, 0);
}
void HCA700_SD_card_power_off()
{
	set_gpio_value(IO_PORT,SD_CARD_POWER_PIN, 1);
}

////////////////////////////////////////////////////////////////

#define TSC_MEASURE_AUX			(2 << 4)
#define TSC_MEASURE_TEMP0		(0 << 4)
#define TSC_MEASURE_TEMP1		(4 << 4)
#define TSC_MEASURE_X			(12 << 4)
#define TSC_MEASURE_Y			(13 << 4)

#define I2C_RETRIES     0x0701
#define I2C_TIMEOUT     0x0702
#define I2C_RDWR        0x0707
 
struct i2c_msg {
        unsigned short addr;     /* slave address                        */
        unsigned short flags;
#define I2C_M_RD        0x01
		unsigned short len;		/* msg length				*/        
        unsigned char *buf;      /* pointer to msg data                  */
};
struct i2c_rdwr_ioctl_data {
        struct i2c_msg *msgs;   /* pointers to i2c_msgs */
        int nmsgs;              /* number of i2c_msgs   */
};
#if 0
static void tsc2007_cmd(int fd, unsigned char cmd)
{
	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msgs;
	
	work_queue.nmsgs = 1;	
	work_queue.msgs = &msgs;
	msgs.len = 1;
	msgs.flags = 0;	//write
	msgs.addr = 0x4b;
	msgs.buf = &cmd;
  	
  	if(ioctl(fd,I2C_RDWR,(unsigned long)&work_queue) < 0)
   		printf("Error during I2C_RDWR ioctl with error\n");
	
}
 
static unsigned short tsc2007_read(int fd, unsigned char source)
{
	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msgs;
	unsigned char buf[2];
	
	tsc2007_cmd(fd, source|4);
	usleep(5000);	
	work_queue.nmsgs = 1;	
	work_queue.msgs = &msgs;
	msgs.len = 2;
	msgs.flags = 1;	//read
	msgs.addr = 0x4b;
	msgs.buf = buf;
  	
  	if(ioctl(fd,I2C_RDWR,(unsigned long)&work_queue) < 0)
   		printf("Error during I2C_RDWR ioctl with error\n");
	return ((unsigned short)buf[0]<<4)|(buf[1]>>4);
}
#endif

static unsigned short ads1000_read(int fd)
{
	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msgs;
	unsigned char buf[3];
	
	work_queue.nmsgs = 1;	
	work_queue.msgs = &msgs;
	msgs.len = 3;
	msgs.flags = 1;	//read
	msgs.addr = 0x48;
	msgs.buf = buf;
  	
  	if(ioctl(fd,I2C_RDWR,(unsigned long)&work_queue) < 0)
	{
   		printf("Error during I2C_RDWR ioctl with error\n");
		return 0;
	}
	
	return ((buf[0]<<8)|buf[1]);
}

static void tpa2016_cmd(int fd, unsigned char cmd)
{
	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msgs;
	
	work_queue.nmsgs = 1;	
	work_queue.msgs = &msgs;
	msgs.len = 1;
	msgs.flags = 0;	//write
	msgs.addr = 0x58;
	msgs.buf = &cmd;
  	
  	if(ioctl(fd,I2C_RDWR,(unsigned long)&work_queue) < 0)
   		printf("Error during I2C_RDWR ioctl with error\n");
	
}
static unsigned short tpa2016_read(int fd, unsigned char source)
{
	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msgs;
	unsigned char buf[2];
	
	tpa2016_cmd(fd, source);
	usleep(1000);	
	work_queue.nmsgs = 1;	
	work_queue.msgs = &msgs;
	msgs.len = 2;
	msgs.flags = 1;	//read
	msgs.addr = 0x58;
	msgs.buf = buf;
  	
  	if(ioctl(fd,I2C_RDWR,(unsigned long)&work_queue) < 0)
   		printf("Error during I2C_RDWR ioctl with error\n");
	return (buf[0]<<4)|(buf[1]>>4);
}
  
unsigned short HCA700_Get_Light_Sensor()
{
	unsigned int fd;
	unsigned int aux;
 #if 0
	fd = open("/dev/i2c-0", O_RDWR);
	if (!fd) {
		printf("Error on opening the device file\n");
		return 0;
	}
	aux = tsc2007_read(fd, TSC_MEASURE_AUX);    
#else
	fd = open("/proc/lightsensor", O_RDWR);
	if (!fd) {
		printf("Error on opening the device file\n");
		return 0;
	}
	read(fd, &aux, 4);    	
#endif
	close(fd);

	return aux;
}

unsigned short HCA700_Get_Battery_Level()
{
	unsigned int fd;
	unsigned short aux;
 
	fd = open("/dev/i2c-0", O_RDWR);
	if (!fd) {
		printf("Error on opening the device file\n");
		return 0;
	}
	aux = ads1000_read(fd);    
	close(fd);
	return (aux*3300*2/2048) + 40;
}

////////////////////////////////////////////////////////////////
#define SSLBL_INTENSITY_GET	_IOR('f',1,int)
#define SSLBL_INTENSITY_SET	_IOW('f',2,int)
#define BACKLIGHT_FD	"/dev/backlight"
unsigned short HCA700_Get_Backlight_level()
{
	int fd, value;
	fd = open(BACKLIGHT_FD,O_RDONLY);
	if (fd<0)
	{
		printf("open "BACKLIGHT_FD" error.\n");
		exit(-1);
	}
	ioctl(fd, SSLBL_INTENSITY_GET, &value);
	close(fd);
	return value;
}

void HCA700_Set_Backlight_level(unsigned char value)
{
	int fd;
	fd = open(BACKLIGHT_FD,O_RDONLY);
	if (fd<0)
	{
		printf("open "BACKLIGHT_FD" error.\n");
		exit(-1);
	}
	ioctl(fd, SSLBL_INTENSITY_SET, &value);
	close(fd);
}

////////////////////////////////////////////////////////////////
#define FUNCTION_0 "IO function init."
#define FUNCTION_1 "WIFI power on/off test."
#define FUNCTION_2 "USB switch on/off test."	
#define FUNCTION_3 "USB function 0/1 test."			   
#define FUNCTION_4 "AMP on/off test."
#define FUNCTION_5 "DEBUG LED on/off test."
#define FUNCTION_6 "SD card power on/off test."
#define FUNCTION_7 "WM8750 clock on/off test."
#define FUNCTION_8 "Key read test."
#define FUNCTION_9 "Battery status test."

#define FUNCTION_B "Lighe Sensor test."
#define FUNCTION_A "Backlight test."
#define FUNCTION_C "All I/O off."
#define FUNCTION_M "Key function test."

int main(int argc,char* argv[])
{
	int is_on, value;
	if(argc < 2)
	{
		printf("USAGE: %s [function] [value]\n" 
			   "\tfunction:\n"
			   "\t0: "FUNCTION_0"\n"
			   "\t1: "FUNCTION_1"\n"
			   "\t2: "FUNCTION_2"\n"
			   "\t3: "FUNCTION_3"\n"
			   "\t4: "FUNCTION_4"\n"
			   "\t5: "FUNCTION_5"\n"
			   "\t6: "FUNCTION_6"\n"
			   "\t7: "FUNCTION_7"\n"
			   "\t8: "FUNCTION_8"\n"			   
			   "\t9: "FUNCTION_9"\n"
			   "\tA: "FUNCTION_A"\n"			   
			   "\tB: "FUNCTION_B"\n"	
			   "\tC: "FUNCTION_C"\n"	
			   "\tM: "FUNCTION_M"\n"	
			   , argv[0]);
		return 0;
	}
	if(argc >= 3)
		is_on = argv[2][0] == '1';
	init_gpio_lib();	
	switch(argv[1][0])
	{
		case '0':
			printf(FUNCTION_0"\n");		
			HCA700_IO_init();
			break;
		case '1':
			if(is_on) 	HCA700_wifi_power_on();
			else		HCA700_wifi_power_off();
			printf(FUNCTION_1 "\t==>\t%s\n", is_on?"on":"off");
			break;
		case '2':
			if(is_on) 	HCA700_USB_switch_on();
			else		HCA700_USB_switch_off();		
			printf(FUNCTION_2 "\t==>\t%s\n", is_on?"on":"off");
			break;		
		case '3':
			if(is_on) 	HCA700_USB_function_WIFI();
			else		HCA700_USB_function_NONE();		
			printf(FUNCTION_3 "\t==>\t%s\n", is_on?"WIFI":"None");
			break;		
		case '4':
			if(is_on) 	HCA700_AMP_power_on();
			else		HCA700_AMP_power_off();		
			printf(FUNCTION_4 "\t==>\t%s\n", is_on?"on":"off");
			break;		
		case '5':
			if(is_on) 	HCA700_DEBUG_LED_on();
			else		HCA700_DEBUG_LED_off();		
			printf(FUNCTION_5 "\t==>\t%s\n", is_on?"on":"off");
			break;		
		case '6':
			if(is_on) 	HCA700_SD_card_power_on();
			else		HCA700_SD_card_power_off();		
			printf(FUNCTION_6 "\t==>\t%s\n", is_on?"on":"off");
			break;		
		case '7':
			if(is_on) 	HCA700_WM8750_on();
			else		HCA700_WM8750_off();		
			printf(FUNCTION_7 "\t==>\t%s\n", is_on?"on":"off");
			break;					
		case '8':
			printf(FUNCTION_8 "\n");
			while(1)
			{
				unsigned int value;
				usleep(500000);
				value = HCA700_get_key();
				printf("key: %02x\n", value);
			}
			break;		
		case '9':
			printf(FUNCTION_9 "\n");
			while(1)
			{
				unsigned int value;
				usleep(500000);
				value = HCA700_get_battery();
				printf("Battery:  [%02x]", value);
				printf("\tDC5v IN: %s, ",		value & 1 ? "yes":"no");
				printf("\tFull: %s, ", 		value & 2 ? "yes":"no");
				printf("\tNo Battery: %s, ",	value & 4 ? "yes":"no");
				printf("\tValue = %d\n",HCA700_Get_Battery_Level());
			}
			break;	
		case 'B':
		case 'b':
			printf(FUNCTION_B "\n");
			printf("Value = %d\n",HCA700_Get_Light_Sensor());
			break;

		case 'A':
		case 'a':
			printf(FUNCTION_A "\n");
			if(argc < 3) break;
			sscanf(argv[2],"%d", &value);
			printf("Value = %d ==> %d\n",HCA700_Get_Backlight_level(), value);
			HCA700_Set_Backlight_level(value);
			break;	
		case 'C':
		case 'c':
			printf(FUNCTION_C "\n");
			HCA700_wifi_power_off();
			HCA700_AMP_power_off();
			HCA700_USB_switch_off();
			HCA700_WM8750_off();
			HCA700_DEBUG_LED_off();
			HCA700_SD_card_power_off();	
			HCA700_Set_Backlight_level(0);
			system("killall -1 watchdog");		
			break;
		case 'M':
		case 'm':
			printf(FUNCTION_M "\n");
			HCA700_IO_init();
			while(1)
			{
				unsigned int value;
				usleep(50000);
				value = HCA700_get_key();	
				if(value & 0x04)
				{
					char path[255];
					HCA700_wifi_power_off();
					HCA700_AMP_power_off();
					HCA700_USB_switch_off();
					HCA700_WM8750_off();
					HCA700_DEBUG_LED_off();
					//HCA700_SD_card_power_off();	
					HCA700_Set_Backlight_level(0);
					system("killall -1 watchdog");
					snprintf(path, sizeof(path), "echo enable > /sys/dpm/control");
					sleep(1);
					system(path);
					sleep(1);					
					HCA700_IO_init();
					HCA700_Set_Backlight_level(250);
				}
				else if(value & 0x0b)
				{
					int fd;
					struct UI2Message{
						int type;
						unsigned int data[3];
					};
					//printf("get key: % 02x\n", value);
					if((fd = open("/tmp/UI2.fifo", O_WRONLY|O_NONBLOCK))!=-1)
					{
						struct UI2Message msg;
						msg.type = 5 ; // from UInput.h in UI2						
						if(value & 1)						
							msg.data[0] = 0x0006000e;  // check UMessageIDs.h in uGUI
						else if(value & 2)
							msg.data[0] = 0x0006000f;  // check UMessageIDs.h in uGUI
						else
							msg.data[0] = 0x00060010;  // check UMessageIDs.h in uGUI
						msg.data[1] = 0x010000;
						write(fd, &msg, sizeof(msg));
						sync();
						close(fd);										
					}
					usleep(150000);
				}
			}
			break;
	}
}

