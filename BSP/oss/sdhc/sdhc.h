/*

File		sdhc.h
Author		Ambat Sasi Nair
Copyright	Solomon Systech Ltd

Secure Digital Host Controller proprietary API & definitions

*/

#ifndef SDHC_SASI
#define SDHC_SASI


typedef struct
{
	uint8_t		*buf;	/* bufferaddress for data(dma/virtual) & R2(virtual) */
	uint16_t	count;	/* # blocks to xfer */
	uint16_t	blk;	/* block size */
	uint16_t	actual;	/* # blocks successfully transferred */
}
sd_dat_t, *sd_dat_p;

typedef struct
{
	uint32_t	arg;	/* SD spec argument */
	uint32_t	rsp;	/* SD spec response R1, R3, R4, or R5 */
	void		*ctx;	/* caller context input */
	int			rt;		/* return SDHC_RET_xxx */
	sd_dat_p	dat;	/* caller struct buf for data or response R2 */
	uint16_t	tout;	/* data/busy timeout in ms | SDCMD_TOUT_WAIT */
	uint8_t		cmd;	/* SD spec command */
	uint8_t		flag;	/* flag SDCMD_F_xxx */
}
sd_cmd_t, *sd_cmd_p;

#define SDCMD_F_DMA			0x80
#define SDCMD_F_MULTI		0x40
#define SDCMD_F_DAT			0x20
#define SDCMD_F_WR			0x10
#define SDCMD_F_STOP		0x08
#define SDCMD_F_RMASK		0x07
#define SDCMD_F_RNONE		0x00
#define SDCMD_F_R1			0x01
#define SDCMD_F_R2			0x02
#define SDCMD_F_R3			0x03
#define SDCMD_F_R1B			0x05
#define SDCMD_F_R4			SDCMD_F_R3
#define SDCMD_F_R5			SDCMD_F_R1
#define SDCMD_F_R5B			SDCMD_F_R1B
#define SDCMD_F_R6			SDCMD_F_R1

#define SDHC_EVT_REM	((sd_cmd_p)0)
#define SDHC_EVT_INS	((sd_cmd_p)1)
#define SDHC_EVT_IO		((sd_cmd_p)2)
#define SDHC_EVT_MAX	((sd_cmd_p)3)

typedef struct
{
/* set by client before sdhc_init */
	uint32_t	r;					/* register base host address */

	void		*ctx;
	void		(*evt)(void *ctx, sd_cmd_p);

	/* local platform settings - set before init, can be overwritten by 
	   driver during init, readonly after init
	*/
	uint16_t	baseclk;			/* SDHC base clock in 100KHz */
	uint32_t	toutclk;			/* SDHC data timeout clock in KHz */

/* opaque */
	/* sdhc caps */
	uint32_t	ocr;				/* supported voltages in SD OCR format */
	uint8_t		fhspeed : 1;		/* 25-50MHz capable */
	uint8_t		fdma : 1;			/* bus master capabile */
	uint8_t		fsuspend : 1;		/* suspend/resume capabile */
	uint8_t		maxblksz : 2;		/* maximum block size 0-512,1-1024,2-2048 */
	/* card state */
	uint8_t		fwp : 1;			/* card write protected? */
	uint8_t		fins : 1;			/* card inserted? */
	uint16_t	ver;
	/* internal working */
	sd_cmd_t	*cmd;
	sd_cmd_t	*dat;
}
sdhc_t, *sdhc_p;

#define SDHC_S_NONE			0
#define SDHC_S_IDLE			1
#define SDHC_S_DET			2
#define SDHC_S_XFER			3

#define SDHC_F_DMA			1


/* API returns */
#define SDHC_RET_NONE		0		/* executed correctly */
#define SDHC_RET_PARM		-1		/* API parameters wrong */
#define SDHC_RET_BUSY		-2		/* h/w is busy, API not executed */
#define SDHC_RET_TOUT		-3		/* unexpected timeout */
#define SDHC_RET_CMD_TOUT	-4		/* response timed out */
#define SDHC_RET_CMD_LINE	-5		/* error in CMD line */
#define SDHC_RET_DAT_TOUT	-6		/* data timed out */
#define SDHC_RET_DAT_LINE	-7		/* error in DAT line */
#define SDHC_RET_REM		-8		/* surprise removal */
#define SDHC_RET_ABORT		-9		/* aborted during error-recovery */
#define SDHC_RET_DAT_OVR	-10		/* data overrun */


