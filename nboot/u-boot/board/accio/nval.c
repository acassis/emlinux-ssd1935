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


int nand_check(uint32_t phy_add,int size,int *total_errpage);
int nand_copy(char *prog, uint32_t dest, uint32_t src, int size);

void rd_maintain(void)
{

	//#define BURN_IN	
//	#define VERBOSE
	#define ECC_BIT_LIMIT 2
	int i=0,j=0;
	int rt;
	int total_page;
		
	
	#ifdef BURN_IN	
	for(;;)
	#endif	
	
	{
		
		
		if (nand_copy("progcv", SECT1_ROOT_START, SECT2_ROOT_START, ROOT_SIZE))
		{	
			//s/w update will only change sect1, we need to copy sect1 rd to sect 2
			//coz sect 2 is logical mtd2 for cramfs
			//so, this case will only happen after s/w update
			#ifdef VERBOSE
			printf("root disk update from spare copy...");
			#endif
			nand_copy("progc", SECT2_ROOT_START, SECT1_ROOT_START, ROOT_SIZE);					
		}			
		else
		{
			#ifdef VERBOSE
			printf("sect1 & sect2 root disk identical\n");	
			#endif
		}	
		
						
		#ifdef BURN_IN	 
		#ifdef VERBOSE
		printf("burn-in count = %d\n",j++);
		#endif
		nand_copy("progc", SECT2_ROOT_START, SECT1_ROOT_START, ROOT_SIZE);					
		nand_copy("progc", SECT1_ROOT_START, SECT2_ROOT_START, ROOT_SIZE);					
		
		for(i=0;i<10;i++)
		#endif
		{	
			rt = nand_check(SECT1_ROOT_START,ROOT_SIZE,&total_page);
			#ifdef VERBOSE
			printf("nand check on sect 1 rd, max ecc corr bit = %d, total err page = %d\n",rt,total_page);
			#endif
			
			if(rt>ECC_BIT_LIMIT)
			{		
				printf("ecc corr bit exceed limit, re-flashing setc1 rd\n");			
				nand_copy("progc", SECT1_ROOT_START, SECT2_ROOT_START, ROOT_SIZE);					
			}	
				
			
			rt = nand_check(SECT2_ROOT_START,ROOT_SIZE,&total_page);		
			#ifdef VERBOSE
			printf("nand check on sect 2 rd, max ecc corr bit = %d, total err page = %d\n",rt,total_page);					
			#endif
			
			if(rt>ECC_BIT_LIMIT)
			{	
				printf("ecc corr bit exceed limit, re-flashing setc2 rd\n");			
				nand_copy("progc", SECT2_ROOT_START, SECT1_ROOT_START, ROOT_SIZE);
			}	
		}
	}
	
			
}




