#include <common.h>
#include "i2c.h"
#include "i2cr.h"
#include "nval.h"

#define BOOT_SIZE		(1 << 20) 
#define KERN_SIZE		(2 << 20)
#define ROOT_SIZE		(8 << 20)

#define SECT1_BOOT_START	0
#define SECT1_KERN_START	(SECT1_BOOT_START + BOOT_SIZE)
#define SECT1_ROOT_START	(SECT1_KERN_START + KERN_SIZE)

#define SECT2_BOOT_START	(SECT1_ROOT_START + ROOT_SIZE)
#define SECT2_KERN_START	(SECT2_BOOT_START + BOOT_SIZE)
#define SECT2_ROOT_START	(SECT2_KERN_START + KERN_SIZE)

#define SECT3_BOOT_START	(SECT2_ROOT_START + ROOT_SIZE)
#define SECT3_KERN_START	(SECT3_BOOT_START + BOOT_SIZE)
#define SECT3_ROOT_START	(SECT3_KERN_START + KERN_SIZE)

#define EC_BACKUP		3000

static uint8_t *itob(uint8_t *dest, uint32_t num, uint8_t len)
{
	int i;

	if (len != 2 && len != 4)
	{
		return 0;
	}

	/* only the same endian */
	for (i = 0; i < len; i++)
	{
		dest[i] = *((uint8_t *)&num + i);
	}

	return dest;
}

static uint32_t btoi(uint8_t *buf, int len)
{
	int i;
	uint32_t value = 0;
	uint8_t *p = (uint8_t *)&value;

	if (len != 2 && len != 4)
	{
		return 0;
	}

	for (i = 0; i < len; i++)
	{
		p[i] = buf[i];
	}

	return value;
}

uint32_t ec_hdr_section_get(uint8_t offset, int len)
{
	uint8_t buf[HDR_SECT_MAX_SIZE];
	eeprom_read(CFG_I2C_EEPROM_ADDR, offset, buf, len);
	return btoi(buf, len);
}

void ec_hdr_section_set(uint8_t offset, uint32_t value, int len)
{
	uint8_t buf[HDR_SECT_MAX_SIZE];

	if (len != 2 && len != 4)
	{
		printf("unreasonable hdr sect length\n");
		return;
	}

	itob(buf, value, len);
	eeprom_write(CFG_I2C_EEPROM_ADDR, offset, buf, len);
}

void ec_hdr_create()
{
	ec_hdr_section_set(HDR_MAGIC_OFFSET, EC_MAGIC, 4);
	ec_hdr_section_set(HDR_EC1_OFFSET, 1, 2);
	ec_hdr_section_set(HDR_EC2_OFFSET, 1, 2);
}

void rd_maintain()
{
	nand_copy("progc", SECT2_ROOT_START, SECT1_ROOT_START, ROOT_SIZE);	
}

