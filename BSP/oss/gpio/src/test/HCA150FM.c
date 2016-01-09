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
#include "TaurusModuleSlave.h"
////////////////////////////////////////////////////////////////
#define IO_PORT		3
// Output pins
#define FM_REST_PIN			4
#define FM_SCLK				9
#define FM_SDIO				12
#define FM_SEN				6

#define DEFAULT_FMRADIO_DEGREE	100
#define freq2ch(f) 	((f - 87500)/DEFAULT_FMRADIO_DEGREE)
#define ch2freq(ch)	(ch*DEFAULT_FMRADIO_DEGREE+87500)

//#define TWO_WIRE

#ifdef TWO_WIRE	
#define 	sda(D)	set_gpio_value(IO_PORT,FM_SDIO, D);
#define 	scl(D)	set_gpio_value(IO_PORT,FM_SCLK, D);

#define READ    0x21   
#define WRITE   0x20  

unsigned char OperationSi4700_2w(unsigned char operation, unsigned char *data, unsigned char numBytes)   
{   
    unsigned char controlWord,  j, error = 0;   
    int i;   
   
/***************************************************  
  
START: make sure here SDIO_DIR =OUT, SCLK = 1,  SDIO = 1  
  
****************************************************/   

    scl(1);   
    sda(1);   
    sda(0);   
    scl(0);   
       
   
/***************************************************  
  
WRITE CONTROL DATA: make sure here: SLCK = 0; SDIO = 0  
  
****************************************************/   
   
    if(operation == READ)   
        controlWord = 0x21;   
    else    
        controlWord = 0x20;   
       
    for(i = 7; i>=0; i--)   
    {   
        sda( (controlWord >> i) & 0x01);   
        //_delay_us(2);   
        scl(1);   
        scl(0);   
        //_delay_us(2);   
    }   
   
       
/***************************  
  
CHECK ACK for control word  
  
***************************/   
   
	set_gpio_input(IO_PORT,FM_SDIO);
   
    //_delay_us(2);   
    scl(1);    
    if(get_gpio_value(IO_PORT,FM_SDIO) != 0)   
    {   
        error = 1;   
        goto STOP;   
    }   
     scl(0);   
    //_delay_us(2);   
   
/***************************************  
  
WRITE or READ data  
  
****************************************/   
    for(j = 0; j < numBytes; j++, data++)   
    {   
        if(operation == WRITE)   
			set_gpio_output(IO_PORT,FM_SDIO,0);
        else   
            set_gpio_input(IO_PORT,FM_SDIO);
           
        for(i = 7; i>=0; i--)   
        {   
            if(operation == WRITE)   
			{
                sda( (*data >> i) & 0x01);   
			}
            //_delay_us(2);   
            scl(1);   
               
            if(operation == READ)   
			{
                *data = (*data << 1) | get_gpio_value(IO_PORT,FM_SDIO);   
			}
            scl(0);   
            //_delay_us(2);   
        }   
               
   
/******************************  
  
CHECK ACK or SEND ACK=0  
  
*******************************/   
   
        if(operation == WRITE)   
			set_gpio_output(IO_PORT,FM_SDIO,0);
        else   
        {   
            set_gpio_input(IO_PORT,FM_SDIO);
            if(j == (numBytes -1))   
			{
                sda(1);   
			}
            else   
			{
                sda(0);   
			}
        }   
        //_delay_us(2);   
        scl(1);   
           
        if(operation == WRITE)   
            if(get_gpio_value(IO_PORT,FM_SDIO) != 0)   
            {   
                error = 1;   
                goto STOP;   
            }   
            scl(0);   
        //_delay_us(2);   
    }   
       
   
/****************************  
  
STOP: make sure here: SCLK = 0  
  
*****************************/   
   
    STOP:   
   
    set_gpio_output(IO_PORT,FM_SDIO,0);
    sda(0);   
    scl(1);   
    sda(1);   
    //_delay_us(2);   
   
    return(error);   
   
}   
#endif
void FM_Write(unsigned short reg, unsigned short value);
unsigned short FM_Read(unsigned short reg);
void HCA150FM_init()
{
	int i;
	init_gpio_lib();
		
	set_gpio_output(IO_PORT,FM_REST_PIN, 0);
	set_gpio_output(IO_PORT,FM_SCLK, 0);
	set_gpio_output(IO_PORT,FM_SDIO, 0);
#ifdef TWO_WIRE				
	set_gpio_output(IO_PORT,FM_SEN, 1); // 2-wite
#else	
	set_gpio_output(IO_PORT,FM_SEN, 0); // 3-wire
#endif	
	usleep(5000);	
	set_gpio_value(IO_PORT,FM_REST_PIN, 1);
	usleep(5000);		
	set_gpio_output(IO_PORT,FM_SEN, 1);
	//FM_Write(7, 0xbc04); 
	//usleep(510000);	
#if 1	
	FM_Write(2, 0x0001);
	usleep(5000);	
	FM_Write(5, 0x001f);
	FM_Write(4, 0x1800);
	FM_Write(2, 0x4801);
	FM_Write(3, 0x8000 + freq2ch(100700));
	
	i = 0;
	while(!(FM_Read(0xa) & 0x4000))
	{	
		if(i++>1000)
		{		
			printf("timeout~\n");
			break;
		}
		usleep(100);
	}
	FM_Write(3, freq2ch(100700));
	//FM_Write(2, 0x4101); //seek

#endif	
}

