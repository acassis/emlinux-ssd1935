#ifndef SDHCR_SASI
#define SDHCR_SASI


#pragma pack(1)

/* hardware map */
typedef struct
{
	uint32_t	dma;
	uint16_t	blk;
	uint16_t	count;
	uint32_t	arg;
	uint16_t	mode;
	uint16_t	cmd;
	uint32_t	rsp[4];
	uint32_t	port;
	uint32_t	sta;
	uint8_t		host;
	uint8_t		pwr;
	uint8_t		gap;
	uint8_t		wake;
	uint8_t		clk;
	uint8_t		frqdiv;
	uint8_t		tout;
	uint8_t		rst;
	uint16_t	ista;
	uint16_t	esta;
	uint16_t	istaen;
	uint16_t	estaen;
	uint16_t	isigen;
	uint16_t	esigen;
	uint8_t		a12err;
	uint8_t		_rsv[3];
	uint8_t		toutcaps;
	uint8_t		clkcaps;		/* 10-63 MHz */
	uint8_t		fncaps;
	uint8_t		pwrcaps;
	uint32_t	_rsv2;
	uint8_t		maxi18v;		/* 1-255:4mA-1020mA, 0:other method */
	uint8_t		maxi30v;
	uint8_t		maxi33v;
	uint8_t		_rsv3;
	uint32_t	_rsv4[44];
	uint16_t	slotirq;
	uint16_t	hcver;
}
volatile sdhcr_t, *sdhcr_p;

#pragma pack()


#define SDHC_BLK_ALIGN_MASK	0x7000
#define SDHC_BLK_ALIGN_SHFT	12
#define SDHC_BLK_ALIGN_4K	0x0000
#define SDHC_BLK_ALIGN_8K	0x1000
#define SDHC_BLK_ALIGN_16K	0x2000
#define SDHC_BLK_ALIGN_32K	0x3000
#define SDHC_BLK_ALIGN_64K	0x4000
#define SDHC_BLK_ALIGN_128K	0x5000
#define SDHC_BLK_ALIGN_256K	0x6000
#define SDHC_BLK_ALIGN_512K	0x7000
#define SDHC_BLK_SIZE_MASK	0x0FFF
#define SDHC_BLK_SIZE_SHFT	0

#define SDHC_MODE_DMA			0x01
#define SDHC_MODE_COUNT			0x02
#define SDHC_MODE_AUTOSTOP		0x04
#define SDHC_MODE_RD			0x10
#define SDHC_MODE_MULTI			0x20

#define SDHC_CMD_IDX_MASK		0x3F00
#define SDHC_CMD_IDX_SHFT		8
#define SDHC_CMD_TYP_MASK		0xC0
#define SDHC_CMD_TYP_SHFT		6
#define SDHC_CMD_TYP_NORMAL		(0 << 6)
#define SDHC_CMD_TYP_SUSPEND	(1 << 6)
#define SDHC_CMD_TYP_RESUME		(2 << 6)
#define SDHC_CMD_TYP_ABORT		(3 << 6)
#define SDHC_CMD_DAT			0x20
#define SDHC_CMD_IDX			0x10
#define SDHC_CMD_CRC			0x08
#define SDHC_CMD_RSP_MASK		0x03
#define SDHC_CMD_RSP_SHFT		0
#define SDHC_CMD_RSP_NONE		0
#define SDHC_CMD_RSP_136		1
#define SDHC_CMD_RSP_48			2
#define SDHC_CMD_RSP_BUSY		3

#define SDHC_STA_CMD_INHIBIT	1
#define SDHC_STA_DAT_INHIBIT	2
#define SDHC_STA_DAT_ACTIVE		4
#define SDHC_STA_WR_ACTIVE		0x100
#define SDHC_STA_RD_ACTIVE		0x200
#define SDHC_STA_WRDY			0x400
#define SDHC_STA_RRDY			0x800
#define SDHC_STA_INS			0x10000
#define SDHC_STA_INS_STABLE		0x20000
/* line level */
#define SDHC_STA_DET			0x40000
#define SDHC_STA_WP				0x80000
#define SDHC_STA_DAT0			0x100000
#define SDHC_STA_DAT1			0x200000
#define SDHC_STA_DAT2			0x400000
#define SDHC_STA_DAT3			0x800000
#define SDHC_STA_DAT			0xF00000
#define SDHC_STA_CMD			0x1000000

