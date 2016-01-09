#ifndef __DMACAPI_H__
#define __DMACAPI_H__

#define DMAC_BT_SINGLE		0	// unspecified length only
#define DMAC_BT_INCR4		2	// incr4
#define DMAC_BT_INCR4U		3	// incr4 with unspecified length
#define DMAC_BT_INCR8		4	// incr8
#define DMAC_BT_INCR8U		5	// incr8 with unspecified length
#define DMAC_BT_INCR16		6	// incr16
#define DMAC_BT_INCR16U		7	// incr16 with unspecified length

#define DMAC_PRI_ROBIN		0	// round robin priorit
#define DMAC_PRI_FIXED		1	// fixed priority, ch0 is highest priority

#define DMAC_ASZ_8BIT		0
#define DMAC_ASZ_16BIT		1
#define DMAC_ASZ_32BIT		2
#define DMAC_ASZ_64BIT		3

#define DMAC_TYP_MEM		0
#define DMAC_TYP_FIFO		1

#define DMAC_STAT_DONE		1
#define DMAC_STAT_BSY		2

// dma src and dst req numbers
#define DMAC_UART1_TX_REQ	0
#define DMAC_UART1_RX_REQ	1
#define DMAC_UART2_TX_REQ	2
#define DMAC_UART2_RX_REQ	3
#define DMAC_UART3_TX_REQ	4
#define DMAC_UART3_RX_REQ	5
#define DMAC_UART4_TX_REQ	6
#define DMAC_UART4_RX_REQ	7
#define DMAC_SPI1_TX_REQ	8
#define DMAC_SPI1_RX_REQ	9
#define DMAC_SPI2_TX_REQ	10
#define DMAC_SPI2_RX_REQ	11

typedef struct _dmac_cfg_t {
	unsigned long	pri;		// input: 0 = round-robin, 1=fixed priority (ch0 == highest pri)
	unsigned long	wait_dly;	// input: 1-64 clocks
	unsigned long	burst_type;	// input: incr, incr4, incr4u, incr8, incr8u, incr16, incr16u
	unsigned long	n_ch;		// output
	unsigned long	fifo_sz;	// output
	unsigned long	id;			// output
	
} dmac_cfg_t;

typedef struct _dmac_ch_cfg_t {
	unsigned long	src_addr;
	unsigned long	dst_addr;
	unsigned long	transfer_cnt;
	unsigned long	burst_len;
	unsigned long	flush_en;
	unsigned long	src_type;
	unsigned long	src_asz;
	unsigned long	src_req;
	unsigned long	dst_type;
	unsigned long	dst_asz;
	unsigned long	dst_req;
} dmac_ch_cfg_t;

int dmac_init (unsigned long base, dmac_cfg_t *cfg);
int dmac_exit (void);
int dmac_rst (void);
int dmac_chan_alloc (unsigned long *id);
int dmac_chan_free (unsigned long *id);
int dmac_chan_rst (unsigned long id);
int dmac_chan_en (unsigned long id);
int dmac_chan_dis (unsigned long id);
int dmac_chan_cfg (unsigned long id, dmac_ch_cfg_t *cfg);
int dmac_chan_status (unsigned long id, unsigned long *status);

#endif
