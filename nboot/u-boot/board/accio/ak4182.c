#include "ak4182.h"

#define SPI1_BASE	 	0x08107000
#define SPI1_CAPR		(SPI1_BASE + 0x04)				// SPI1 Capability Reg
#define SPI1_CTL 		(SPI1_BASE + 0x08)				// SPI1 Control Reg
#define SPI1_OP			(SPI1_BASE + 0x0C)				// SPI1 Operational Reg
#define SPI1_MST		(SPI1_BASE + 0x10)				// SPI1 Master Reg
#define SPI1_MARC		(SPI1_BASE + 0x14)				// SPI1 Master Auto Read Count Reg
#define SPI1_SLV		(SPI1_BASE + 0x18)				// SPI1 Slave Reg
#define SPI1_SWTC		(SPI1_BASE + 0x1C)				// SPI1 Slave Word Timeout Count Reg
#define SPI1_FCR		(SPI1_BASE + 0x20)				// SPI1 FIFO Control Reg
#define SPI1_RBR		(SPI1_BASE + 0x24)				// SPI1 Receive Buffer Ready Reg
#define SPI1_THR		(SPI1_BASE + 0x28)				// SPI1 Transmit Holding Reg
#define SPI1_IER		(SPI1_BASE + 0x2C)				// SPI1 Interrupt Enable Reg
#define SPI1_STA		(SPI1_BASE + 0x30)				// SPI1 Status Register
#define BIT_THRE		0x2								// Bit 1 - Transmitter Holding Reg Empty
#define BIT_RST			0x2								// Bit 1 - Software Reset
#define BIT_ENA			0x1								// Bit 0 - SPI module enable
#define BIT_RXWM		0x40							// Bit 6 - RX FIFO Watermark Trigger
#define SPI1_TIMEOUT	8000

#define reg_rd32(p)		(*(volatile unsigned int *)(p))
#define reg_wr32(p, v)	(*(volatile unsigned int *)(p) = v)

static int spi1_write(unsigned int data);
static int spi1_read(unsigned char *buffer, int len);
static int ak4182_spi1_init(void);
static int spi1_disable(void);

static int spi1_write(unsigned int data)
{
	int timeout;	
	
	reg_wr32(SPI1_FCR, 0x3);												// reset FIFO
	timeout = SPI1_TIMEOUT;
	while ( (reg_rd32(SPI1_FCR)) && (timeout > 0) )							// check if FIFO reset completed
	{
		timeout--;
	}
	if (timeout > 0)
	{
		timeout = SPI1_TIMEOUT;
		while ( (!((reg_rd32(SPI1_STA)) & BIT_THRE)) && (timeout > 0) )		// wait until THR empty
		{
			timeout--;
		}
		if (timeout > 0)
		{
			reg_wr32(SPI1_THR, data);										// write data
			timeout = SPI1_TIMEOUT;
			while ( (!((reg_rd32(SPI1_STA)) & BIT_THRE)) && (timeout > 0) )	// wait until THR empty
			{
				timeout--;
			}
			if (timeout > 0)
			{
				return 0;
			}
		}
	}
	printf("cannot write 0x%08x thru SPI1\n", data);
	return -1;
}

static int spi1_read(unsigned char *buffer, int len)
{
	int timeout, word_cnt, num_of_read_bytes;
	unsigned int burst_read_num;
	unsigned char *ptr;
	unsigned char read_fail = 0;
	int actual_rd_len = 0;

	word_cnt = len;
	burst_read_num = ((reg_rd32(SPI1_CAPR)) & 0xFF) + 1;
	ptr=buffer;
	reg_wr32(SPI1_MST, 0x000C04F4);											// enable auto read, force CS active, clock rate = 8MHz (0x000C0212)
	//reg_wr32(SPI1_FCR, 0x3);                                                // reset FIFO
	timeout = SPI1_TIMEOUT;
	while (((reg_rd32(SPI1_FCR)) != 0) && (timeout > 0))
		timeout--;
	if (timeout) {
		while(word_cnt) {
			timeout = SPI1_TIMEOUT;
			while ((reg_rd32(SPI1_MARC)) && (timeout))
				timeout--;
			if(timeout) {
				if (word_cnt > burst_read_num) {
					num_of_read_bytes = burst_read_num;
					word_cnt -= burst_read_num;
				} else {
					num_of_read_bytes = word_cnt;
					word_cnt = 0;
				}
				reg_wr32(SPI1_MARC, 0x8);
				while (num_of_read_bytes) {
					timeout = SPI1_TIMEOUT;
					while (!((reg_rd32(SPI1_STA) & (0x41))) && (timeout))
						timeout--;
					if(timeout) {
						*ptr = (unsigned char)(reg_rd32(SPI1_RBR));
						ptr++;
						num_of_read_bytes--;
					} else {
						read_fail = 1;
						word_cnt = 0;
						num_of_read_bytes = 0;
					}
				}
			} else {
				read_fail = 2;
				word_cnt = 0;
			}
		}
	} else
		read_fail = 3;
	reg_wr32(SPI1_MST, 0x4F4);
	actual_rd_len = ptr - buffer;
	if (read_fail == 0) {
	//	printf("read %d byte thru SPI bus\n", actual_rd_len);
		return actual_rd_len;
	}
	printf("the read_fail = %d.\n", read_fail);
	printf("cannot read %d bytes thru SPI bus, %d bytes only\n", len, actual_rd_len);
	return -1;
}

static int ak4182_spi1_init(void)
{
	int timeout;
	reg_wr32(SPI1_CTL, BIT_ENA);                    // enable SPI1 module
	reg_wr32(SPI1_CTL, BIT_RST | BIT_ENA);          // reset SPI1
	timeout = SPI1_TIMEOUT;
	while ( (reg_rd32(SPI1_CTL)) & BIT_RST )        // wait until SPI1 reset complete
	{
		if (!(timeout--)) {
			reg_wr32(SPI1_CTL, 0);                  // disable SPI1 module if reset fail
			printf("cannot reset SPI1\n");
			return -1;
		}
	}
	reg_wr32(SPI1_OP, 0x80020607);                  // enable operation, SS0, master mode, POL=0, PHA=0, MSB 1st, SS active low, 8 bits/word
	reg_wr32(SPI1_MST, 0x404F4);                        // disable auto read, clock rate = 8MHz
	reg_wr32(SPI1_IER, 0);                          // disable all interrupts
	return 0;
}

static int spi1_disable(void)
{
	reg_wr32(SPI1_CTL, 0);							// disable SPI1 module										
	return 0;
}

unsigned int ak4182_init(void)
{
	unsigned char buff[3]={0};
	int i;
	unsigned int bvalue;
	if (ak4182_spi1_init() == 0) {
		spi1_write(0xA7);
		for(i=0;i<100;i++)
			udelay(1000);
		spi1_read(&buff, 8);
	}
	bvalue = (buff[1]<<8)|buff[2];
	bvalue = bvalue >> 3;
	bvalue &= 0x0fff;
	bvalue *= 2500;
	bvalue = bvalue >>12;
	bvalue *= 4;
	spi1_disable();
	return bvalue;
}

