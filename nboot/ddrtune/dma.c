#include "platform.h"
#include "mytypes.h"
//#include "debug.h"
#include "dma.h"
#include "dmacapi.h"
#include "uart.h"

#ifdef RUN_BENCHMARK
#include "gpt.h"
#endif

#define dbg 

#define MAX_CHAN	8
#define MAX_WAIT_DLY	0x1000000

int dma_test_1_ex_bt (
	unsigned long src, unsigned long dst, 
	unsigned long *len, unsigned long *reading, unsigned long bt)
{
	dmac_cfg_t	cfg;
	dmac_ch_cfg_t	chan_cfg[MAX_CHAN];
	unsigned long	status = 0;
	unsigned long	wait_cnt = 0;

	cfg.pri = DMAC_PRI_ROBIN;		// 0=round robin, 1 = fixed priority
	cfg.burst_type = (bt == -1UL ? DMAC_BT_INCR16U : bt);
	cfg.wait_dly = 0;
	cfg.n_ch = 0;
	cfg.fifo_sz = 0;
	cfg.id = 0;

	if (*len == 16*MB)
		*len -= 1;

	switch (cfg.burst_type)
	{
		case DMAC_BT_INCR4:
			// round of to multiple of 4x8 bytes
			*len = *len & ~((0x04 << 3) - 1); 
			break;
		case DMAC_BT_INCR8:
			// round of to multiple of 8x8 bytes
			*len = *len & ~((0x08 << 3) - 1); 
			break;
		case DMAC_BT_INCR16:
			// round of to multiple of 16x8 bytes
			*len = *len & ~((0x10 << 3) - 1); 
			break;
		case DMAC_BT_SINGLE:
		case DMAC_BT_INCR4U:
		case DMAC_BT_INCR8U:
		case DMAC_BT_INCR16U:
		default: 
			break;
	}

	chan_cfg[0].src_addr = src;
	chan_cfg[0].dst_addr = dst;
	chan_cfg[0].transfer_cnt = *len;
	chan_cfg[0].burst_len = 0;		// ignored for memory type
	chan_cfg[0].flush_en = 0;		// ignored for memory type
	chan_cfg[0].src_type = DMAC_TYP_MEM;
	chan_cfg[0].src_asz = 0;		// ignored for memory type
	chan_cfg[0].src_req = 0;		// ignored for memory type
	chan_cfg[0].dst_type = DMAC_TYP_MEM;
	chan_cfg[0].dst_req = 0;		// ignored for memory type
	chan_cfg[0].dst_asz = 0;		// ignored for memory type

	if (dmac_init (DMAC_BASE, &cfg) < 0)
	{
		dbg ("dma: init failed\n");
		return -1;
	}

	dbg ("dma: id = %08lx, fifo_sz = %08lx, n_ch = %lu\n",
		cfg.id, cfg.fifo_sz, cfg.n_ch);

	if (dmac_chan_cfg (0, &chan_cfg[0]) < 0)
	{
		dbg ("dma: chan[%lu] cfg failed\n", 0);
	}

	wait_cnt = 0;
	status = 0;

#ifdef RUN_BENCHMARK
	if (reading)
		gpt_start ();
#endif

	if (dmac_chan_en (0) < 0)
	{
		dbg ("dma: chan en failed\n");
		return -1;
	}
#if RUN_BENCHMARK
	do {
		dmac_chan_status (0, &status);
	} while (status != DMAC_STAT_DONE);

	if (reading)
		gpt_stop (reading);
#else
	do {
		dmac_chan_status (0, &status);
		wait_cnt++;
	} while ((status != DMAC_STAT_DONE) && (wait_cnt < MAX_WAIT_DLY));
#endif

	dbg ("dmac_chan[0]: status=%08lx\n", status);

	dmac_chan_rst (0);
	dmac_exit ();

	if (wait_cnt == MAX_WAIT_DLY)
		return -1;

	return 0;
}