void ec_maintain()
{
	//	int	ec1_update_flag = 1, ec2_update_flag = 1;
	uint32_t magic = ec_hdr_section_get(HDR_MAGIC_OFFSET, 4);
	if (magic == EC_MAGIC)
	{
		uint16_t ec1 = ec_hdr_section_get(HDR_EC1_OFFSET, 2);
		printf("ec1 = %d\n", ec1);
		if (ec1 >= EC_BACKUP)
		{
			uint16_t ec2 = ec_hdr_section_get(HDR_EC2_OFFSET, 2);
			printf("ec2 = %d\n", ec2);
			if (ec2 >= EC_BACKUP)
			{
				printf("The 2nd level mirror maintaining:\n");
				//A. Handle Bootloader
				if (!nand_copy("progv", SECT2_BOOT_START, SECT3_BOOT_START, BOOT_SIZE))
				{
					//rewrite 2 if 2&3 are identical
					nand_copy("prog", SECT2_BOOT_START, SECT3_BOOT_START, BOOT_SIZE);
				}
				else
				{
					//copy 1 to 2 if 2&3 are different
					nand_copy("prog", SECT2_BOOT_START, SECT1_BOOT_START, BOOT_SIZE);
				}

				//B. Handle Kernel
				if (!nand_copy("progcv", SECT2_KERN_START, SECT3_KERN_START, KERN_SIZE))
				{
					nand_copy("progc", SECT2_KERN_START, SECT3_KERN_START, KERN_SIZE);
				}
				else
				{
					nand_copy("progc", SECT2_KERN_START, SECT1_KERN_START, KERN_SIZE);
				}

				//C. Handle Rootdisk
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
			
			//A. Handle Bootloader
			if (!nand_copy("progv", SECT1_BOOT_START, SECT2_BOOT_START, BOOT_SIZE))
			{
				nand_copy("prog", SECT1_BOOT_START, SECT2_BOOT_START, BOOT_SIZE);
			}
			else
			{
				if (!nand_copy("progv", SECT1_BOOT_START, SECT3_BOOT_START, BOOT_SIZE))
				{
					nand_copy("prog", SECT1_BOOT_START, SECT3_BOOT_START, BOOT_SIZE);
					//nand_copy("prog", SECT2_BOOT_START, SECT3_BOOT_START, BOOT_SIZE);
				}
				else
				{
					printf("restore backup1&2 from primary nboot!\n");
					nand_copy("prog", SECT2_BOOT_START, SECT1_BOOT_START, BOOT_SIZE);
					nand_copy("prog", SECT3_BOOT_START, SECT1_BOOT_START, BOOT_SIZE);
					//nand_copy("prog", SECT1_BOOT_START, SECT2_BOOT_START, BOOT_SIZE);
				}
			}

			//B. Handle Kernel
			if (!nand_copy("progcv", SECT1_KERN_START, SECT2_KERN_START, KERN_SIZE))
			{
				nand_copy("progc", SECT1_KERN_START, SECT2_KERN_START, KERN_SIZE);
			}
			else
			{
				if (!nand_copy("progcv", SECT1_KERN_START, SECT3_KERN_START, KERN_SIZE))
				{
					nand_copy("progc", SECT1_KERN_START, SECT3_KERN_START, KERN_SIZE);
					//nand_copy("progc", SECT2_KERN_START, SECT3_KERN_START, KERN_SIZE);
				}
				else
				{
					printf("restore backup1&2 from primary kernel!\n");
					nand_copy("progc", SECT2_KERN_START, SECT1_KERN_START, KERN_SIZE);
					nand_copy("progc", SECT3_KERN_START, SECT1_KERN_START, KERN_SIZE);
					//nand_copy("progc", SECT1_KERN_START, SECT2_KERN_START, KERN_SIZE);
				}
			}
			
			//C. Handle Rootdisk
			if (!nand_copy("progcv", SECT1_ROOT_START, SECT2_ROOT_START, ROOT_SIZE))
			{
				//rewrite 1 if 1&2 identical
				nand_copy("progc", SECT1_ROOT_START, SECT2_ROOT_START, ROOT_SIZE);
			}
			else
			{				
				if (!nand_copy("progcv", SECT1_ROOT_START, SECT3_ROOT_START, ROOT_SIZE))
				{
					//rewrite 1 if 1&3 identical
					nand_copy("progc", SECT1_ROOT_START, SECT3_ROOT_START, ROOT_SIZE);
					//nand_copy("progc", SECT2_ROOT_START, SECT3_ROOT_START, ROOT_SIZE);
				}
				else
				{
					//both 2&3 are different from 1, copy 1 to 2&3, 
					//case of after s/w update
					printf("restore backup1&2 from primary rootdisk!\n");
					nand_copy("progc", SECT2_ROOT_START, SECT1_ROOT_START, ROOT_SIZE);
					nand_copy("progc", SECT3_ROOT_START, SECT1_ROOT_START, ROOT_SIZE);
					//nand_copy("progc", SECT1_ROOT_START, SECT2_ROOT_START, ROOT_SIZE);
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