void FM_Write(unsigned short reg, unsigned short value)
{
	int i;
	set_gpio_output(IO_PORT,FM_SCLK, 0);
	set_gpio_output(IO_PORT,FM_SDIO, 1);
	set_gpio_output(IO_PORT,FM_SEN, 0);	
	reg |= (0x60 << 1);
	for(i=8;i>=0;i--)
	{
		if(reg&(0x1<<i))
		{
			set_gpio_value(IO_PORT,FM_SDIO, 1);
			set_gpio_value(IO_PORT,FM_SCLK, 1);
			set_gpio_value(IO_PORT,FM_SCLK, 0);
		}
		else
		{
			set_gpio_value(IO_PORT,FM_SDIO, 0);
			set_gpio_value(IO_PORT,FM_SCLK, 1);
			set_gpio_value(IO_PORT,FM_SCLK, 0);
		}
	}	
	for(i=15;i>=0;i--)
	{
		if(value&(0x1<<i))
		{
			set_gpio_value(IO_PORT,FM_SDIO, 1);
			set_gpio_value(IO_PORT,FM_SCLK, 1);
			set_gpio_value(IO_PORT,FM_SCLK, 0);
		}
		else
		{
			set_gpio_value(IO_PORT,FM_SDIO, 0);
			set_gpio_value(IO_PORT,FM_SCLK, 1);
			set_gpio_value(IO_PORT,FM_SCLK, 0);
		}
	}		
	set_gpio_value(IO_PORT,FM_SEN, 1);
	//for(i=0;i<7;i++)
	{
		set_gpio_value(IO_PORT,FM_SCLK, 1);
		set_gpio_value(IO_PORT,FM_SCLK, 0);	
	}
	
}

unsigned short FM_Read(unsigned short reg)
{
	int i;
	unsigned short ret = 0;
	reg = reg | ((0x60 | 0x10) << 1);
	//printf("read reg: %02x\n", reg);	
	set_gpio_output(IO_PORT,FM_SCLK, 0);
	set_gpio_output(IO_PORT,FM_SDIO, 1);
	set_gpio_output(IO_PORT,FM_SEN, 0);
	
	for(i=8;i>=0;i--)
	{
		if(reg&(0x1<<i))
		{
			set_gpio_value(IO_PORT,FM_SDIO, 1);
			set_gpio_value(IO_PORT,FM_SCLK, 1);
			set_gpio_value(IO_PORT,FM_SCLK, 0);
		}
		else
		{
			set_gpio_value(IO_PORT,FM_SDIO, 0);
			set_gpio_value(IO_PORT,FM_SCLK, 1);
			set_gpio_value(IO_PORT,FM_SCLK, 0);
		}
	}	
	set_gpio_input(IO_PORT,FM_SDIO);
	//set_gpio_value(IO_PORT,FM_SCLK, 1);	
	for(i=15;i>=0;i--)
	{
		int r;
		set_gpio_value(IO_PORT,FM_SCLK, 1);
		set_gpio_value(IO_PORT,FM_SCLK, 0);
		r  = get_gpio_value(IO_PORT,FM_SDIO);
		//printf("%d ", r);
		ret |= r << i;

	}
	//printf("\n ");	
	set_gpio_value(IO_PORT,FM_SEN, 1);
	//for(i=0;i<7;i++)
	{
		set_gpio_value(IO_PORT,FM_SCLK, 1);
		set_gpio_value(IO_PORT,FM_SCLK, 0);	
	}
	return ret;
}

void FM_Out(int value)
{
	int fd = open("/dev/vol-update", O_RDWR);

	if (fd == -1) {
		printf("Error on opening the device file\n");
		return;
	}
	ioctl(fd, 4, &value);
	close(fd);
}