void ec_maintain()
{
//	int	ec1_update_flag = 1, ec2_update_flag = 1;
	uint32_t magic = ec_hdr_section_get(HDR_MAGIC_OFFSET, 4);
	if (magic == EC_MAGIC)
	{
		uint16_t ec1 = ec_hdr_section_get(HDR_EC1_OFFSET, 2);
//printf("ec1 = %d\n", ec1);
		if (ec1 >= EC_BACKUP)
		{
			uint16_t ec2 = ec_hdr_section_get(HDR_EC2_OFFSET, 2);
//printf("ec2 = %d\n", ec2);
			if (ec2 >= EC_BACKUP)
			{
				printf("The 2nd level mirror maintaining:\n");
				if (!nand_copy("progv", SECT2_BOOT_START, SECT3_BOOT_START, BOOT_SIZE))
				{
					nand_copy("prog", SECT2_BOOT_START, SECT3_BOOT_START, BOOT_SIZE);
				}
				else
				{
					nand_copy("prog", SECT2_BOOT_START, SECT1_BOOT_START, BOOT_SIZE);
				}

				if (!nand_copy("progcv", SECT2_KERN_START, SECT3_KERN_START, KERN_SIZE))
				{
					nand_copy("progc", SECT2_KERN_START, SECT3_KERN_START, KERN_SIZE);
				}
				else
				{
					nand_copy("progc", SECT2_KERN_START, SECT1_KERN_START, KERN_SIZE);
				}

				if (!nand_copy("progcv", SECT2_ROOT_START, SECT3_ROOT_START, ROOT_SIZE))
				{
					nand_copy("progc", SECT2_ROOT_START, SECT3_ROOT_START, ROOT_SIZE);
				}
				else
				{
					nand_copy("progc", SECT2_ROOT_START, SECT1_ROOT_START, ROOT_SIZE);
				}
#if 0
				if (ec2_update_flag)
				{
					ec2 = 0;
					ec_hdr_section_set(HDR_EC2_OFFSET, ec2, 2);
				}
#endif
				ec2 = 0;
				ec_hdr_section_set(HDR_EC2_OFFSET, ec2, 2);
			}

			printf("The 1st level mirror maintaining:\n");
			if (!nand_copy("progv", SECT1_BOOT_START, SECT2_BOOT_START, BOOT_SIZE))
			{
				nand_copy("prog", SECT1_BOOT_START, SECT2_BOOT_START, BOOT_SIZE);
			}
			else
			{
				if (!nand_copy("progv", SECT1_BOOT_START, SECT3_BOOT_START, BOOT_SIZE))
				{
					nand_copy("prog", SECT1_BOOT_START, SECT3_BOOT_START, BOOT_SIZE);
//					nand_copy("prog", SECT2_BOOT_START, SECT3_BOOT_START, BOOT_SIZE);
				}
				else
				{
					printf("restore backup1&2 from primary nboot!\n");
					nand_copy("prog", SECT2_BOOT_START, SECT1_BOOT_START, BOOT_SIZE);
					nand_copy("prog", SECT3_BOOT_START, SECT1_BOOT_START, BOOT_SIZE);
//					nand_copy("prog", SECT1_BOOT_START, SECT2_BOOT_START, BOOT_SIZE);
				}
			}

			if (!nand_copy("progcv", SECT1_KERN_START, SECT2_KERN_START, KERN_SIZE))
			{
				nand_copy("progc", SECT1_KERN_START, SECT2_KERN_START, KERN_SIZE);
			}
			else
			{
				if (!nand_copy("progcv", SECT1_KERN_START, SECT3_KERN_START, KERN_SIZE))
				{
					nand_copy("progc", SECT1_KERN_START, SECT3_KERN_START, KERN_SIZE);
//					nand_copy("progc", SECT2_KERN_START, SECT3_KERN_START, KERN_SIZE);
				}
				else
				{
					printf("restore backup1&2 from primary kernel!\n");
					nand_copy("progc", SECT2_KERN_START, SECT1_KERN_START, KERN_SIZE);
					nand_copy("progc", SECT3_KERN_START, SECT1_KERN_START, KERN_SIZE);
//					nand_copy("progc", SECT1_KERN_START, SECT2_KERN_START, KERN_SIZE);
				}
			}

			if (!nand_copy("progcv", SECT1_ROOT_START, SECT2_ROOT_START, ROOT_SIZE))
			{
				nand_copy("progc", SECT1_ROOT_START, SECT2_ROOT_START, ROOT_SIZE);
			}
			else
			{
				if (!nand_copy("progcv", SECT1_ROOT_START, SECT3_ROOT_START, ROOT_SIZE))
				{
					nand_copy("progc", SECT1_ROOT_START, SECT3_ROOT_START, ROOT_SIZE);
//					nand_copy("progc", SECT2_ROOT_START, SECT3_ROOT_START, ROOT_SIZE);
				}
				else
				{
					printf("restore backup1&2 from primary rootdisk!\n");
					nand_copy("progc", SECT2_ROOT_START, SECT1_ROOT_START, ROOT_SIZE);
					nand_copy("progc", SECT3_ROOT_START, SECT1_ROOT_START, ROOT_SIZE);
//					nand_copy("progc", SECT1_ROOT_START, SECT2_ROOT_START, ROOT_SIZE);
				}
			}

#if 0
			if (ec1_update_flag)
				ec1 = 0;
			ec2++;
			ec_hdr_section_set(HDR_EC2_OFFSET, ec2, 2);
#endif
			ec1 = 0;
			ec2++;
			ec_hdr_section_set(HDR_EC2_OFFSET, ec2, 2);
		}
		ec1++;
		ec_hdr_section_set(HDR_EC1_OFFSET, ec1, 2);
	}
	else
	{
		ec_hdr_create();
	}
}

