#ifndef __BACKLIGHT_H__
#define __BACKLIGHT_H__

#define BACKLIGHT_GPIO_PIN	GPIO_NUM(5, 10)

/*PWM register*/
#define PWM_CFG			2
#define PWM_CTR			5

/*PWM Device Identification Register*/
#define PWM_IDR_CLIDV		0x014;
#define PWM_IDR_CLID(d)		((d)>>16);
#define PWM_IDR_DSG(d)		(((d) >> 10) & 0x3F)
#define PWM_IDR_MAJ(d)		(((d) >> 6) & 0x0F)
#define PWM_IDR_MIN(d)		((d) & 0x0F)

/*PWM Configuration Register*/
#define PWM_CFG_SPD 	(1<<3)
#define PWM_CFG_RST	(1<<1)
#define PWM_CFG_EN		1

/*PWM Control Register*/
#define PWM_CTR_HOST	(0<<3)
#define PWM_CTR_DIRECT	(1<<3)
#define PWM_CTR_FSYNC	(1<<2)
#define PWM_CTR_APB	(1<<1)
#define PWM_CTR_32K	(0<<1)
#define PWM_CTR_EN		1

/*define the cmd*/
#define SSLBL_INTENSITY_GET	_IOR('f',1,int)
#define SSLBL_INTENSITY_SET	_IOW('f',2,int)
#define SSLBL_FREQ_DIV_SET	_IOW('f',3,int)

#endif
