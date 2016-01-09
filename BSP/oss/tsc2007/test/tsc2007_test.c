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

#define TSC_MEASURE_AUX			(2 << 4)
#define TSC_MEASURE_TEMP0		(0 << 4)
#define TSC_MEASURE_TEMP1		(4 << 4)
#define TSC_MEASURE_X			(12 << 4)
#define TSC_MEASURE_Y			(13 << 4)


#define I2C_RETRIES     0x0701
#define I2C_TIMEOUT     0x0702
#define I2C_RDWR        0x0707
 
struct i2c_msg {
        __u16 addr;     /* slave address                        */
        __u16 flags;
#define I2C_M_RD        0x01
		__u16 len;		/* msg length				*/        
        __u8 *buf;      /* pointer to msg data                  */
};
struct i2c_rdwr_ioctl_data {
        struct i2c_msg *msgs;   /* pointers to i2c_msgs */
        int nmsgs;              /* number of i2c_msgs   */
};

void tsc2007_cmd(int fd, unsigned char cmd)
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
 
unsigned short tsc2007_read(int fd, unsigned char source)
{
	struct i2c_rdwr_ioctl_data work_queue;
	struct i2c_msg msgs;
	unsigned char buf[2];
	
	tsc2007_cmd(fd, source);
	usleep(1000);	
	work_queue.nmsgs = 1;	
	work_queue.msgs = &msgs;
	msgs.len = 2;
	msgs.flags = 1;	//read
	msgs.addr = 0x4b;
	msgs.buf = buf;
  	
  	if(ioctl(fd,I2C_RDWR,(unsigned long)&work_queue) < 0)
   		printf("Error during I2C_RDWR ioctl with error\n");
	return (buf[0]<<4)|(buf[1]>>4);
}
 
 
int main(int argc, char **argv)
{
  unsigned int fd;
  unsigned short adx, ady, t0, t1, aux;
 
  fd = open("/dev/i2c-0", O_RDWR);
 
  if (!fd) {
  	printf("Error on opening the device file\n");
  	return 0;
  }

  adx = tsc2007_read(fd, TSC_MEASURE_X);  
  ady = tsc2007_read(fd, TSC_MEASURE_Y);  
  t0 = tsc2007_read(fd, TSC_MEASURE_TEMP0);  
  t1 = tsc2007_read(fd, TSC_MEASURE_TEMP1);  
  aux = tsc2007_read(fd, TSC_MEASURE_AUX);  
  
  close(fd);
  printf("ts, x:%d, y:%d, t0:%d, t1:%d, aux:%d\n", adx, ady, t0, t1, aux);
  return 0;
}
