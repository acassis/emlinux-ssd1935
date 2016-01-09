#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <linux/input.h>
#include <fcntl.h>
#include "../ssl_bl.h"

#define PRESSED 	1
#define RELEASED 	0

int main(int argc, char **argv)
{
	int bl;
	int value;

	
	bl = open(argv[1],O_RDONLY);
	if (bl<0)
	{
		printf("the %s is erro.\n", argv[1]);
		exit(-1);
	}

	printf("Babk Light test\n");
	
	if(argc>3)
	{
		sscanf(argv[3],"%d", &value);
		ioctl(bl, SSLBL_FREQ_DIV_SET, &value);	
		value = (value + 1)*256;
		printf("BL freq divided by: %d (%d Hz)\n", value, 234368/value);
	}
	else
	{
		ioctl(bl, SSLBL_INTENSITY_GET, &value);
		printf("BL get: %d\n", value);
		sscanf(argv[2],"%d", &value);
		printf("BL set to: %d, %s\n", value, argv[2]);
		ioctl(bl, SSLBL_INTENSITY_SET, &value);
	}
	close(bl);

	return 0;
}
