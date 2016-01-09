#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "mapm.h"


void *mapm(unsigned int addr, unsigned int len)
{
	int 	fd;
	void	*p;

	fd = open("/dev/mem", O_SYNC | O_RDWR);
	if (fd == -1)
	{
		return MAPM_ERR_FILE;
	}
	p = fmapm(fd, addr, len);
	close(fd);
	return p;
}


void *fmapm(int fd, unsigned int addr, unsigned int len)
{
	unsigned char	*p;
	unsigned int	ofs;

	ofs = addr & (getpagesize() - 1);
	p = (unsigned char *)mmap(NULL, len + ofs, 
			PROT_READ | PROT_WRITE, MAP_SHARED, fd, addr - ofs);
	if (p == MAP_FAILED)
	{
		return MAPM_ERR_MAP;
	}
	return p + ofs;
}


int unmapm(void *addr, unsigned int len)
{
	unsigned int	ofs;

	ofs = (unsigned int)addr & (getpagesize() - 1);
	return munmap((void *)((unsigned int)addr - ofs), len + ofs);
}

