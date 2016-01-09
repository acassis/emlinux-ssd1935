#ifndef __SEC_H_
#define __SEC_H_

#define HDR_MAGIC_OFFSET        0x0
#define HDR_EC1_OFFSET          0x4
#define HDR_EC2_OFFSET          0x6

#define HDR_SECT_MAX_SIZE       4
#define EC_MAGIC                0x4D4C4321 /* magic: MLC! */
//#define MAX_BYTES		8

uint32_t ec_hdr_section_get(uint8_t offset, int len);
void ec_hdr_section_set(uint8_t offset, uint32_t value, int len);
void ec_hdr_create(void);
void ec_maintain(void);

#endif
