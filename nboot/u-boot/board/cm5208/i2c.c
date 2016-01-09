#include <common.h>
#include <i2c.h>
#include "i2cr.h"
#include "i2c.h"

#define I2C_TOUT 20000
#define I2C_HAS_START(cmd)	(!((cmd) & 2))
#define I2C_HAS_END(cmd)	(!((cmd) & 1))

//#define io_rd32(a)		(*(volatile uint32_t *)&(a))
//#define io_wr32(a, d)		(*(volatile uint32_t *)&(a) = d)

/* internal functions */
static inline i2cr_p const magus_getbase_i2c(void)
{
	return (i2cr_p)MAGUS_I2C_BASE;
}

static i2c_err i2c_reset(void)
{
	int	tout = I2C_TOUT;
	i2cr_p	r;

	r = magus_getbase_i2c();
	io_wr32(r->CFGR, I2C_CFGR_RST | I2C_CFGR_EN);
	while (io_rd32(r->CFGR) & I2C_CFGR_RST)
	{
		if (!tout--)
		{
			return I2C_ERR_TOUT;
		}
	}

	return 0;
}

/* external functions */

void i2c_init(int speed, int slaveaddr)
{
	i2c_err		err;
	unsigned int	d;
	i2cr_p		reg;
	int		baud;

	reg = magus_getbase_i2c();
	d = io_rd32(reg->IDR);
	if (((d & I2C_IDR_CLID)>>16) != I2C_PCI_CLASS)
	{
//		return I2C_ERR_HW;
		return;
	}

	err = i2c_reset();
	if (err)
	{
//		return err;
		return;
	}

	io_wr32(reg->SADDR, slaveaddr);
	/* baud rate */
	baud = 64 - (speed / (32 * 32000000));
	io_wr32(reg->BRR, baud);

//	return 0;
	return;
}

int i2c_probe(uchar chip)
{
	return 0;
}

int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int	i;
	i2c_err	ret;
	i2cr_p	reg = magus_getbase_i2c();

	udelay(8000);
	io_wr32(reg->SADDR, chip);

	/* dummy write */
	io_wr32(reg->OPR, 0x15);

	ret = i2c_wait();
	if (ret == I2C_ERR_TOUT)
	{
		goto out;
	}

	io_wr32(reg->DATA, addr);
	io_wr32(reg->OPR, 0x12);

	ret = i2c_wait();
	if (ret == I2C_ERR_TOUT)
	{
		goto out;
	}

	/* read op */
	io_wr32(reg->OPR, 0x1D);

	ret = i2c_wait();
	if (ret == I2C_ERR_TOUT)
	{
		goto out;
	}

	for (i = 0; i < len - 1; i++)
	{
		io_wr32(reg->OPR, 0x1A);
		ret = i2c_wait();
		if (ret == I2C_ERR_TOUT)
		{
			goto out;
		}

		buffer[i] = io_rd32(reg->DATA);
	}

	io_wr32(reg->OPR, 0xB);
	ret = i2c_wait();
	if (ret == I2C_ERR_TOUT)
	{
		goto out;
	}

	buffer[i] = io_rd32(reg->DATA);
	return 0;

out:
	printf("i2c_read: timeout\n");
	return ret;
}

int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int	i;
	i2c_err	ret;
	i2cr_p	reg = magus_getbase_i2c();

	udelay(8000);
	io_wr32(reg->SADDR, chip);

	/* start */
	io_wr32(reg->OPR, 0x15);
	
	ret = i2c_wait();
	if (ret == I2C_ERR_TOUT)
	{
		goto out;
	}

	/* data addr */
	io_wr32(reg->DATA, addr);
	io_wr32(reg->OPR, 0x12);

	ret = i2c_wait();
	if (ret == I2C_ERR_TOUT)
	{
		goto out;
	}

	for (i = 0; i < len - 1; i++)
	{
		io_wr32(reg->DATA, buffer[i]);
		io_wr32(reg->OPR, 0x12);

		ret = i2c_wait();
		if (ret == I2C_ERR_TOUT)
		{
			goto out;
		}
	}
	
	/* last data */
	io_wr32(reg->DATA, buffer[i]);
	io_wr32(reg->OPR, 0x13);

	ret = i2c_wait();
	if (ret == I2C_ERR_TOUT)
	{
		goto out;
	}

	return 0;

out:
	printf("i2c_write: timeout\n");
	return ret;
}

#if 0
void i2c_exit(void)
{
	i2cr_p reg = magus_getbase_i2c;
	io_wr32(reg->CFGR, 0);
}
#endif
i2c_err i2c_wait(void)
{
	int	tout = I2C_TOUT;
	i2cr_p	reg = magus_getbase_i2c();

        while (!(io_rd32(reg->ISR) & I2C_INT_RDY))
        {
                if (!tout--)
                {
                        return I2C_ERR_TOUT;
                }
        }
	io_wr32(reg->ISR, 0x1);

        return 0;

}

