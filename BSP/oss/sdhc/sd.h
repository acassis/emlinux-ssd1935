/*
@file		sd.h
@author		Ambat Sasi Nair
@copyright	Solomon Systech Ltd

Secure Digital definitions as per Part1 (Physical) and E1 (SDIO)
*/

#ifndef SD_SASI
#define SD_SASI


/* SD commands */
#define SD_C_GO_IDLE		0
#define SD_C_GET_OCR		1				/* MMC */
#define SD_C_ALL_GET_CID	2
#define SD_C_GET_RCA		3
#define SD_C_SET_RCA		SD_C_GET_RCA	/* MMC */
#define SD_C_SET_DSR		4
#define SD_C_IO_GET_OCR		5				/* SDIO */
#define SD_C_SEL			7
#define SD_C_GET_CSD		9
#define SD_C_GET_CID		10
#define SD_C_RD_STRM		11				/* MMC */
#define SD_C_ABORT			12
#define SD_C_GET_STA		13
#define SD_C_GO_INACTIVE	15
#define SD_C_SET_BLEN		16
#define SD_C_RD				17
#define SD_C_MRD			18
#define SD_C_WR_STRM		20				/* MMC */
#define SD_C_SET_BCNT		23				/* MMC */
#define SD_C_WR				24
#define SD_C_MWR			25
#define SD_C_PROG_CID		26
#define SD_C_PROG_CSD		27
#define SD_C_SET_WP			28
#define SD_C_CLR_WP			29
#define SD_C_GET_WP			30
#define SD_C_ERABL_BEG		32				/* SD */
#define SD_C_ERABL_END		33				/* SD */
#define SD_C_ERAGP_BEG		35				/* MMC */
#define SD_C_ERAGP_END		36				/* MMC */
#define SD_C_ERASE			38
#define SD_C_FASTIO			39				/* MMC IO */
#define SD_C_GO_IRQ			40				/* MMC IO */
#define SD_C_LOCK			42
#define SD_C_IO_DIR			52				/* SDIO */
/* could be an abort */
#define SD_C_IO_EXT			53				/* SDIO */
#define SD_C_APP			55
#define SD_C_GEN			56
/* Application-specific commands */
#define SD_AC_BUS_WIDTH		6
#define SD_AC_SDSTAT		13
#define SD_AC_GET_NUM_WR	22
#define SD_AC_SET_NUM_ERA	23
#define SD_AC_GET_OCR		41
#define SD_AC_SET_DET		42
#define SD_AC_GET_SCR		51


/* card state bits common to R1 & R6 */
#define SD_S_STA(r1)			((r1) & (0x0F << 9))
#define SD_S_ST_IDLE			(0x00 << 9)
#define SD_S_ST_RDY				(0x01 << 9)
#define SD_S_ST_ID				(0x02 << 9)
#define SD_S_ST_STBY			(0x03 << 9)
#define SD_S_ST_TX				(0x04 << 9)
#define SD_S_ST_DAT				(0x05 << 9)
#define SD_S_ST_RX				(0x06 << 9)
#define SD_S_ST_PRG				(0x07 << 9)
#define SD_S_ST_DIS				(0x08 << 9)
#define SD_S_ST_SDIO			(0x0F << 9)	/* R6: this is SDIO */
#define SD_S_STA_RDY			(1 << 8)
#define SD_S_STA_APP			(1 << 5)
#define SD_S_ERR_AKESEQ			(1 << 3)
/* card state bits returned in R1 */
#define SD_S_ERR_RANGE			(1 << 31)
#define SD_S_ERR_ADDR			(1 << 30)
#define SD_S_ERR_BLEN			(1 << 29)
#define SD_S_ERR_ERASEQ			(1 << 28)
#define SD_S_ERR_ERAPARM		(1 << 27)
#define SD_S_ERR_WP				(1 << 26)
#define SD_S_STA_LOCKED			(1 << 25)
#define SD_S_ERR_LOCK			(1 << 24)
#define SD_S_ERR_CRC			(1 << 23)
#define SD_S_ERR_CMD			(1 << 22)
#define SD_S_ERR_ECC			(1 << 21)
#define SD_S_ERR_CC				(1 << 20)
#define SD_S_ERR_UNKNOWN		(1 << 19)
#define SD_S_ERR_CIDCSD			(1 << 16)
#define SD_S_STA_ERAWP			(1 << 15)
#define SD_S_STA_NOECC			(1 << 14)
#define SD_S_STA_ERARST			(1 << 13)
#define SD_S_R1_ERR_MASK		0xFDF90008
/* card state bits returned in R6 */
#define SD_S_R6_ERR_CRC			(1 << 15)
#define SD_S_R6_ERR_CMD			(1 << 14)
#define SD_S_R6_ERR_UNKNOWN		(1 << 13)
#define SD_S_R6_ERR_MASK		0xE008

