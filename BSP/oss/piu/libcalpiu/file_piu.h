#ifndef _PLAYER_PIU_H_
#define _PLAYER_PIU_H_

/**
@file		player_piu.h
@author		luanxu@solomon-systech.com
@version	0.0
@todo	
*/

#define FILENAME_LEN	128
#define DSP_BIN_PATH	"/emlx-app/avdsp/local/bin"
//#define DSP_BIN_PATH	"/mnt/nfs/cal032"

extern char g_filename[FILENAME_LEN];
extern int g_loopfile;
extern char			*pL1, *pL20, *pL21, *sdram;
volatile uint32_t isplaying;

#define OPTIMIZE_FILEIO

typedef struct
{
	uint8_t	msgid;		// message id
	uint32_t	addr;	// data buf address
	int32_t		len;	// data buf length
	uint32_t	fd;		// file descripter
#ifdef OPTIMIZE_FILEIO
	uint8_t		bFileSeek;
	uint32_t	offset;
	uint16_t 	where;
#endif
}
piufile_t;	// file operation structure

typedef struct
{
	uint8_t	msgid;	// message id
	uint32_t	fd;	// file descripter
	int32_t	flen;	// file name length
	uint8_t	fname[4];	// used for file extension
}
piu_msfile_t; // data structure for media stream file open


typedef struct
{
	int		fd;	// file descripter
	char	mode[4];	// file open mode
	char	name[1];	// file name
}
fopen_t;	// file open structure

typedef struct
{
	int	rt;	// return value
	int	fd;	// file descripter
}
fclose_t;// file close structure

typedef struct
{
	int	rt;	// return value
	int	fd;	// file descripter
	uint32_t	offset;	// offset
	int	whence;	// flag for fseek
}
fseek_t;	// file seek structure

typedef struct
{
	int	rt;	// return value
	int	fd;	// file descripter
	uint32_t	offset;	// offset
}
ftell_t;	// file tell structure

typedef struct
{
	uint8_t		msgid;	// message id
	uint32_t	fd;	// file descripter
	int32_t		ret;	// return value
}
piufile_rep_t;	// file reply structure


//void piu_player_start(const char *filename);
/**<
send piu message to start ceva player
@param[in]	filename	the name of media file to be played
*/


void handle_file(void *ctx, piu_msg_p msg);
/**<
piu callback funtion to handle file io messages
@param[in]	p	piu message
@return	0 = handle file io message successfully
*/

#endif
