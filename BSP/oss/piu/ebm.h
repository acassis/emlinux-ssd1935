#ifndef EBM_SASI
#define EBM_SASI


#define EBM_F_SZ8	0
#define EBM_F_SZ16	1
#define EBM_F_SZ32	2
#define EBM_F_SZM	3
#define EBM_F_FIFO	4
#define EBM_F_BURST	8
#define EBM_F_WR	0x10

int			ebm_init(void *r);
void		ebm_exit(void *r);
/* 
	processor interrupts are disabled during the following calls to prevent 
	contention.  So calling ebm_ios will a large 'len' will lock up for the
	transfer
*/
void		ebm_wr32(void *r, uint32_t addr, uint32_t data);
uint32_t	ebm_rd32(void *r, uint32_t addr);
void		ebm_wr16(void *r, uint32_t addr, uint16_t data);
uint16_t	ebm_rd16(void *r, uint32_t addr);
void		ebm_wr8(void *r, uint32_t addr, uint8_t data);
uint8_t		ebm_rd8(void *r, uint32_t addr);
void		ebm_ios(void *r, uint32_t addr, void *data, int len, int flag);


#endif

