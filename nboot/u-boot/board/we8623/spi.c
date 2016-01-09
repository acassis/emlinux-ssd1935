#include "spi.h"
#include "../../../include/config.h"


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
#define GPIO_BASE		0x0810F000
#define GPIO_PORTF		0x500
#define GPIO_PF_FUNC	(GPIO_BASE + GPIO_PORTF + 14)	// GPIO port F Function Reg

#define SPI1_TIMEOUT	8000

#define reg_rd32(p)		(*(volatile unsigned int *)(p))
#define reg_wr32(p, v)	(*(volatile unsigned int *)(p) = v)


static int spi1_init(void);
static int spi1_disable(void);
static int spi1_write(unsigned int data);
static int spi1_write_hx8257(unsigned char reg_addr, unsigned short reg_data);


static int spi1_init(void)
{
	#ifdef CONFIG_CPT480X272

	int timeout;
	unsigned int tmp;
					
	tmp = reg_rd32(GPIO_PF_FUNC);
	reg_wr32(GPIO_PF_FUNC, tmp & 0xF0FFFFFF);		// select primary function (SPI) in GPIO
	reg_wr32(SPI1_CTL, BIT_ENA);					// enable SPI1 module
	reg_wr32(SPI1_CTL, BIT_RST | BIT_ENA);			// reset SPI1
	timeout = SPI1_TIMEOUT;
	while ( (reg_rd32(SPI1_CTL)) & BIT_RST )		// wait until SPI1 reset complete
	{
		if (!(timeout--))
		{  		
  			reg_wr32(SPI1_CTL, 0);					// disable SPI1 module if reset fail
  			printf("cannot reset SPI1\n");
  			return -1;
		}
	}
	reg_wr32(SPI1_OP, 0x80000277);					// enable operation, SS0, master mode, POL=1, PHA=1, MSB 1st, SS active low, 24 bits/word
	//reg_wr32(SPI1_OP, 0x80030277);					// enable operation, SS3, master mode, POL=1, PHA=1, MSB 1st, SS active low, 24 bits/word
	//reg_wr32(SPI1_MST, 0x1);						// disable auto read, clock rate = 8MHz
	reg_wr32(SPI1_MST, 0x0);						// disable auto read, clock rate = 16MHz		
	reg_wr32(SPI1_IER, 0);							// disable all interrupts			  	
	return 0;
	
	#elif CONFIG_TPO800X480
	
	int timeout;
	unsigned int tmp;
					
	tmp = reg_rd32(GPIO_PF_FUNC);
	reg_wr32(GPIO_PF_FUNC, tmp & 0xF0FFFFFF);		// select primary function (SPI) in GPIO
	reg_wr32(SPI1_CTL, BIT_ENA);					// enable SPI1 module
	reg_wr32(SPI1_CTL, BIT_RST | BIT_ENA);			// reset SPI1
	timeout = SPI1_TIMEOUT;
	while ( (reg_rd32(SPI1_CTL)) & BIT_RST )		// wait until SPI1 reset complete
	{
		if (!(timeout--))
		{  		
  			reg_wr32(SPI1_CTL, 0);					// disable SPI1 module if reset fail
  			printf("cannot reset SPI1\n");
  			return -1;
		}
	}
	reg_wr32(SPI1_OP, 0x8000020F);					// enable operation, SS0, master mode, POL=0, PHA=0, MSB 1st, SS active low, 16 bits/word
	//reg_wr32(SPI1_MST, 0x1);						// disable auto read, clock rate = 8MHz
	reg_wr32(SPI1_MST, 0x0);						// disable auto read, clock rate = 16MHz		
	reg_wr32(SPI1_IER, 0);							// disable all interrupts			  	
	return 0;
		
	#else

	#endif	
}


static int spi1_disable(void)
{
	reg_wr32(SPI1_CTL, 0);							// disable SPI1 module										
	return 0;
}


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


static int spi1_write_hx8257(unsigned char reg_addr, unsigned short reg_data)
{
	#ifdef CONFIG_CPT480X272

	unsigned int data_raw;
	unsigned int data_out;
	
	data_raw = reg_addr;
	data_out = 0x00700000 | data_raw;
	if ( spi1_write(data_out) == 0 )
	{
		data_raw = reg_data;
		data_out = 0x00720000 | data_raw;
		if ( spi1_write(data_out) == 0 )
		{
			return 0;
		}	
	}	
	return -1;
	
	#elif CONFIG_TPO800X480
	
	unsigned int data_raw;
	unsigned int data_out;
	
	data_raw = reg_addr;
	data_raw <<= 10;
	data_raw &= 0x0000FC00;
	reg_data &= 0x000000FF;
	data_out = data_raw | reg_data;
	if ( spi1_write(data_out) == 0 )
	{
		return 0;
	}	
	return -1;
		
	#else
	
	#endif	
}


