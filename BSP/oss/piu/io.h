#ifndef _IO_SASI
#define _IO_SASI


#define IO_MAP_MEM	1
#define IO_MAP_IO	2
#define IO_MAP_EBM	3
#define IO_WIDTH_32	
#define IO_WIDTH_16	
#define IO_WIDTH_8
#define IO_WIDTH_V
#define IO_TYPE_MEM
#define IO_TYPE_FIFO

/*
Interface
*/

void		io_wr32(volatile uint32_t * r, uint32_t d);
uint32_t	io_rd32(volatile uint32_t * r);
void		io_wr16(volatile uint32_t * r, uint16_t d);
uint16_t	io_rd16(volatile uint32_t * r);
void		io_wr8(volatile uint32_t * r, uint8_t d);
uint8_t		io_rd8(volatile uint32_t * r);

void		io_wr32s(volatile uint32_t * r, uint32_t *d, int len);
void		io_rd32s(volatile uint32_t * r, uint32_t *d, int len);
void		io_wr32f(volatile uint32_t * r, uint32_t *d, int len);
void		io_rd32f(volatile uint32_t * r, uint32_t *d, int len);
void		io_wr16s(volatile uint16_t * r, uint16_t *d, int len);
void		io_rd16s(volatile uint16_t * r, uint16_t *d, int len);
void		io_wr16f(volatile uint16_t * r, uint16_t *d, int len);
void		io_rd16f(volatile uint16_t * r, uint16_t *d, int len);
void		io_wr8s(volatile uint8_t *r, uint8_t *d, int len);
void		io_rd8s(volatile uint8_t *r, uint8_t *d, int len);
void		io_wr8f(volatile uint8_t *r, uint8_t *d, int len);
void		io_rd8f(volatile uint8_t *r, uint8_t *d, int len);
void		io_wrs(volatile uint8_t *r, uint8_t *d, int len);
void		io_rds(volatile uint8_t *r, uint8_t *d, int len);

void		io_bf_set32(volatile uint32_t *reg, 
				uint8_t pos, uint8_t width, uint32_t val);
uint32_t	io_bf_get32(volatile uint32_t *reg, uint8_t pos, uint8_t width);
void		io_b_wr32(volatile uint32_t *reg, uint8_t pos, uint8_t val);
void		io_b_clr32(volatile uint32_t *reg, uint8_t pos);
void		io_b_set32(volatile uint32_t *reg, uint8_t pos);

/*
Implementation
*/