#define SDHC_HOST_LED		1
#define SDHC_HOST_WIDE		2
#define SDHC_HOST_HSPEED	4

#define SDHC_PWR_ENA		0x01
#define SDHC_PWR_VMASK		0x0E
#define SDHC_PWR_VSHFT		0
#define SDHC_PWR_V18		(5 << 1)
#define SDHC_PWR_V30		(6 << 1)
#define SDHC_PWR_V33		(7 << 1)

#define SDHC_GAP_STOP		1
#define SDHC_GAP_RESUME		2
#define SDHC_GAP_RDWAIT		4
#define SDHC_GAP_INTR		8

#define SDHC_WAKE_CARD		1
#define SDHC_WAKE_INS		2
#define SDHC_WAKE_REM		4

#define SDHC_CLK_ENA		4
#define SDHC_CLK_STABLE		2
#define SDHC_CLK_INT		1

#define SDHC_FRQDIV(x)		(x >> 1)	/* x = 2^^y where y = 0..8 */

#define SDHC_TOUT_V(x)		(x - 13)	/* x = 13..27, (TMCLK * 2^^x) */

#define SDHC_RST_DAT		4
#define SDHC_RST_CMD		2
#define SDHC_RST_ALL		1

#define SDHC_IRQ_ERR	0x8000
#define SDHC_IRQ_IO		0x100
#define SDHC_IRQ_REM	0x80
#define SDHC_IRQ_INS	0x40
#define SDHC_IRQ_RD		0x20
#define SDHC_IRQ_WR		0x10
#define SDHC_IRQ_DMA	0x08
#define SDHC_IRQ_GAP	0x04
#define SDHC_IRQ_DAT	0x02
#define SDHC_IRQ_CMD	0x01

#define SDHC_ERR_ALL_MASK	0x1FF
#define SDHC_ERR_A12		0x100
#define SDHC_ERR_CURRENT	0x80
/* dat errors */
#define SDHC_ERR_DAT_MASK	0x70
#define SDHC_ERR_DAT_SHFT	4
#define SDHC_ERR_DAT_END	0x40
#define SDHC_ERR_DAT_CRC	0x20
#define SDHC_ERR_DAT_TOUT	0x10
/* rsp errors */
#define SDHC_ERR_RSP_MASK	0x0F
#define SDHC_ERR_RSP_SHFT	0
#define SDHC_ERR_RSP_IDX	0x08
#define SDHC_ERR_RSP_END	0x04
#define SDHC_ERR_RSP_CRC	0x02
#define SDHC_ERR_RSP_TOUT	0x01

#define SDHC_A12ERR_EXEC	0x01
#define SDHC_A12ERR_TOUT	0x02
#define SDHC_A12ERR_CRC		0x04
#define SDHC_A12ERR_END		0x08
#define SDHC_A12ERR_IDX		0x10
#define SDHC_A12ERR_CMD		0x80

#define SDHC_PWRCAPS_V18		4
#define SDHC_PWRCAPS_V30		2
#define SDHC_PWRCAPS_V33		1
#define SDHC_FNCAPS_SUSPEND		0x80
#define SDHC_FNCAPS_DMA			0x40
#define SDHC_FNCAPS_HSPEED		0x20
#define SDHC_FNCAPS_BMASK		3
#define SDHC_FNCAPS_BSHFT		0
#define SDHC_FNCAPS_B2048		2
#define SDHC_FNCAPS_B1024		1
#define SDHC_FNCAPS_B512		0
#define SDHC_TOUTCAPS_MHZ		0x80
#define SDHC_TOUTCAPS_MASK		0x3F		/* KHz/MHz, 1-63 */
#define SDHC_TOUTCAPS_SHFT		0

#define SDHC_MAXI(mA)			((mA) >> 2)

