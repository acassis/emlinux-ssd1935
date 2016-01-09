#ifndef __PIU_MSG_H__
#define __PIU_MSG_H__


/* Defines the unique id to be used by ARM PIU to interact with DSP PIU interface */

/* Id used by the File IO operations */
#define PIU_FILE_QID    1
/* Id used for Audio Control interface */
#define PIU_AUDIO_QID     2
/* Id used to display configuration */
#define PIU_DISP_QID    3 
/* Id used by Player Control Interface */
#define PIU_PLYCTL_QID  4       /* change to 1: player 2.3.1; change to 4: player 2.3.3*/

/* Id used by ARM CAL Control Interface */
#define PIU_CALMCU_QID  6


/* Message between ARM side application and DSP side interface */
#define PIUMSG_I2C		0x10
#define PIUMSG_FILE		0x20
#define	PIUMSG_VPP		0x40
#define PIUMSG_PLAYER	0x80

/* SPI/I2C related message Id */
#define PIUMSG_AUDIOCTL_TX	0x13
#define PIUMSG_AUDIOCTL_RX	0x14

/* FILE IO operation related message id */
#define PIUMSG_FILE_OPEN	0x21
#define PIUMSG_FILE_CLOSE	0x22
#define PIUMSG_FILE_READ	0x23
#define PIUMSG_FILE_SEEK	0x24
#define PIUMSG_FILE_TELL	0x25
#define PIUMSG_FILE_CHOOSE	0x26
#define PIUMSG_MSFILE_OPEN	0x27
#define PIUMSG_MSFILE_NAME	0x28
#define PIUMSG_FILE_WRITE	0x29

/* VPP related message id */
#define PIUMSG_VPP_START	0x41

/* Player Control rlated message id */
#define PIUMSG_PLAYER_START	0x81
#define PIUMSG_PLAYER_STOP	0x82
#define	PIUMSG_PLAYER_CTL	0x83
#define PIUMSG_PLAYER_CTL_RSP	0x84


#endif
