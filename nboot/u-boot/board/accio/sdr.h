#ifndef __SDR_H__
#define __SDR_H__

#define SDR_REG_BASE	0x08000000
#define SDR_CSD0_BASE	0x50000000
#define SDR_CSD1_BASE 	0x90000000


typedef struct _sdr_cfg_t 
{
	int cs;		/* cs=0 or 1 */
	int row;	/* 0..3 -> 11:14 */
	int col;	/* 0..3 -> 7:11 */
	int bl;		/* burst len, 2^^(bl), bl=4 =>full page */
	int cas;	/* 1,2,3 */
	int dsz;	/* 0=16-bit,1=32-bit */
}
sdr_cfg_t;

int sdr_init (uint32_t base);
int sdr_reset (void);
int sdr_enable (void);
int sdr_power_up (void);
int sdr_start (sdr_cfg_t *cfg);
int sdr_cfg (sdr_cfg_t *cfg);

#endif	// __SDR_H__