/* pre-encoded commands */
#define SDHC_C_GO_IDLE		0 
#define SDHC_C_GET_OCR		(((1 << 8) | 0x02))				/* MMC */
#define SDHC_C_ALL_GET_CID	((2 << 8) | 0x09)
#define SDHC_C_GET_RCA		((3 << 8) | 0x1a)
#define SDHC_C_SET_RCA		SDHC_C_GET_RCA					/* MMC */
#define SDHC_C_SET_DSR		((4 << 8))
#define SDHC_C_IO_GET_OCR	((5 << 8) | 0x02)				/* SDIO */
#define SDHC_C_SEL			((7 << 8) | 0x1b)
#define SDHC_C_GET_CSD		((9 << 8) | 0x09)
#define SDHC_C_GET_CID		((10 << 8) | 0x09)
#define SDHC_C_RD_STRM		((11 << 8) | 0x1a | 0x20)		/* MMC */
#define SDHC_C_ABORT		((12 << 8) | 0x1b | 0xC0)
#define SDHC_C_GET_STA		((13 << 8) | 0x1a)
#define SDHC_C_GO_INACTIVE	((15 << 8))
#define SDHC_C_SET_BLEN		((16 << 8) | 0x1a)
#define SDHC_C_RD			((17 << 8) | 0x1a | 0x20)
#define SDHC_C_MRD			((18 << 8) | 0x1a | 0x20 | 0x4000)
#define SDHC_C_WR_STRM		((20 << 8) | 0x1a | 0x20 | 4)	/* MMC */
#define SDHC_C_SET_BCNT		((23 << 8) | 0x1a)				/* MMC */
#define SDHC_C_WR			((24 << 8) | 0x1a | 0x20 | 4)
#define SDHC_C_MWR			((25 << 8) | 0x1a | 0x20 | 0x4004)
#define SDHC_C_PROG_CID		((26 << 8) | 0x1a | 0x20 | 4)
#define SDHC_C_PROG_CSD		((27 << 8) | 0x1a | 0x20 | 4)
#define SDHC_C_SET_WP		((28 << 8) | 0x1b)
#define SDHC_C_CLR_WP		((29 << 8) | 0x1b)
#define SDHC_C_GET_WP		((30 << 8) | 0x1a | 0x20)
#define SDHC_C_ERABL_BEG	((32 << 8) | 0x1a)				/* SD */
#define SDHC_C_ERABL_END	((33 << 8) | 0x1a)				/* SD */
#define SDHC_C_ERAGP_BEG	((35 << 8) | 0x1a)				/* MMC */
#define SDHC_C_ERAGP_END	((36 << 8) | 0x1a)				/* MMC */
#define SDHC_C_ERASE		((38 << 8) | 0x1b)
#define SDHC_C_FASTIO		((39 << 8) | 0x1a)				/* MMC IO */
#define SDHC_C_GO_IRQ		((40 << 8) | 0x1a)				/* MMC IO */
#define SDHC_C_LOCK			((42 << 8) | 0x1a | 0x20 | 4)
#define SDHC_C_IO_DIR		((52 << 8) | 0x1a)				/* SDIO */
/* could be an abort */
#define SDHC_C_IO_EXT		((53 << 8) | 0x1a | 0x20)		/* SDIO */
#define SDHC_C_APP			((55 << 8) | 0x1a)
#define SDHC_C_GEN			((56 << 8) | 0x1a | 0x20)

/* Application-specific commands pre-encoded */
#define SDHC_AC_BUS_WIDTH	((6 << 8) | 0x1a)
#define SDHC_AC_SDSTAT		((13 << 8) | 0x1a)
#define SDHC_AC_GET_NUM_WR	((22 << 8) | 0x1a | 0x20)
#define SDHC_AC_SET_NUM_ERA	((23 << 8) | 0x1a)
#define SDHC_AC_GET_OCR		((41 << 8) | 0x02)
#define SDHC_AC_SET_DET		((42 << 8) | 0x1a)
#define SDHC_AC_GET_SCR		((51 << 8) | 0x1a | 0x20)

/* Security commands */

#endif

