#ifndef NAND_SASI
#define NAND_SASI


#define NAND_CMD_READ		0
#define NAND_CMD_READ_SLC	1
#define NAND_CMD_READ_MLC	0x30
#define NAND_CMD_PROG		0x10
#define NAND_CMD_ERASE		0x60
#define NAND_CMD_STAT		0x70
#define NAND_CMD_SEQ		0x80
#define NAND_CMD_ID			0x90
#define NAND_CMD_ERASE2		0xD0
#define NAND_CMD_RST		0xFF

#define NAND_ST_FAIL		0x01
#define NAND_ST_FAIL_N1		0x02
#define NAND_ST_TRUE_READY	0x20
#define NAND_ST_READY		0x40
#define NAND_ST_WP			0x80

typedef struct
{
	uint8_t	mfg;
	uint8_t	dev;
	uint8_t	cell;
	uint8_t	geo;
	uint8_t	ext;
}
nand_id_t;

typedef struct
{
	uint8_t	page;	/* bytes per page = 1 << page */
	uint8_t	block;	/* bytes per block = 1 << block */
	uint8_t	size;	/* bytes per device = 1MB << size */
	uint8_t	bit16;	/* 0:8 bit data, 1:16 bit data */
	uint8_t	spare;	/* bytes per spare area */
}
nand_geo_t;


int  nand_geo(nand_id_t *id, nand_geo_t *geo);


#endif

