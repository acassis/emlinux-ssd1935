#ifndef __DMA_H__
#define __DMA_H__


#define dma_test_1(s,d,l)			dma_test_1_ex_bt((s),(d),(l), 0, -1UL)
#define dma_test_2(s1,d1,s2,d2,l)		dma_test_2_ex_bt((s1),(d1),(s2),(d2),(l), 0, -1UL)

#define dma_test_1_ex(s,d,l,r)			dma_test_1_ex_bt((s),(d),(l),(r),-1UL)
#define dma_test_2_ex(s1,d1,s2,d2,l,r)		dma_test_2_ex_bt((s1),(d1),(s2),(d2),(l),(r),-1UL)

int dma_test_1_ex_bt (
	unsigned long src,
	unsigned long dst,
	unsigned long *len,
	unsigned long *reading,
	unsigned long bt);

int dma_test_2_ex_bt (
	unsigned long src1,
	unsigned long dst1,
	unsigned long src2,
	unsigned long dst2,
	unsigned long *len,
	unsigned long *reading,
	unsigned long bt);

#endif
