#ifndef __SDRTYPE_H__
#define __SDRTYPE_H__

typedef volatile struct _sdr_t {
	ulong	id;
	ulong	cap;
	ulong	ctl;
	ulong	cfg0;
	ulong	cfg1;
	ulong	rfrt;
	ulong	mrs;		
	ulong	dqs_cnt;	// for sdr & ddr
	ulong	dly_dat;	// only for sdr
	ulong	dly_cfg;	// tuning cfg
} sdr_t;


#define SDR_ID		0x05000040
#define SDR_CAP		0x00000000

#define SDR_CAP_INT_RFCLK	BIT(0)
#define SDR_CAP_DDR			BIT(1)


/* control reg */
#define SDR_CTL_EN	BIT(0)
#define SDR_CTL_RST	BIT(1)
#define SDR_CTL_LPEN	BIT(2)
#define SDR_CTL_PWRUP	BIT(3)
#define SDR_CTL_CLKSEL	BIT(4)
#define SDR_CTL_STUNEN	BIT(5)
#define SDR_CTL_TAP	BIT(6)
#define SDR_CTL_DDR	BIT(7)
#define SDR_CTL_RFCSEL	BIT(8)
#define SDR_CTL_CMD	(BIT(31) | BIT(30) | BIT(29) | BIT(28))
#define SDR_CMD(x)	((x) << 28)

/* cs config */
#define SDR_CS_EN	BIT(0)
#define SDR_CS_ATRFR	(BIT(4) | BIT(3) | BIT(2) | BIT(1))
#define SDR_CS_RCD	(BIT(6) | BIT(5))
#define SDR_CS_RPD	(BIT(8) | BIT(7))
#define SDR_CS_CAS	(BIT(12) | BIT(11) | BIT(10) | BIT(9))
#define SDR_CS_MRD	(BIT(14) | BIT(13))
#define SDR_CS_IAM	BIT(15)
#define SDR_CS_DPD	(BIT(17) | BIT(16))
#define SDR_CS_RSVD1 BIT(18)
#define SDR_CS_DSZ	BIT(19)
#define SDR_CS_BL	(BIT(22) | BIT(21) | BIT(20))
#define SDR_CS_COL	(BIT(25) | BIT(24) | BIT(23))
#define SDR_CS_ROW	(BIT(27) | BIT(26))
#define SDR_CS_RFR	(BIT(30) | BIT(29) | BIT(28))
#define SDR_CS_RSVD2 BIT(31)

#define SDR_ATRFR(x)	((x) << 1)
#define SDR_RCD(x)	((x) << 5)
#define SDR_RPD(x)	((x) << 7)
#define SDR_CAS(x)	((x) << 9)
#define SDR_MRD(x)	((x) << 13)
#define SDR_DPD(x)	((x) << 16)
#define SDR_BL(x)	((x) << 20)
#define SDR_COL(x)	((x) << 23)
#define SDR_ROW(x)	((x) << 26)
#define SDR_RFR(x)	((x) << 28)

#define SDR_EMRS_SEL(x)		((x) << 16)
#define SDR_EMRS_DS(x)		((x) << 5)
#define SDR_EMRS_PASR(x)	(x)

// sdramc commands issued via sdr ctl register
#define SC_NORMAL			0
#define SC_PRECHARGE_ALL	1
#define SC_AUTO_REFRESH		2
#define SC_SET_MRS			3
#define SC_RSVD				4
#define SC_SELF_REFRESH		5
#define SC_DQS_DELAY_TUNE	6
#define SC_PWR_DOWN			7
#define SC_DEEP_PWR_DOWN	8

// sdram jedec mode register command settings
#define MRS_BL_1	0
#define MRS_BL_2	1
#define MRS_BL_4	2
#define MRS_BL_8	3
#define MRS_BL_PAGE	7	// this is mapped as 3'b100 in sdramc

// sdram jedec extended mode register command settings for mobile sdram
#define EMRS_SEL	2UL

#define DS_FULL		0UL
#define DS_HALF		1UL

#define PASR_FULL	0UL
#define PASR_HALF	1UL
#define PASR_QTR	2UL

#endif	// __SDRTYPE_H__
