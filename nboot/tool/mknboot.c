#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "../include/nboot.h"
#include "../include/config.h"

#define IPL		"ipl.bin"
#define SPL		"u-boot.bin" 
#define NBOOT		"nboot.bin"

#if defined DDR_TUNE
#define DDR_TUNE_NAME	"ddrtune.bin "
#endif

#define FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)


static void usage (char *arg)
{
	fprintf(stderr, "Usage: %s [-i ipl] [-s spl] [-o nboot] [-h]\n", arg);
	exit(EXIT_FAILURE);
}
 
int main (int argc, char *argv[])
{
	int		c;
	char		*iname = NULL;
	char		*sname = NULL;
	char		*oname = NULL;
	char		*dname = NULL;
	int		fdi, fds, fdo;
	void		*bufi, *bufs, *bufo, *dst;
	int		leni, pad, lens;
	struct stat	statbuf;
	struct nboot_desc	nboot;
#if defined DDR_TUNE
	int fdd, lend, pad_ipl, pad_tune;
	void * buf_tune, *tmp;
	struct ddr_tune_desc 	ddr_tune;
#endif

	while ((c = getopt(argc, argv, ":i:s:o:d:h")) != -1) {
		switch (c) {
		case 'i':
			iname = optarg;
			break;
		case 's':
			sname = optarg;
			break;
		case 'o':
			oname = optarg;
			break;
		case 'd':
			dname = optarg;
			break;
		case 'h':
		case '?':
		default :
			usage(argv[0]);
			break;
		}
	}

	if (iname == NULL)
		iname = IPL;

	if (sname == NULL)
		sname = SPL;

	if (oname == NULL)
		oname = NBOOT;

#if defined DDR_TUNE
	if (dname == NULL)
		dname = DDR_TUNE_NAME;
	printf("i = %s, d = %s, s = %s, o = %s\n", iname, dname, sname, oname);
#else
	printf("i = %s, s = %s, o = %s\n", iname, sname, oname);
#endif

	if ((fdi = open(iname, O_RDONLY)) < 0) {
		printf("can't open %s\n", iname);
		return -1;
	}

	if ((fds = open(sname, O_RDONLY)) < 0) {
		printf("can't open %s\n", sname);
		close(fdi);
		return -1;
	}

	if ((fdo = open(oname, O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0) {
		printf("can't open %s\n", oname);
		close(fds);
		close(fdi);
		return -1;
	}

#ifdef DDR_TUNE
	if ((fdd = open(dname, O_RDONLY)) < 0) {
		printf("can't open %s\n", dname);
		close(fdi);
		close(fds);
		close(fdo);
		return -1;
	}

	if (fstat(fdi, &statbuf) == 0) {
		leni = statbuf.st_size;
		pad_ipl = IPL_LEN - leni - sizeof(struct ddr_tune_desc);
		if (pad_ipl < 0) {
			printf("%s size too length\n", iname);
			return -1;
		}
	} else {
		printf("fstat %s error\n", iname);
		return -1;
	}

	if (fstat(fdd, &statbuf) == 0) {
		lend = statbuf.st_size;
		pad_tune = TUNE_LEN - lend - sizeof(struct nboot_desc);
		if (pad_tune < 0) {
			printf("%s size too length\n", dname);
			return -1;
		}
	} else {
		printf("fstat %s error\n", dname);
		return -1;
	}
#else
	if (fstat(fdi, &statbuf) == 0) {
		leni = statbuf.st_size;
		pad = IPL_LEN - leni - sizeof(struct nboot_desc);
		if (pad < 0) {
			printf("%s size too length\n", iname);
			return -1;
		}
	} else {
		printf("fstat %s error\n", iname);
		return -1;
	}
#endif

	if (fstat(fds, &statbuf) == 0) {
		lens = statbuf.st_size;
	} else {
		printf("fstat %s error\n", sname);
		return -1;
	}
#if defined DDR_TUNE
	nboot.magic = NBOOT_MAGIC;
	nboot.len = IPL_LEN + TUNE_LEN +lens;
	ddr_tune.magic = TUNE_MAGIC;
	ddr_tune.len = TUNE_LEN;
#else
	nboot.magic = NBOOT_MAGIC;
	nboot.len = IPL_LEN + lens;
#endif

	if (lseek(fdo, nboot.len - 1, SEEK_SET) == -1) {
		printf("lseek %s error\n", oname);
		return -1;
	}

	if (write(fdo, "", 1) != 1) {
		printf("write %s error\n", oname);
		return -1;
	}

	if ((bufi = mmap(0, leni, PROT_READ, MAP_SHARED, fdi, 0)) == MAP_FAILED) {
		printf("mmap %s error\n", iname);
		return -1;
	}

	if ((bufs = mmap(0, lens, PROT_READ, MAP_SHARED, fds, 0)) == MAP_FAILED) {
		printf("mmap %s error\n", sname);
		return -1;
	}

#if defined DDR_TUNE
	if ((buf_tune = mmap(0, lend, PROT_READ, MAP_SHARED, fdd, 0)) == MAP_FAILED) {
		printf("mmap %s error\n", dname);
		return -1;
	}
#endif

	if ((bufo = mmap(0, nboot.len, PROT_READ | PROT_WRITE, MAP_SHARED, fdo, 0)) == MAP_FAILED) {
		printf("mmap %s error\n", oname);
		return -1;
	}
	
	dst = bufo;
	memcpy(dst, bufi, leni);
	dst += leni;
#if defined DDR_TUNE
	memset(dst, 0xff, pad_ipl);
	dst += pad_ipl;
	memcpy(dst, &ddr_tune, sizeof(struct ddr_tune_desc));
	dst = bufo + IPL_LEN;
	memcpy(dst, buf_tune, lend);
	dst += lend;
	memset(dst, 0xff, pad_tune);
	dst += pad_tune;
	memcpy(dst, &nboot, sizeof(struct nboot_desc));
	dst = bufo + IPL_LEN + TUNE_LEN;
	memcpy(dst, bufs, lens);
#else
	memset(dst, 0xff, pad);
	dst += pad;
	memcpy(dst, &nboot, sizeof(struct nboot_desc));
	memcpy(bufo + IPL_LEN, bufs, lens);
#endif
	return 0;
}

