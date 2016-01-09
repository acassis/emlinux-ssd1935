#ifndef __NBOOT_H
#define __NBOOT_H

#include "config.h"
#define LDR_BASE	0xFFFF1000
#define LDR_UBOOT_BASE  0xFFFF2000
#define NBOOT_MAGIC	0x4E424C52
#define TUNE_MAGIC  0x4E424C25
#define NBOOT_MAX_LEN	(1 << 19)	/* 512 KB */
#define IPL_LEN		4096		/* 4 KB	*/
#define TUNE_LEN    (1 << 13)   /* 8 KB */
#define BOOT_SIZE	(1 << 20)
#define KERN_SIZE	(3 << 20)   // load 3MB kernel image

#ifdef CONFIG_CPT480X272
#define FLT2_ADDR	0x53E7A000
#elif CONFIG_TPO800X480
#ifdef CONFIG_BPP32
#define FLT2_ADDR	0x53B80000
#else
#define FLT2_ADDR	0x53DCC000
#endif
#else
#define FLT2_ADDR	0x53F1F000
#endif

struct nboot_desc {
	uint32_t magic;
	uint32_t len;
};

struct ddr_tune_desc {
	uint32_t magic;
	uint32_t len;
};

#endif

