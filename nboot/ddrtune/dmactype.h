#ifndef __DMACTYPE_H__
#define __DMACTYPE_H__
#include "mytypes.h"

typedef volatile struct _chan_t {
	unsigned long		cfg;
	unsigned long		saddr;
	unsigned long		daddr;
	unsigned long		tc;
	unsigned long		ien;
	unsigned long		isr;
	unsigned long		rsvd[2];
} chan_t;

typedef volatile struct _dma_t {
	unsigned long		id;
	unsigned long		cap;
	unsigned long		ctl;
	unsigned long		rsvd0;
	unsigned long		ien;
	unsigned long		isr;
	unsigned long		rsvd1[(0x100-0x018)>>2];
	chan_t				chan[1];
} dmac_t;

#define DMAC_ID		0x08010440UL

#define DMAC_CAP_NCH	0x00000003UL
#define DMAC_CAP_FIFOSZ	0x0000000CUL

#define GET_DMAC_NCH(x)		((x) & DMAC_CAP_NCH)
#define GET_DMAC_FIFOSZ(x)	(((x) & DMAC_CAP_FIFOSZ) >> 2)

#define DMAC_CTL_EN	BIT(0)
#define DMAC_CTL_RST	BIT(1)
#define DMAC_CTL_PRI	BIT(2)
#define DMAC_CTL_WTDLY	0x00000F00UL
#define DMAC_CTL_BT	0x0000E000UL

// dmac burst type encoding
#define DMAC_BT(x)		(((x) << 13) & DMAC_CTL_BT)
#define GET_DMAC_BT(x)		(((x) & DMAC_CTL_BT) >> 13)

// dmac waitdly encoding
#define DMAC_WTDLY(x)		(((x) << 8) & DMAC_CTL_WTDLY)
#define GET_DMAC_WTDLY(x)	(((x) & DMAC_CTL_WTDLY) >> 8)

// dmac channel congfiguration registers and bitfields
#define DMAC_CFG_DREQ	0xF0000000UL
#define DMAC_CFG_DASZ	0x0C000000UL
#define DMAC_CFG_DTYP	0x03000000UL
#define DMAC_CFG_SREQ	0x00F00000UL
#define DMAC_CFG_SASZ	0x000C0000UL
#define DMAC_CFG_STYP	0x00030000UL
#define DMAC_CFG_BL	0x00003F00UL
#define DMAC_CFG_FLSHEN	BIT(3)
#define DMAC_CFG_RST	BIT(2)
#define DMAC_CFG_DIS	BIT(1)
#define DMAC_CFG_EN	BIT(0)

#define DMAC_DREQ(x)	(((x) << 28) & DMAC_CFG_DREQ)
#define DMAC_DASZ(x)	(((x) << 26) & DMAC_CFG_DASZ)
#define DMAC_DTYP(x)	(((x) << 24) & DMAC_CFG_DTYP)
#define DMAC_SREQ(x)	(((x) << 20) & DMAC_CFG_SREQ)
#define DMAC_SASZ(x)	(((x) << 18) & DMAC_CFG_SASZ)
#define DMAC_STYP(x)	(((x) << 16) & DMAC_CFG_STYP)
#define DMAC_BL(x)	(((x) << 8)  & DMAC_CFG_BL)

#define GET_DMAC_DREQ(x)	(((x) & DMAC_CFG_DREQ) >> 28)
#define GET_DMAC_DASZ(x)	(((x) & DMAC_CFG_DASZ) >> 26)
#define GET_DMAC_DTYP(x)	(((x) & DMAC_CFG_DTYP) >> 24)
#define GET_DMAC_SREQ(x)	(((x) & DMAC_CFG_SREQ) >> 20
#define GET_DMAC_SASZ(x)	(((x) & DMAC_CFG_SASZ) >> 18
#define GET_DMAC_STYP(x)	(((x) & DMAC_CFG_STYP) >> 16)
#define GET_DMAC_BL(x)		(((x) & DMAC_CFG_BL) >> 8)

// dmac interrupt & status register bits
#define DMAC_INT_DONE	BIT(0)
#define DMAC_INT_BSY	BIT(1)
#define DMAC_INT_SRQ	BIT(2)
#define DMAC_INT_DRQ	BIT(3)
#define DMAC_INT_FLSH	BIT(4)

#endif


