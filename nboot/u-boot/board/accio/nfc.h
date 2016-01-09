#ifndef	_NFC_SASI_
#define	_NFC_SASI_

/**
@file		nfc.h
@author		sasin@solomon-systech.com
@version	1.0
@comments	for booting from CS0 only
*/

#define NFC_ERR_CMD	-1
#define NFC_ERR_DAT	-2
#define NFC_ERR_OP	-3
#define NFC_ERR_TOUT	-4

/** nfc context */
typedef struct
{
	void	*r;		/**< controller base address */
	int		cs;		/**< current chip select, remember to cfg after changing */
	int		ecc;	/**< ecc enabled? */
	int		page;	/**< current chip page size shift value, read only */
	int		ver;
	int		eccmode;
}
nfc_t, *nfc_p;


int nfc_init(nfc_p t);
/**<
nfc initialization
@param[in]	t		context
@return		nfc error
*/

void nfc_exit(nfc_p t);
/**<
disable nfc device
@param[in]	t		context
*/


int nfc_cfg(nfc_p t, nand_geo_t *c);
/**<
configure geometry
@param[in]	t		context
@param[in]	c		geometry
*/

int nfc_nand_rst(nfc_p t);
/**<
reset NAND
@param[in]	t	context
@return		NFC_ERR_xxx
*/

int nfc_nand_id(nfc_p t, nand_id_t *id);
/**<
read NAND id bytes
@param[in]	t	context
@param[in]	id	id
@return		NFC_ERR_xxx
*/

int nfc_nand_read(nfc_p t, uint32_t page);
/**<
read NAND page
@param[in]	t		context
@param[in]	page	page number
@return		NFC_ERR_xxx
*/

int nfc_nand_prog(nfc_p t, uint32_t page);
/**<
program NAND page
@param[in]	t		context
@param[in]	page	page number
@return		NFC_ERR_xxx or orred bitmask of NAND_ST_xxx
*/

int nfc_nand_erase(nfc_p t, uint32_t page);
/**<
erase NAND block
@param[in]	t		context
@param[in]	page	page number at which the block starts
@return		NFC_ERR_xxx or orred bitmask of NAND_ST_xxx
*/

void nfc_nand_boot(nfc_p t);
/**<
read NAND page 0 and boot from it
@param[in]	t		context
*/

void nfc_buf_read(nfc_p t, uint32_t *buf, int count);
/**<
read NFC  buffer
@param[in]	t		context
@param[out]	buf		buffer
@param[in]	count	count of quad-octets to read to buffer
*/

void nfc_buf_write(nfc_p t, uint32_t *buf, int count);
/**<
write NFC  buffer
@param[in]	t		context
@param[out]	buf		buffer
@param[in]	count	count of quad-octets to write from buffer
*/

int block_isbad(nfc_p t, uint32_t pg, uint32_t pgsz);

#endif

