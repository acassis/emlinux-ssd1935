/* NFC registers */
#pragma pack(4)

typedef struct
{
	uint32_t	IDR;	/**< 0x00 device identification register */
	uint32_t	CAPR;	/**< 0x04 capabilities register */
	uint32_t	CFGR1;	/**< 0x08 configuration register 1 */
	uint32_t	CFGR2;	/**< 0x0c configuration register 2 */
	uint32_t	HADDR1;	/**< 0x10 high address 1 register */
	uint32_t	ADDR1;	/**< 0x14 address 1 register */
	uint32_t	HADDR2;	/**< 0x18 high address 2 register */
	uint32_t	ADDR2;	/**< 0x1c address 2 register */
	uint32_t	CMDR;	/**< 0x20 command register */
	uint32_t	CTLR;	/**< 0x24 control register */
	uint32_t	IER;	/**< 0x28 interrupt enable register */
	uint32_t	ISR;	/**< 0x2c interrupt status register */
	uint32_t	FSCR;	/**< 0x30 flash status control register */
	uint32_t	FSR;	/**< 0x34 flash status register */
	uint32_t	DIOR;	/**< 0x38 data IO register */
	uint32_t	RSV1;	/**< 0x3c reserved */
	uint32_t	ECTRL;	/**< 0x40 ECC control register */
	uint32_t	TECC4;	/**< 0x44 ECC TECC4 control register */
	uint32_t	TECC8;	/**< 0x48 ECC TECC8 control registe */
	uint32_t	RSV2;	/**< 0x4c reserved */
	uint32_t	ECCR;	/**< 0x50 ECC register */
	uint32_t	TECC4R;	/**< 0x54 ECC4 error-count register */
	uint32_t	TECC8R;	/**< 0x58 ECC4 error-count register */
	uint32_t	RSV3;	/**< 0x5c reserved */
	uint32_t	TECCR0;	/**< 0x60 Toshiba ECC0 register */
	uint32_t	TECCR1;	/**< 0x64 Toshiba ECC1 register */
	uint32_t	TECCR2;	/**< 0x68 Toshiba ECC2 register */
	uint32_t	TECCR3;	/**< 0x6c Toshiba ECC3 register */
	uint32_t	TECCR4;	/**< 0x70 Toshiba ECC4 register */
	uint32_t	RSV4[35]; /**< 0x74 - 0x100 reserved */
	uint32_t	TECC4DBG;	/**< 0x100 TECC4 debug register */
	uint32_t	TECC8DBG;	/**< 0x104 TECC8 debug register */
	uint32_t	DBG0;		/**< 0x108 debug 0 register */
	uint32_t	DBG1;		/**< 0x10c debug 1 register */
	uint32_t	RSV5[956];	/**< 0x110 - 0x1000 reserved */
	uint32_t	BUF[4352];		/**< data buffer */
}
volatile nfcr_t, *nfcr_p;
#pragma pack()

#define	NFC_BUFFER		0x1000
#define	NFC_BUF_SIZE	0x1100

/** device identification register */
#define	NFC_IDR_CLID(d)		(d & 0xFFFF0000) >> 16
#define	NFC_IDR_DSGNR(d)	(d & 0xFC00) >> 10
#define	NFC_IDR_MAJ(d)		(d & 0x3C0) >> 6
#define	NFC_IDR_MIN(d)		(d & 0x3F)
#define	NFC_PCI_CLASS		0xAC8F

/** capabilities register */
#define	NFC_CAPR_BUFSZ		3

/** configuration register 1 */
#define	NFC_CFGR1_ROWSZ		0x1F000000
#define	NFC_CFGR1_COLSZ		0x1F0000
#define	NFC_CFGR1_BLKSZ		0x7000
#define	NFC_CFGR1_BUSWIDTH	0x800
#define	NFC_CFGR1_PGSZ		0x300
#define	NFC_CFGR1_SEQ		0x8
#define	NFC_CFGR1_RST		0x2
#define	NFC_CFGR1_EN		0x1

/** configuration register 2 */
#define	NFC_CFGR2_TWEH	0xF0000000
#define	NFC_CFGR2_TWEL	0xF000000
#define	NFC_CFGR2_TREH	0xF00000
#define	NFC_CFGR2_TREL	0xF0000
#define	NFC_CFGR2_TACLH	0xF000
#define	NFC_CFGR2_TACLS	0xF00
#define	NFC_CFGR2_TRWSP	0xF0
#define	NFC_CFGR2_TWEBUSY	0xF

/** high address registers */
#define	NFC_HADDR_ADDR	0xFF

/** command register */
#define	NFC_CMDR(c1, c2, c3, c4)	((c4 << 24) | (c3 << 16) | (c3 << 8) | c1)

/** control register */
#define	NFC_CTLR_ECCAUTO	0x10
#define	NFC_CTLR_ECCEN		8
#define	NFC_CTLR_STATREP	4
#define	NFC_CTLR_STAT		2
#define	NFC_CTLR_GO			1
#define NFC_CTLR_COUNT(n)	(n << 16)
#define NFC_CTLR_CHIP(n)	(((n) & 7) << 5)
#define NFC_CTLR_CYCLE(n)	(((n) & 7) << 12)
#define	NFC_CTLR_OP_RST		(0 << 8)
#define NFC_CTLR_OP_ID		(1 << 8)
#define NFC_CTLR_OP_STAT	(2 << 8)
#define NFC_CTLR_OP_READ	(3 << 8)
#define NFC_CTLR_OP_PROG	(4 << 8)
#define NFC_CTLR_OP_ERASE	(5 << 8)

/** interrupt enable register */
#define	NFC_IER_ERR		0x80
#define	NFC_IER_STANZ	0x40
#define	NFC_IER_WAIT	0x4
#define	NFC_IER_RDY		0x2
#define	NFC_IER_CMP		0x1
#define	NFC_IER_ALL		0xC7

/** interrupt status register */
#define	NFC_ISR_ECC3	0xC000
#define	NFC_ISR_ECC2	0x3000
#define	NFC_ISR_ECC1	0xC00
#define	NFC_ISR_ECC0	0x300
#define	NFC_ISR_ERR		0x80
#define	NFC_ISR_STANZ	0x40
#define	NFC_ISR_ECCERR	0x10
#define	NFC_ISR_DRDY	0x8
#define	NFC_ISR_RAWRDY	0x4
#define	NFC_ISR_RDY		0x2
#define	NFC_ISR_CMP		0x1
#define	NFC_ISR_ANY		0xFFDF

/** flash status control register */
#define	NFC_FSCR_STACMD	0xFF0000
#define	NFC_FSCR_STAMSK	0xFF

/** ECC control register */
#define	NFC_ECC_MODEH		0
#define	NFC_ECC_MODET4		0x20000
#define	NFC_ECC_MODET8		0x30000
#define	NFC_ECC_HOFS(x)		((x) << 4)
#define	NFC_ECC_HRST		2

/** ECC TECC4 control register */
#define	NFC_TECC4_REDEC		8
#define	NFC_TECC4_RST		2

/** ECC TECC8 control register */
#define	NFC_TECC8_SIZE(x)	((x) << 8)
#define	NFC_TECC8_RST		2

/** TECC4 debug register */
#define	NFC_TECC4DBG_STATE	0xF0000
#define	NFC_TECC4DBG_DBGCLK	0x1

/** TECC8 debug register */
#define	NFC_TECC8DBG_ERRSTATE	0xF000000
#define	NFC_TECC8DBG_STATE		0xF0000
#define	NFC_TECC8DBG_EV			0x4
#define	NFC_TECC8DBG_DBGCLK		0x1