void lcm_ssd2123_init(void)
{
	if ( spi1_init() == 0 )		
	{
		#ifdef CONFIG_CPT480X272
		/*
		spi1_write_hx8257(0x28, 0x0006);
		spi1_write_hx8257(0x04, 0x042A);
		spi1_write_hx8257(0x0B, 0xD000);
		spi1_write_hx8257(0x2A, 0x01D2);
		spi1_write_hx8257(0x03, 0x0A0F);
		spi1_write_hx8257(0x0C, 0x0005);
		spi1_write_hx8257(0x0D, 0x000C);
		spi1_write_hx8257(0x0E, 0x2C00);
		spi1_write_hx8257(0x1E, 0x029C);
		spi1_write_hx8257(0x30, 0x0000);
		spi1_write_hx8257(0x31, 0x0302);
		spi1_write_hx8257(0x32, 0x0304);
		spi1_write_hx8257(0x33, 0x0207);
		spi1_write_hx8257(0x34, 0x0506);
		spi1_write_hx8257(0x35, 0x0003);
		spi1_write_hx8257(0x36, 0x0707);
		spi1_write_hx8257(0x37, 0x0303);
		spi1_write_hx8257(0x3A, 0x0F0F);
		spi1_write_hx8257(0x3B, 0x0F02);
		
		//spi1_write_hx8257(0x01, 0x230F);
		//spi1_write_hx8257(0x10, 0x02CE);
		//spi1_write_hx8257(0x15, 0x0090);
		//spi1_write_hx8257(0x16, 0xEF8E);
		//spi1_write_hx8257(0x17, 0x0003);
		*/
		spi1_write_hx8257(0x28, 0x0006);
		spi1_write_hx8257(0x01, 0x230f);
		spi1_write_hx8257(0x02, 0x0c02);
		spi1_write_hx8257(0x03, 0x91ee);
		spi1_write_hx8257(0x04, 0x0440);
		spi1_write_hx8257(0x0b, 0x2004);
		spi1_write_hx8257(0x0c, 0x0005);
		spi1_write_hx8257(0x0d, 0x000A);
		spi1_write_hx8257(0x0e, 0x3000);
		spi1_write_hx8257(0x1e, 0x0298);
		spi1_write_hx8257(0x16, 0xEF8E);
		spi1_write_hx8257(0x17, 0x0003);
		spi1_write_hx8257(0x30, 0x0700);
		spi1_write_hx8257(0x31, 0x0305);
		spi1_write_hx8257(0x32, 0x0204);
		spi1_write_hx8257(0x33, 0x0205);
		spi1_write_hx8257(0x34, 0x0507);
		spi1_write_hx8257(0x35, 0x0304);
		spi1_write_hx8257(0x36, 0x0207);
		spi1_write_hx8257(0x37, 0x0203);
		spi1_write_hx8257(0x3a, 0x000f); 
		spi1_write_hx8257(0x3b, 0x1300);
		{
			int delay = 1000;
			
			while (delay)
				delay--;
		}
		spi1_write_hx8257(0x28, 0x0006);
		spi1_write_hx8257(0x2d, 0x3f40);
		
		#elif CONFIG_TPO800X480
		
		spi1_write_hx8257(0x11, 0x05); //MSB: GAMMA0[9:8], GAMMA8[9:8], GAMMA16[9:8], GAMMA32[9:8] : LSB
		spi1_write_hx8257(0x12, 0x6A); //MSB: GAMMA64[9:8], GAMMA96[9:8], GAMMA128[9:8], GAMMA192[9:8] :LSB
		spi1_write_hx8257(0x13, 0xFF); //MSB: GAMMA224[9:8], GAMMA240[9:8], GAMMA248[9:8], GAMMA256[9:8] :LSB	
		spi1_write_hx8257(0x14, 0x6A); //GAMMA0[7:0]
		spi1_write_hx8257(0x15, 0xC8); //GAMMA8[7:0]
		spi1_write_hx8257(0x16, 0x21); //GAMMA16[7:0]
		spi1_write_hx8257(0x17, 0x77); //GAMMA32[7:0]
		spi1_write_hx8257(0x18, 0xCC); //GAMMA64[7:0]
		spi1_write_hx8257(0x19, 0x1F); //GAMMA96[7:0]
		spi1_write_hx8257(0x1a, 0x71); //GAMMA128[7:0]
		spi1_write_hx8257(0x1b, 0xC1); //GAMMA192[7:0]
		spi1_write_hx8257(0x1c, 0x11); //GAMMA224[7:0]
		spi1_write_hx8257(0x1d, 0x60); //GAMMA240[7:0]
		spi1_write_hx8257(0x1e, 0xAE); //GAMMA248[7:0]
		spi1_write_hx8257(0x1f, 0xFC); //GAMMA256[7:0]
		spi1_write_hx8257(0x20, 0xf0); //[7:4] Positive Gamma output driver input FFH, [3:0] Positive Gamma output driver input 00H
		spi1_write_hx8257(0x21, 0xf0); //[7:4] Negative Gamma output driver input FFH, [3:0] Negative Gamma output driver input 00H
		
		#else
		
		#endif
														
		spi1_disable();
	}
}


