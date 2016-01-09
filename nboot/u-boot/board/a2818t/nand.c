#include <common.h>
#include "nand.h"


typedef struct
{
	uint8_t	id;
	uint8_t	page;
	uint8_t	block;
	uint8_t	size;
}
nand_old_t;

typedef struct
{
	uint8_t	id;
	uint8_t	size;
}
nand_new_t;

//#define NAND_OLD_ID16	22
#define NAND_OLD_ID16	21
#define NAND_NEW_ID16	12

static const nand_old_t	nandold[] = 
{
	{0x6e, 8, 12, 0},
	{0x64, 8, 12, 1},
	{0x6b, 9, 13, 2},
	{0xe8, 8, 12, 0},
	{0xec, 8, 12, 0},
	{0xea, 8, 12, 1},
//	{0xd5, 9, 13, 2},
	{0xe3, 9, 13, 2},
	{0xe5, 9, 13, 2},
	{0xd6, 9, 13, 3},
	{0x39, 9, 13, 3},
	{0xe6, 9, 13, 3},
	{0x33, 9, 14, 4},
	{0x73, 9, 14, 4},
	{0x35, 9, 14, 5},
	{0x75, 9, 14, 5},
	{0x36, 9, 14, 6},
	{0x76, 9, 14, 6},
	{0x78, 9, 14, 7},
	{0x39, 9, 14, 7},
	{0x79, 9, 14, 7},
	{0x71, 9, 14, 8},
/* 16 bit */
	{0x49, 9, 13, 3},
	{0x59, 9, 13, 3},
	{0x43, 9, 14, 4},
	{0x53, 9, 14, 4},
	{0x45, 9, 14, 5},
	{0x55, 9, 14, 5},
	{0x46, 9, 14, 6},
	{0x56, 9, 14, 6},
	{0x72, 9, 14, 7},
	{0x49, 9, 14, 7},
	{0x74, 9, 14, 7},
	{0x59, 9, 14, 7},
};

static const nand_new_t	nandnew[] = 
{
	{0xA2, 6},
	{0xF2, 6},
	{0xA1, 7},
	{0xF1, 7},
	{0xAA, 8},
	{0xDA, 8},
	{0xAC, 9},
	{0xDC, 9},
	{0xA3, 10},
	{0xD3, 10},
	{0xA5, 11},
	{0xD5, 11},
/* 16 bit */
	{0xB2, 6},
	{0xC2, 6},
	{0xB1, 7},
	{0xC1, 7},
	{0xBA, 8},
	{0xCA, 8},
	{0xBC, 9},
	{0xCC, 9},
	{0xB3, 10},
	{0xC3, 10},
	{0xB5, 11},
	{0xC5, 11},
};


#define NAND_ID_PAGE(id)	(((id) & 3) + 10)
#define NAND_ID_SPARE(id, pg, tmp)	\
	((tmp = ((id) >> 2) & 3) & 2) ? \
		((tmp & 1) ? 0 : 218) :		\
		(1 << (tmp + 3 + (pg) - 9))
/* id[3] is rsv 0 but for 4K page, spare can be 218 bytes id[3:2]=1:0 */
#define NAND_ID_BLOCK(id)	((((id) >> 4) & 3) + 16)
#define NAND_ID_BIT16(id)	(((id) >> 6) & 1)

int  nand_geo(nand_id_t *id, nand_geo_t *geo)
/*
@return 0, else error
*/
{
	uint8_t	b;
	int		i;

	b = id->dev;
	for (i = 0; i < sizeof(nandold) / sizeof(nandold[0]); i++)
	{
		if (b == nandold[i].id)
		{
			geo->page = nandold[i].page;
			geo->block = nandold[i].block;
			geo->size = nandold[i].size;
			geo->spare = 1 << (geo->page - 5);
			geo->bit16 = (i >= NAND_OLD_ID16) ? 1 : 0;
			return 0;
		}
	}
	for (i = 0; i < sizeof(nandnew) / sizeof(nandnew[0]); i++)
	{
		if (b == nandnew[i].id)
		{
			uint8_t	spare;

			b = id->geo;
			geo->page = NAND_ID_PAGE(b);
			geo->block = NAND_ID_BLOCK(b);
			geo->size = nandnew[i].size;
			geo->spare = NAND_ID_SPARE(b, NAND_ID_PAGE(b), spare);
			if (!geo->spare)
			{
				/* unknown spare setting */
				return -2;
			}
			geo->bit16 = NAND_ID_BIT16(b);
			return 0;
		}
	}
	return -1;
}