#define io_b_wr32(reg, pos, val)			io_wr32(reg, \
	(((io_rd32(reg) & ~(1 << (pos)) | ((val) << (pos)))
#define io_b_set32(reg, pos)				io_b_wr32(reg, pos, 1)
#define io_b_clr32(reg, pos)				io_b_wr32(reg, pos, 0)
#define io_bf_set32(reg, pos, width, val)	io_wr32(reg, \
	(((io_rd32(reg) & ~(((1 << (width)) - 1)) << (pos))) | ((val) << (pos)))
#define io_bf_get32(reg, pos, width, val) \
	((io_rd32(reg) >> (pos)) & ~((1 << (width)) - 1))


#ifndef IO_MAP
#error IO_MAP undefined
#endif
#if IO_MAP==IO_MAP_MEM
#if 1
#define io_wr32(d, v)	(*(volatile uint32_t *)&(d) = v)
#define io_rd32(d)		(*(volatile uint32_t *)&(d))
#define io_wr16(d, v)	(*(volatile uint16_t *)&(d) = v)
#define io_rd16(d)		(*(volatile uint16_t *)&(d))
#else
#define io_wr32(d, v)	io_wr16(*(volatile uint16_t *)&d, (v) & 0xffff); \
		io_wr16(((volatile uint16_t *)&d)[1], ((v) >> 16) & 0xffff); 
#define io_rd32(d)		(io_rd16(*(volatile uint16_t *)&d) \
		| (io_rd16(((volatile uint16_t *)&d)[1]) << 16))
#define io_wr16(d, v)	io_wr8(*(v8)&d, (v) & 0xff); \
		io_wr8(((v8)&d)[1], ((v) >> 8) & 0xff); 
#define io_rd16(d)		(io_rd8(*(v8)&d) | (io_rd8(((v8)&d)[1]) << 8))
#endif
#define io_wr8(d, v)	(*(volatile uint8_t *)&(d) = v)
#define io_rd8(d)		(*(volatile uint8_t *)&(d))
#define io_wr32s		iom_wr32s
#define io_rd32s		iom_rd32s
#define io_wr32f		iom_wr32f
#define io_rd32f		iom_rd32f
#define io_wr16s		iom_wr16s
#define io_rd16s		iom_rd16s
#define io_wr16f		iom_wr16f
#define io_rd16f		iom_rd16f
#define io_wr8s			iom_wr8s
#define io_rd8s			iom_rd8s
#define io_wr8f			iom_wr8f
#define io_rd8f			iom_rd8f
#define io_wrs			iom_wrs
#define io_rds			iom_rds
void	iom_wr32s(volatile uint32_t * r, uint32_t *d, int len);
void	iom_wr16s(volatile uint16_t * r, uint16_t *d, int len);
void	iom_wr8s(volatile uint8_t *r, uint8_t *d, int len);
void	iom_rd32s(volatile uint32_t * r, uint32_t *d, int len);
void	iom_rd16s(volatile uint16_t * r, uint16_t *d, int len);
void	iom_rd8s(volatile uint8_t *r, uint8_t *d, int len);
void	iom_wrs(volatile uint8_t *r, uint8_t *d, int len);
void	iom_rds(volatile uint8_t *r, uint8_t *d, int len);
#elif IO_MAP==IO_MAP_IO
#define io_wr32(d, v)	outpd((uint32_t)&(d), v)
#define io_rd32(d)		inpd((uint32_t)&(d))
#define io_wr16(d, v)	outpw((uint32_t)&(d), v)
#define io_rd16(d)		inpw((uint32_t)&(d))
#define io_wr8(d, v)	outp((uint32_t)&(d), v)
#define io_rd8(d)		inp((uint32_t)&(d))
#define io_wr32s		ioi_wr32s
#define io_rd32s		ioi_rd32s
#define io_wr32f		ioi_wr32f
#define io_rd32f		ioi_rd32f
#define io_wr16s		ioi_wr16s
#define io_rd16s		ioi_rd16s
#define io_wr16f		ioi_wr16f
#define io_rd16f		ioi_rd16f
#define io_wr8s			ioi_wr8s
#define io_rd8s			ioi_rd8s
#define io_wr8f			ioi_wr8f
#define io_rd8f			ioi_rd8f
#define io_wrs			ioi_wrs
#define io_rds			ioi_rds
void	ioi_wr32s(volatile uint32_t * r, uint32_t *d, int len);
void	ioi_wr16s(volatile uint16_t * r, uint16_t *d, int len);
void	ioi_wr8s(volatile uint8_t *r, uint8_t *d, int len);
void	ioi_rd32s(volatile uint32_t * r, uint32_t d, int len);
void	ioi_rd16s(volatile uint16_t * r, uint16_t *d, int len);
void	ioi_rd8s(volatile uint8_t *r, uint8_t *d, int len);
void	ioi_wrs(volatile uint8_t *r, uint8_t *d, int len);
void	ioi_rds(volatile uint8_t *r, uint8_t *d, int len);
#elif IO_MAP==IO_MAP_EBM
#include "ebm.h"
extern void	*_ebmr;
#define io_wr32(d, v)		ebm_wr32(_ebmr, (uint32_t)&(d), v)
#define io_rd32(d)			ebm_rd32(_ebmr, (uint32_t)&(d))
#define io_wr16(d, v)		ebm_wr16(_ebmr, (uint32_t)&(d), v)
#define io_rd16(d)			ebm_rd16(_ebmr, (uint32_t)&(d))
#define io_wr8(d, v)		ebm_wr8(_ebmr, (uint32_t)&(d), v)
#define io_rd8(d)			ebm_rd8(_ebmr, (uint32_t)&(d))
#define io_wr32s(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, \
								EBM_F_WR | EBM_F_SZ32)
#define io_rd32s(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, EBM_F_SZ32)
#define io_wr32f(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, \
								EBM_F_WR | EBM_F_FIFO | EBM_F_SZ32)
#define io_rd32f(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, \
								EBM_F_FIFO | EBM_F_SZ32)
#define io_wr16s(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, \
								EBM_F_WR | EBM_F_SZ16)
#define io_rd16s(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, EBM_F_SZ16)
#define io_wr16f(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, \
								EBM_F_WR | EBM_F_FIFO | EBM_F_SZ16)
#define io_rd16f(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, \
								EBM_F_FIFO | EBM_F_SZ16)
#define io_wr8s(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, \
								EBM_F_WR | EBM_F_SZ8)
#define io_rd8s(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, EBM_F_SZ8)
#define io_wr8f(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, \
								EBM_F_WR | EBM_F_FIFO | EBM_F_SZ8)
#define io_rd8f(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, \
								EBM_F_FIFO | EBM_F_SZ8)
#if 0
#define io_wrs(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, \
								EBM_F_WR | EBM_F_SZM)
#define io_rds(r, d, l)	ebm_ios(_ebmr, (uint32_t)r, d, l, EBM_F_SZM)
#else
#define io_rds				io_rd32s
#define io_wrs				io_wr32s
#endif
#else
#error IO_MAP definition invalid
#endif


#endif