/* R5 bits */
#define SD_R5_MSK_STA			(3 << 20)
#define SD_R5_MSK_ERR			(0xCF << 16)
#define SD_R5_ERR_CRC			(1 << 23)
#define SD_R5_ERR_CMD			(1 << 22)
#define SD_R5_STA_TRN			(1 << 21)
#define SD_R5_STA_CMD			(1 << 20)
#define SD_R5_ERR_UNKNOWN		(1 << 19)
#define SD_R5_ERR_FNUM			(1 << 17)
#define SD_R5_ERR_RANGE			(1 << 16)
#define SD_R5_DAT(r5)			((r5 >> 8) & 0xff)

#define SD_OCR_V(dV)			(dV - 12)	/* dV = 16-35, 1.6V-3.5V */
#define SD_OCR_RDY				(1 << 31)
#define SDIO_OCR_FNMASK			(7 << 28)
#define SDIO_OCR_MEM			(1 << 27)


typedef struct
{
	uint8_t		date[2];
	uint8_t		ser[4];
	uint8_t		rev;
	char		prod[5];
	char		app[2];
	uint8_t		manu;
}
sd_cid_t;


typedef struct
{
	uint8_t		file;
	uint8_t		wr[2];
	uint8_t		era[2];
	uint8_t		vdd[2];
	uint8_t		size[2];
	uint8_t		ccbl[2];
	uint8_t		tran;
	uint8_t		nsac;
	uint8_t		taac;
}
sd_csd_t;

#define SD_CSD_ECC(csd)			((csd)->file & 3)
#define SD_CSD_ECC_NONE			0
#define SD_CSD_ECC_BCH			1
#define SD_CSD_FMT(csd)			(((csd)->file >> 2) & 3)
#define SD_CSC_FMT_HD			0
#define SD_CSC_FMT_FD			1
#define SD_CSC_FMT_UNI			2
#define SD_CSD_WPTEMP(csd)		((csd)->file & 0x10)
#define SD_CSD_WPPERM(csd)		((csd)->file & 0x20)
#define SD_CSD_FILE_OTP(csd)	((csd)->file & 0x40)
#define SD_CSD_FILE_GRP(csd)	((csd)->file & 0x80)
#define SD_CSD_WR_PARTIAL(csd)	((csd)->wr[0] & 0x20
#define SD_CSD_WR_LEN(csd)		(((csd)->wr[0] >> 6) | \
									(((csd)->wr[1] & 3) << 2))
#define SD_CSD_R2W(csd)			(((csd)->wr[1] >> 2) & 7)
#define SD_CSD_ECC_DEF(csd)		(((csd)->wr[1] >> 5) & 3)
#define SD_CSD_WP_GRPENA(csd)	((csd)->wr[1] & 0x80)
#define SD_CSD_WP_GRPSIZE(csd)	((csd)->era[0] & 0x7F)
#define SD_CSD_ERA_LEN(csd)		(((csd)->era[0] >> 7) | \
									(((csd)->era[1] & 0x3F) << 1))
#define SD_CSD_ERA_BLK(csd)		((csd)->era[1] & 0x40)
#define SD_CSD_SIZE_MULT(csd)	(((csd)->era[1] >> 7) | \
									(((csd)->vdd[0] & 3) << 1))
#define SD_CSD_VDD_WRMAX(csd)	(((csd)->vdd[0] >> 2) & 7)
#define SD_CSD_VDD_WRMIN(csd)	((csd)->vdd[0] >> 5)
#define SD_CSD_VDD_RDMAX(csd)	((csd)->vdd[1] & 7)
#define SD_CSD_VDD_RDMIN(csd)	(((csd)->vdd[1] >> 3) & 7)
#define SD_CSD_SIZE(csd)		(((csd)->vdd[1] >> 6) | \
									((csd)->size[0] << 2) | \
									(((csd)->size[1] & 0x0F) << 10))
#define SD_CSD_F_DSR(csd)		((csd)->size[1] & 0x10)
#define SD_CSD_F_RDALIGN(csd)	((csd)->size[1] & 0x20)
#define SD_CSD_F_WRALIGN(csd)	((csd)->size[1] & 0x40)
#define SD_CSD_F_RDPARTIAL(csd)	((csd)->size[1] & 0x80)
#define SD_CSD_RD_LEN(csd)		((csd)->ccbl[0] & 0x0F)
#define SD_CSD_CCC(csd)			(((csd)->ccbl[0] >> 4) | \
									(csd>->ccbl[1] << 4))

typedef struct
{
	uint32_t	_rsvmanu;
	uint16_t	_rsv;
	uint8_t		sup;
	uint8_t		ver;
}
sd_scr_t;

#define SD_SCR_SUP_ERASTA	0x80
#define SD_SCR_SUP_SECMASK	0x30
#define SD_SCR_SUP_SECSHFT	4
#define SD_SCR_SUP_SECNONE	0x00
#define SD_SCR_SUP_SEC1		0x10
#define SD_SCR_SUP_SEC2		0x20
#define SD_SCR_SUP_DAT4		0x04
#define SD_SCR_SUP_DAT1		0x01