int dma_test_2_ex_bt (unsigned long src1, unsigned long dst1, 
	unsigned long src2, unsigned long dst2, 
	unsigned long *len, unsigned long *reading, unsigned long bt)
{
	dmac_cfg_t	cfg;
	dmac_ch_cfg_t	chan_cfg[MAX_CHAN];
	unsigned long	status1 = 0;
	unsigned long	status2 = 0;
	unsigned long	wait_cnt = 0;

	cfg.pri = DMAC_PRI_ROBIN;		// 0=round robin, 1 = fixed priority
	cfg.burst_type = (bt == -1UL ? DMAC_BT_INCR16U : bt);
	cfg.wait_dly = 0;
	cfg.n_ch = 0;
	cfg.fifo_sz = 0;
	cfg.id = 0;

	if (*len == 16*MB)
		*len -= 1;

	switch (cfg.burst_type)
	{
		case DMAC_BT_INCR4:
			// round of to multiple of 4x8 bytes
			*len = *len & ~((0x04 << 3) - 1); 
			break;
		case DMAC_BT_INCR8:
			// round of to multiple of 8x8 bytes
			*len = *len & ~((0x08 << 3) - 1); 
			break;
		case DMAC_BT_INCR16:
			// round of to multiple of 16x8 bytes
			*len = *len & ~((0x10 << 3) - 1); 
			break;
		case DMAC_BT_SINGLE:
		case DMAC_BT_INCR4U:
		case DMAC_BT_INCR8U:
		case DMAC_BT_INCR16U:
		default: 
			break;
	}

	chan_cfg[0].src_addr = src1;
	chan_cfg[0].dst_addr = dst1;
	chan_cfg[0].transfer_cnt = *len;
	chan_cfg[0].burst_len = 0;		// not ignored for memory type
	chan_cfg[0].flush_en = 0;		// ignored for memory type
	chan_cfg[0].src_type = DMAC_TYP_MEM;
	chan_cfg[0].src_asz = 0;		// ignored for memory type
	chan_cfg[0].src_req = 0;		// ignored for memory type
	chan_cfg[0].dst_type = DMAC_TYP_MEM;
	chan_cfg[0].dst_req = 0;		// ignored for memory type
	chan_cfg[0].dst_asz = 0;		// ignored for memory type

	chan_cfg[1].src_addr = src2;
	chan_cfg[1].dst_addr = dst2;
	chan_cfg[1].transfer_cnt = *len;
	chan_cfg[1].burst_len = 0;		// not ignored for memory type
	chan_cfg[1].flush_en = 0;		// ignored for memory type
	chan_cfg[1].src_type = DMAC_TYP_MEM;
	chan_cfg[1].src_asz = 0;		// ignored for memory type
	chan_cfg[1].src_req = 0;		// ignored for memory type
	chan_cfg[1].dst_type = DMAC_TYP_MEM;
	chan_cfg[1].dst_req = 0;		// ignored for memory type
	chan_cfg[1].dst_asz = 0;		// ignored for memory type

	if (dmac_init (DMAC_BASE, &cfg) < 0)
	{
		dbg ("dma: init failed\n");
		return -1;
	}

	dbg ("dma: id = %08lx, fifo_sz = %08lx, n_ch = %lu\n",
		cfg.id, cfg.fifo_sz, cfg.n_ch);

	if (dmac_chan_cfg (0, &chan_cfg[0]) < 0)
	{
		dbg ("dma: chan[%lu] cfg failed\n", 0);
	}

	if (dmac_chan_cfg (1, &chan_cfg[1]) < 0)
	{
		dbg ("dma: chan[%lu] cfg failed\n", 1);
	}

#ifdef RUN_BENCHMARK
	if (reading)
		gpt_start ();
#endif

	if (dmac_chan_en (0) < 0)
	{
		return -1;
	}

	if (dmac_chan_en (1) < 0)
	{
		return -1;
	}

	wait_cnt = 0;
	status1 = status2 = 0;
	do {
		dmac_chan_status (0, &status1);
		dmac_chan_status (1, &status2);
		wait_cnt++;
	} while (
		((status1 != DMAC_STAT_DONE) || (status2 != DMAC_STAT_DONE)) && (wait_cnt < MAX_WAIT_DLY));

#ifdef RUN_BENCHMARK
	if (reading)
		gpt_stop (reading);
#endif

	dbg ("dmac_chan[0]: status1=%08lx\n", status1);
	dbg ("dmac_chan[1]: status2=%08lx\n", status2);

	dmac_chan_rst (0); dmac_chan_rst(1);
	dmac_exit ();
	
	if (wait_cnt == MAX_WAIT_DLY)
	{
		dbg ("wait_cnt == MAX_WAIT_DLY\n");
		return -1;
	}

	return 0;
}

/* end */