void SetFreq(int Freq)
{
	int i;
	printf("FM set freq: %d, ch(%d)\n", Freq, freq2ch(Freq));
	FM_Write(3, 0x8000 + freq2ch(Freq)); 	
	i = 0;
	while(!(FM_Read(0xa) & 0x4000))
	{	
		if(i++>1000)
		{		
			printf("timeout~\n");
			break;
		}
		usleep(10);
	}
	FM_Write(3, freq2ch(Freq)); 
}
////////////////////////////////////////////////////////////////
#define DAB_REST_PIN		14

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

static void DAB_set_address(int fd, unsigned short address)
{
	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msgs;
	unsigned char buf[2];
	
	buf[0] = address >> 8;
	buf[1] = address & 0xff;
	
	work_queue.nmsgs = 1;	
	work_queue.msgs = &msgs;
	msgs.len = 2;
	msgs.flags = 0;	//write
	msgs.addr = TAURUS_SLAVE_ID>>1;
	msgs.buf = buf;
  	
  	if(ioctl(fd,I2C_RDWR,(unsigned long)&work_queue) < 0)
   		printf("Error during I2C_RDWR ioctl with error\n");	
}

static void DAB_set_command(int fd, unsigned char cmd)
{
	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msgs;
	unsigned char buf[3] = {0x00, TAURUS_SLAVE_COMMAND_ADDR, 0x0};
	
	buf[2] = cmd;

	work_queue.nmsgs = 1;	
	work_queue.msgs = &msgs;
	msgs.len = 3;
	msgs.flags = 0;	//write
	msgs.addr = TAURUS_SLAVE_ID>>1;
	msgs.buf = buf;
  	
  	if(ioctl(fd,I2C_RDWR,(unsigned long)&work_queue) < 0)
   		printf("Error during I2C_RDWR ioctl with error\n");
}

static unsigned char DAB_read_status(int fd)
{
	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msgs;
	unsigned char buf[2];
	
	DAB_set_address(fd, TAURUS_SLAVE_COMMAND_STATUS_ADDR);
	usleep(1000);	
	work_queue.nmsgs = 1;	
	work_queue.msgs = &msgs;
	msgs.len = 1;
	msgs.flags = 1;	//read
	msgs.addr = TAURUS_SLAVE_ID>>1;
	msgs.buf = buf;
  	
  	if(ioctl(fd,I2C_RDWR,(unsigned long)&work_queue) < 0)
	{
   		printf("Error during I2C_RDWR ioctl with error\n");
		return 0xff;
	}
	return buf[0] ;
	
}

static void DAB_read_argument(int fd, unsigned char *buf, int len)
{
	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msgs;
	
	DAB_set_address(fd, TAURUS_SLAVE_ARGUMENT_ADDR);
	usleep(1000);	
	work_queue.nmsgs = 1;	
	work_queue.msgs = &msgs;
	msgs.len = len;
	msgs.flags = 1;	//read
	msgs.addr = TAURUS_SLAVE_ID>>1;
	msgs.buf = buf;
  	
  	if(ioctl(fd,I2C_RDWR,(unsigned long)&work_queue) < 0)
   		printf("Error during I2C_RDWR ioctl with error\n");
}

static void DAB_write_argument(int fd, unsigned char *buf, int len)
{
	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msgs;
	unsigned char buffer[12] = {0x00, TAURUS_SLAVE_ARGUMENT_ADDR};
	memcpy(&buffer[2], buf, len); 
	work_queue.nmsgs = 1;	
	work_queue.msgs = &msgs;
	msgs.len = len + 2;
	msgs.flags = 1;	//read
	msgs.addr = TAURUS_SLAVE_ID>>1;
	msgs.buf = buffer;
  	
  	if(ioctl(fd,I2C_RDWR,(unsigned long)&work_queue) < 0)
   		printf("Error during I2C_RDWR ioctl with error\n");
}

void DAB_get(unsigned char cmd)
{
	unsigned int fd;
	unsigned short version, i;
	unsigned char buf[128];

	fd = open("/dev/i2c-0", O_RDWR);
	if (!fd) {
		printf("Error on opening the device file\n");
		return;
	}
	//printf("ADS1000: %d \n",ads1000_read(fd));
	DAB_set_command(fd, cmd);
	usleep(1000);
	printf("DAB status: %02x \n",DAB_read_status(fd)); usleep(1000);
	DAB_read_argument(fd, buf, 128);
	printf("DAB get:  %02x", cmd);
	for(i=0;i<128;i++)
	{
		if((i % 8) == 0)
		{
			if((i % 16) == 0) printf("\n");
			else printf("- ");
		}
		printf("%02x ", buf[i]);
		
	}
	printf("\n");
	close(fd);
}