typedef struct
{
	uint8_t	cmd;
	uint8_t	len;
	/* password follows */
}
sd_lock_t;

#define SD_LOCK_ERASE	8
#define SD_LOCK_LOCK	4
#define SD_LOCK_CLR		2
#define SD_LOCK_SET		1

/* SDIO CMD52/53 argument formatting */
#define SDIO_F_WR	0x80000000	/* write/read */
#define SDIO_F_INC	0x04000000	/* increment addr for every data byte */
#define SDIO_F_RAW	0x08000000	/* readback after write */
#define SDIO_FF_WRRD	(SDIO_F_WR | SDIO_F_RD)
#define SDIO_FF_INCW	(SDIO_F_WR | SDIO_F_INC)
#define SDIO_ARG(fn, reg, fl)		((fn << 28) | (reg << 9) | fl)


/* SDIO registers in function 0 space */

#define SDIO_CCCR_REV_SDIO		0x00		/* R/O */
#define SDIO_CCCR_REV_SD		0x01		/* R/O */
#define SDIO_CCCR_ENA			0x02
#define SDIO_CCCR_RDY			0x03		/* R/O */
#define SDIO_CCCR_IENA			0x04
#define SDIO_CCCR_IPEND			0x05		/* R/O */
#define SDIO_CCCR_ABORT			0x06		/* W/O */
#define SDIO_CCCR_BUS			0x07
#define SDIO_CCCR_CAPS			0x08
#define SDIO_CCCR_CIS			0x09		/* R/O 3 byte addr */
#define SDIO_CCCR_SUSP			0x0C
#define SDIO_CCCR_SEL			0x0D
#define SDIO_CCCR_EXEF			0x0E		/* valid only during suspend */
#define SDIO_CCCR_RDYF			0x0F		/* valid only during suspend */
#define SDIO_CCCR_BSIZE			0x10		/* 2 bytes */

#define SDIO_CCCR_ABORT_FNMASK	0x07
#define SDIO_CCCR_ABORT_FN(fn)	(fn)
#define SDIO_CCCR_ABORT_RST		0x08		/* W/O */

#define SDIO_CCCR_BUS_WIDE		0x02
#define SDIO_CCCR_BUS_ECSI		0x20
#define SDIO_CCCR_BUS_SCSI		0x40
#define SDIO_CCCR_BUS_CDDIS		0x80

#define SDIO_CCCR_CAPS_4BLS		0x80		/* 4bit? */
#define SDIO_CCCR_CAPS_LSC		0x40		/* low speed? */
#define SDIO_CCCR_CAPS_E4MI		0x20		/* R/W - ena IRQ in 4bit mode */
#define SDIO_CCCR_CAPS_S4MI		0x10		/* IRQ in 4 bit mode? */
#define SDIO_CCCR_CAPS_SBS		0x08		/* suspend-resume? */
#define SDIO_CCCR_CAPS_SRW		0x04		/* read-wait? */
#define SDIO_CCCR_CAPS_SMB		0x02		/* multi-block? */
#define SDIO_CCCR_CAPS_SDC		0x01		/* CMD52 while CMD53? */

#define SDIO_CCCR_SUSP_REQ		2
#define SDIO_CCCR_SUSP_STA		1			/* R/O */

#define SDIO_CCCR_SEL_FNMASK	0x07
#define SDIO_CCCR_SEL_MEM		0x08
#define SDIO_CCCR_SEL_DF		0x80		/* R/O - more data */

/* for all other SDIO CCCR registers */
#define SDIO_CCCR_FN(fn)		(1 << (fn))
#define SDIO_CCCR_IENA_EN		1			/* 0 -> all IRQ masked */
#define SDIO_CCCR_EXE_MEM		1

/* SDIO FBR registers where fn=1..7 */
#define SDIO_FBR_TYPE(fn)		((fn) << 8)
#define SDIO_FBR_CIS(fn)		(((fn) << 8) + 0x09)		/* 3B */
#define SDIO_FBR_CSA_ADR(fn)	(((fn) << 8) + 0x0C)		/* 3B */
#define SDIO_FBR_CSA_DAT(fn)	(((fn) << 8) + 0x0F)
#define SDIO_FBR_BSIZE(fn)		(((fn) << 8) + 0x10)		/* 2B */

/* SDIO FBR type register values */
#define SDIO_FBR_TYPE_FNMASK	0x0F
#define SDIO_FBR_TYPE_FN_UART	0x01
#define SDIO_FBR_TYPE_FN_BTHCI	0x02
#define SDIO_FBR_TYPE_FN_BTMDM	0x03
#define SDIO_FBR_TYPE_FN_GPS	0x04
#define SDIO_FBR_TYPE_FN_CAM	0x05
#define SDIO_FBR_TYPE_FN_PHS	0x06
#define SDIO_FBR_TYPE_CSA_SUP	0x40
#define SDIO_FBR_TYPE_CSA_EN	0x80


#endif

