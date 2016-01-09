#ifndef _SPI_LUANXU_
#define _SPI_LUANXU_

/**
@file		spi.h
@author		luanxu@solomon-systech.com
@version	1.0
@todo
*/

#define SPI_TOUT	20000

/** spi API returns */
typedef enum
{
	SPI_ERR_NONE	= 0,	/**< successful */
	SPI_ERR_HW		= -1,	/**< hardware error */
	SPI_ERR_PARM	= -2,	/**< parameter error */
	SPI_ERR_TOUT	= -3	/**< timeout */
}
spi_err;


/**< events in event callback */
typedef enum
{
	SPI_EVT_ERR,	/**< err */
	SPI_EVT_DONE	/**< done */
}
spi_evt;


/** spi master mode config */
typedef struct
{
	uint8_t		cs;			/**< chip select */
#ifdef SPI_AUTOCS
	uint8_t		burst;		/**< burst word width */
#endif
	uint8_t		word;		/**< word bit width */
	uint8_t		csdly;		/**< half-bit time from CS to data */
	uint16_t	wdly;		/**< bit time between spi word */
	int			baud;		/**< baud rate */
	int			per_clk;	/**< peripheral clock */
	int			lsb_first : 1;
	int			clk_phase : 1;
	int			clk_fall : 1;
	int			cs_high : 1;
}
spi_cfg_t, *spi_cfg_p;


#define SPI_XFR_CS	1		/* assert CS before xfer */
#define SPI_XFR_NCS	2		/* de-assert CS after xfer */

/** transfer descriptor */
typedef struct
{
	const uint8_t	*tx;	/**< tx data buffer */
	uint8_t	*rx;			/**< rx data buffer */
	int		len;			/**< buffer length in words */
#ifndef SPI_AUTOCS
	int		csflag;			/**< SPI_XFR_xxx or-mask */
#endif
	int		actual;			/**< rx or tx data length */
}
spi_xfr_t, *spi_xfr_p;


/** spi context */
typedef struct
{
	/** pubic - initialized by client */
	void		*r;			/**< register base address */
	void		*ctx;		/**< context for evt callback */
	void		(*evt)(void *ctx, spi_evt e);	/**< evt callback */
	uint8_t		master;		/**< master mode */
	uint8_t		dma;		/**< dma enable */

	/** private - zero-d by client on init, opaque after that */
	spi_xfr_p	bp;			/**< current xfer descriptor */
	int			inc;		/**< word size >> 4 */
	uint8_t		fifosize;	/**< FIFO depth */
#ifdef SPI_AUTOCS
	int			burst : 1;	/**< fixed bursts or derived CS */
#endif
}
spi_t, *spi_p;


/* api */

spi_err spi_init(spi_p t);
/**<
initialize spi device
@param[in] t		context
@return		spi error
*/


void spi_exit(spi_p t);
/**<
disable spi device
@param[in] t		context
*/


spi_err spi_cfg(spi_p t, spi_cfg_p c);
/**<
spi master configeration
@param[in] t		context
@param[in] c		config parameters
@return				spi error
*/


int spi_isr(spi_p t);
/**<
spi interrupt service routine
@param[in] t		context
@return			1=handled, 0=not handled
*/


int spi_cs(spi_p t, int ena);
/**<
assert/deassert CS
@param[in]	t		context
@param[in]	ena		0-deassert else-assert
@return		0-ok
*/


int spi_io(spi_p t, spi_xfr_p bp);
/**<
xfer data between spi and data buffer
@param[in]	t		context
@param[in]	bp		xfer descriptor
@return		0-ok
*/

spi_err spi_tx_wait(spir_p r);
#endif