void DAB_test()
{
	unsigned int fd;
	unsigned short version;
	unsigned char buf[12];

	fd = open("/dev/i2c-0", O_RDWR);
	if (!fd) {
		printf("Error on opening the device file\n");
		return;
	}
	//printf("ADS1000: %d \n",ads1000_read(fd));
	DAB_set_command(fd, 0x01);
	usleep(1000);
	DAB_read_argument(fd, (unsigned char*)&version, 2);
	printf("DAB ver: %04x\n", version);	
	
	DAB_set_command(fd, 0x11);
	printf("DAB status: %02x \n",DAB_read_status(fd)); usleep(1000);
	printf("DAB status: %02x \n",DAB_read_status(fd)); usleep(1000);
	DAB_read_argument(fd, buf, 4);	
	printf("DAB date: %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3]);	
	DAB_set_command(fd, 0x12);
	printf("DAB status: %02x \n",DAB_read_status(fd)); usleep(1000);
	printf("DAB status: %02x \n",DAB_read_status(fd)); usleep(1000);
	DAB_read_argument(fd, buf, 3);	
	printf("DAB time: %02x %02x %02x\n", buf[0], buf[1], buf[2]);	
	close(fd);
}
////////////////////////////////////////////////////////////////
#define FUNCTION_0 "init FM module."
#define FUNCTION_1 "Set freq. [para1: freq.]"
#define FUNCTION_2 "Read register. [para1: reg]"	
#define FUNCTION_3 "Write register. [para1: reg] [para2: value]"	
#define FUNCTION_R "Read all register."	

int main(int argc,char* argv[])
{
	unsigned int value, freq, reg, i;
	unsigned char buf[32];
	if(argc < 2)
	{
		printf("USAGE: %s [function] [para1] [para2]\n" 
			   "\tfunction:\n"
			   "\t0: "FUNCTION_0"\n"
			   "\t1: "FUNCTION_1"\n"
			   "\t2: "FUNCTION_2"\n"			   
			   "\t3: "FUNCTION_3"\n"			   
			   "\t3: "FUNCTION_R"\n"			   
			   , argv[0]);
		return 0;
	}
	init_gpio_lib();
	switch(argv[1][0])
	{
		case '0':
			printf(FUNCTION_0"\n");		
			HCA150FM_init();
#ifdef TWO_WIRE			
			OperationSi4700_2w(READ, buf, 32);
			for(value=0;value<16;value++)
			{
				printf("%02x ", buf[value]);
			}			
#else
			for(value=0;value<16;value++)
			{
				printf("%04x ", FM_Read(value));
				if(value % 8 == 7) printf("\n");
			}		
#endif			
			printf("\n");
			break;
		case '1':
			sscanf(argv[2], "%d", &freq);
			SetFreq(freq);
			break;
			
		case '2':
			sscanf(argv[2], "%x", &reg);
			value = FM_Read(reg);
			printf(FUNCTION_2 "\t==>\t%02x = 0x%04x\n", reg, value);
			break;			
			
		case '3':
			sscanf(argv[2], "%x", &reg);
			sscanf(argv[3], "%x", &value);		
			FM_Write(reg, value);
			printf(FUNCTION_3 "\t==>\t%02x <== 0x%04x\n", reg, value);
			break;	
			
		case '4':
			FM_Write(2, 0x4301);
			i = 0;
			while(!(FM_Read(0xa) & 0x4000))
			{	
				if(i++>1000)
				{		
					printf("timeout~\n");
					FM_Write(2, 0x4001);
					break;
				}
				usleep(100);
			}		
			FM_Write(2, 0x4201);
			value = FM_Read(0x0b);
			freq = (value & 0x1ff)*100+87500;
			printf("ch: %d  freq: %d.%d\n", value& 0x1ff, freq/1000, freq%1000);
			break;
	
		case 'R':
			printf(FUNCTION_R"\n");
			for(value=0;value<16;value++)
			{
				printf("%04x ", FM_Read(value));
				if(value % 8 == 7) printf("\n");
			}		
			printf("\n");
			break;
		
		case 'M':
			sscanf(argv[2], "%d", &reg);
			printf("Set mixer: %d\n", reg);
			FM_Out(reg);
			break;
			
		case 'D':
		{
			if(argc > 2)			
			{
				sscanf(argv[2], "%x", &reg);
				DAB_get(reg);
			}
			else
			{
				set_gpio_output(IO_PORT,DAB_REST_PIN, 0);
				usleep(50000);	
				set_gpio_value(IO_PORT,DAB_REST_PIN, 1);
						
				DAB_test();
			}
		}
			break;
			
	}
}

