#include <stdint.h>


#define UART_CTL		(0x08 >> 2)
#define UART_THR		(0x10 >> 2)
#define UART_LCR		(0x20 >> 2)
#define UART_LSR		(0x28 >> 2)
#define UART_DLL		(0x30 >> 2)
#define UART_CTL_EN		1
#define UART_CTL_RST	2
#define UART_LSR_TEMT	0x40
#define UART_LCR_W8		0x03
#define UART_LCR_S1		0x10


void  uart_init(void)
{
	volatile uint32_t	*r = (void *)0x8103000;

	/* enable, reset and wait for ready */
	r[UART_CTL] = UART_CTL_RST | UART_CTL_EN;
	while ((r[UART_CTL] & (UART_CTL_EN | UART_CTL_RST)) != UART_CTL_EN)
	{
		;
	}

	r[UART_LCR] = UART_LCR_W8 | UART_LCR_S1;
	/* 115200bps */
	r[UART_DLL] = 0x20;
}


int puts(char *s)
{
	volatile uint32_t	*r = (void *)0x8103000;
	int					c;

	while ((c = *s))
	{
l_putchar:
		while (!(r[UART_LSR] & UART_LSR_TEMT))
		{
			;
		}
		r[UART_THR] = c;
		if (c == '\n')
		{
			c = '\r';
			goto l_putchar;
		}
		s++;
	}
	return 0;
}