int sdhc_init(sdhc_t *t);
/**<
resets sdhc for operation
@param[in,out] t	context (r set)
@return SDHC_RET_XXX
*/


void sdhc_exit(sdhc_t *t);
/**<
resets sdhc for shutdown
@param[in,out] t	context (r set)
*/


int	sdhc_set_clk(sdhc_t *t, int clk);
/**<
enables/disables SDCLK
@param[in] t	context
@param[in] clk	frequency in 100KHz units (0 disables)
@return			SDHC_RET_XXX
*/

int	sdhc_stop_clk(sdhc_t *t);
#define sdhc_stop_clk(t)	sdhc_set_clk(t, 0)


int	sdhc_set_pwr(sdhc_t *t, int pwr);
/**<
enables/disables SDVDD
@param[in] t	context
@param[in] pwr	SD_OCR_V(xx) (0 disables, -1 max)
@return SDHC_RET_XXX
*/

int	sdhc_stop_pwr(sdhc_t *t);
#define sdhc_stop_pwr(t)	sdhc_set_pwr(t, 0)


void sdhc_set_width(sdhc_t *t, int wide);
/**<
enables/disables wide bus (nibble) mode
@param[in] t	context
@param[in] wide	0=single else=nibble
*/


void sdhc_set_led(sdhc_t *t, int on);
/**<
enables/disables LED
@param[in] t	context
@param[in] on	0=disable else=enable
*/


int	sdhc_cmd(sdhc_t *t, sd_cmd_t *cmd);
/**<
performs SD/MMC/SDIO transaction
@param[in] t	context
@param[in] cmd	transaction info
@return SDHC_RET_XXX
*/


void  sdhc_ioirq_ena(sdhc_t *t, int ena);
/**<
enables/disables SDIO irq
@param[in] t	context
@param[in] ena	1 - enable, 0 - disable
*/


int sdhc_isr(sdhc_t *t);
/**<
performs ISR handling
@param[in] t	context
@return 0 if not handled
*/


void  sdhc_flush(sdhc_t *t);
/**<
completes all pending requests
@param[in] t	context
@remark must be called with sdhc interrupt off
*/


int  sdhc_get_pwr(sdhc_t *t);
/**<
get pwr supported by host controller
@param[in] t	context
@return OCR format
*/


int  sdhc_get_maxblk(sdhc_t *t);
/**<
get maximum block size in bytes supported by host controller
@param[in] t	context
@return 512, 1024, or 2048
*/


int  sdhc_has_hspeed(sdhc_t *t);
/**<
check if high speed is supported by host controller
@param[in] t	context
@return 0 if not supported
*/


int  sdhc_has_dma(sdhc_t *t);
/**<
check if bus mastering is supported by host controller
@param[in] t	context
@return 0 if not supported
*/


int  sdhc_has_suspend(sdhc_t *t);
/**<
check if suspend is supported by host controller
@param[in] t	context
@return 0 if not supported
*/


int  sdhc_is_wp(sdhc_t *t);
/**<
check if card is mechanically write protected
@param[in] t	context
@return 0 if not write protected
*/


int  sdhc_is_ins(sdhc_t *t);
/**<
check if card is inserted
@param[in] t	context
@return 0 if not inserted
*/


/* implementation */
#define sdhc_get_pwr(t)		((sdhc_t *)t)->ocr
#define sdhc_get_maxblk(t)	(1 << (((sdhc_t *)t)->maxblksz + 9))
#define sdhc_has_hspeed(t)	((sdhc_t *)t)->fhspeed
#define sdhc_has_dma(t)		((sdhc_t *)t)->fdma
#define sdhc_has_suspend(t)	((sdhc_t *)t)->fsuspend
#define sdhc_is_wp(t)		((sdhc_t *)t)->fwp
#define sdhc_is_in(t)		((sdhc_t *)t)->fins


#endif

