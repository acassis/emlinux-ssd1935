#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mapm.h"
#include "piu_types.h"
#include "piu_msg.h"
#include "piulib.h"
#include "file_piu.h"


piu_msg_t	file_piu_msg;
piu_msg_t	file_piu_r;
piu_msg_t	msfile_piu_r;
piufile_rep_t	file_piu_repmsg;

volatile fread_ctr = 0;

//clean 071128 to avoid twice fclose problem
unsigned int filestatus[10]={0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,0xffffffff,};
//endclean

void handle_file(void *ctx, piu_msg_p msg)
{
	piufile_t	*fmsg;
	void *cmdbuf;
	FILE	*fd;
	int		freadbyte,fwritebyte;
	int ret;
int i;

//	printf(" 2");
//	fflush(stdout);
	memcpy(&file_piu_msg, msg, sizeof(file_piu_msg));
	fmsg = (piufile_t *)file_piu_msg.p;
//	printf("m %x, a %x\n", fmsg->msgid, fmsg->addr);
//	fflush(stdout);
#if 1
#if 0 /* Artemis */
	if (fmsg->addr < 0x420000)
	{
		if (fmsg->addr < 0x10000)
		{
			cmdbuf = pL1 + fmsg->addr;
		}
		else if (fmsg->addr < 0x220000)
		{
			cmdbuf = pL20 + fmsg->addr - 0x200000;
		}
		else 
		{
			cmdbuf = pL21 + fmsg->addr - 0x400000;
		}
#else /* Aphrodite */
	if (fmsg->addr <= 0x5FFFFF)
	{
		if (fmsg->addr < 0x100000)
		{
			cmdbuf = pL1 + fmsg->addr;
		}
		else if (fmsg->addr < 0x400000)
		{
			cmdbuf = pL20 + fmsg->addr - 0x200000;
		}
		else
		{
			cmdbuf = pL21 + fmsg->addr - 0x400000;
		}
#endif
//printf("cmdbuf addr %X", (uint32_t)cmdbuf);
	}
	else
#endif
	{
		cmdbuf = mapm(fmsg->addr, fmsg->len);
	}
//printf("msgid=%X paddr=%X len=%d vaddr=%X\n", fmsg->msgid, fmsg->addr, fmsg->len, (unsigned int)cmdbuf);
	switch (fmsg->msgid)
	{
		case PIUMSG_FILE_OPEN:
		{
			fopen_t *focmd = (fopen_t *)cmdbuf;

//clean
#if 1
			// 2009.04.21 :
			// avldr request the *.dlist *.dlm files directly without absolute path
			// we need to add the absolute path before these file names
			unsigned char name[strlen(focmd->name) + strlen(DSP_BIN_PATH) + 2];
			
			strcpy(name, focmd->name);
			
			if (strrchr(focmd->name, '/') == NULL && 
				((strlen(focmd->name) >= 4 && 
				  strncasecmp(&focmd->name[strlen(focmd->name) - 4], ".dlm", 4)) || 
				 (strlen(focmd->name) >= 5 && 
				  strncasecmp(&focmd->name[strlen(focmd->name) - 5], ".dlist", 5))))
			{
				strcpy(name, DSP_BIN_PATH);
				strcpy(name + strlen(DSP_BIN_PATH), "/");
				strcpy(name + strlen(DSP_BIN_PATH) + 1, focmd->name);
///#ifdef HANDLE_FILE_DEBUG
				printf("%s(): dsp file : %s\n", __FUNCTION__, name);
///#endif
			}
			//
			
			if (strcmp("r",focmd->mode) == 0 ||
				strcmp("rb",focmd->mode) == 0 ||
				strcmp("rt",focmd->mode) == 0)
				fd = fopen(name/*focmd->name*/, "rb");
			else fd = fopen(name/*focmd->name*/,focmd->mode);
#else
			if ((strcmp("r",focmd->mode)==0||
			   strcmp("rb",focmd->mode)==0||
			   strcmp("rt",focmd->mode)==0))
				fd = fopen(focmd->name, "rb");
			else fd = fopen(focmd->name,focmd->mode);
#endif
			if (fd)
			{

//clean 071128 to avoid twice fclose problem
for (i = 0 ; i<10; i++)
{
        if (filestatus[i]== 0xffffffff)
        {
                filestatus[i]=(uint32_t)fd;
                break;
        }
}
if (i==10) printf("filestatus arrany full\n");
//endclean
				file_piu_repmsg.fd = (uint32_t)fd;
//printf("fo %s mode %s fd %d\n", focmd->name, focmd->mode, file_piu_repmsg.fd);
				if (0 == strcmp(focmd->name, g_filename))
				{
					isplaying = (uint32_t)fd;
printf("Media file is playing\n");
				}
			}
			else
			{
				file_piu_repmsg.fd = -1;
				printf("can not open file %s\n", focmd->name);
			}
			file_piu_repmsg.msgid = fmsg->msgid;
			file_piu_r.type = PIU_REP;
			file_piu_r.len = sizeof(piufile_rep_t);
			memcpy(file_piu_r.p, &file_piu_repmsg, file_piu_r.len);
			break;
		}
		case PIUMSG_FILE_CLOSE:
		{
			fclose_t *fccmd = (fclose_t *)cmdbuf;
			fd = (FILE *)fccmd->fd;

			if (!fd)
			{
				printf("fc invalid file number %d\n", *((int *)(cmdbuf + 4)));
				file_piu_repmsg.ret = -1;
			}
			else
			{
//printf("fc %d\n", fccmd->fd);
				if(isplaying == fccmd->fd)
				{
					isplaying = 0;
printf("Media file is closed\n");
				}

//clean 071128 to avoid twice fclose problem
for (i = 0 ; i<10; i++)
{
        if (filestatus[i]== (uint32_t)fd)
        {
                fclose(fd);
                filestatus[i]= 0xffffffff;
                break;
        }
}
if (i==10) printf("error : fclose file twice\n");

//                              fclose(fd);
//endclean
				file_piu_repmsg.ret = 1;
			}
			file_piu_repmsg.msgid = fmsg->msgid;
			file_piu_r.type = PIU_REP;
			file_piu_r.len = sizeof(piufile_rep_t);
			memcpy(file_piu_r.p, &file_piu_repmsg, file_piu_r.len);
			break;
		}
		case PIUMSG_FILE_READ:
			///printf("%s():READ %d\n", __FUNCTION__, fmsg->len);
			fd = (FILE *)fmsg->fd;
			if (!fd)
			{
				printf("invalid file number %d\n", fmsg->fd);
				file_piu_repmsg.ret = -3;
			}
                        else
 			{
#ifdef OPTIMIZE_FILEIO
				if(fmsg->bFileSeek == 1)
				{
			//		printf("fsk, fd %d, loc %d\n",fd, fmsg->offset);
					fseek(fd, fmsg->offset, fmsg->where);
				}
#endif
		//		printf("fr l %d",fmsg->len);
	//			fflush(stdout);
				freadbyte = fread(cmdbuf, 1, fmsg->len, fd);
		//		printf("%d rl %d\n",fread_ctr,freadbyte);
		//		fflush(stdout);
				if (freadbyte)
				{
					file_piu_repmsg.ret = freadbyte;
				}
				else
				{
					if (feof(fd))
					{
						printf("end of file\n");
						file_piu_repmsg.ret = -1;
					}
					else if (ferror(fd))
					{
						printf("fread error\n");
						printf("file error\n");
						file_piu_repmsg.ret = -2;
					}
					else
					{
						printf("fread error\n");
						printf("read 0 byte of data\n");
						file_piu_repmsg.ret = -1;
					}
				}
			}
			
			file_piu_repmsg.msgid = fmsg->msgid;
			file_piu_r.type = PIU_REP;
			file_piu_r.len = sizeof(piufile_rep_t);
			memcpy(file_piu_r.p, &file_piu_repmsg, file_piu_r.len);
			break;
//clean
		case PIUMSG_FILE_WRITE:
			fd = (FILE *)fmsg->fd;
			if (!fd)
			{
				printf("invalid file number %d\n", fmsg->fd);
				file_piu_repmsg.ret = -3;
			}
                        else
			{

		//	        printf("fd= %d, start fwrite\n",fd);
		//		printf ("bufer = %s",cmdbuf);
				fwritebyte= fwrite(cmdbuf, 1, fmsg->len, fd);
//printf("fr fd %d, l %d rl %d\n",fmsg->fd, fmsg->len, freadbyte);
//printf(".");
//fflush(stdout);
				if (fwritebyte)
				{
		//		        printf("write %d bytes data\n",fwritebyte);
					file_piu_repmsg.ret = fwritebyte;
				}
				else
				{
					printf("write 0 byte of data\n");
					//*((int *)cmdbuf) = -1;
					file_piu_repmsg.ret = -1;
				}
			}
			file_piu_repmsg.msgid = fmsg->msgid;
			file_piu_r.type = PIU_REP;
			file_piu_r.len = sizeof(piufile_rep_t);
			memcpy(file_piu_r.p, &file_piu_repmsg, file_piu_r.len);
			break;

		case PIUMSG_FILE_SEEK:
		{
			fseek_t *fscmd = (fseek_t *)cmdbuf;
			fd = (FILE *)fmsg->fd;
			if (!fd)
			{
				printf("invalid file number %d\n", *((int *)(cmdbuf + 4)));
				file_piu_repmsg.ret = -1;
			}
			else
			{
				printf("fs fd %d off %ld whence %d\n", fscmd->fd, (long int)fscmd->offset, fscmd->whence);
				fflush(stdout);
				fseek(fd, fscmd->offset, fscmd->whence);
				file_piu_repmsg.ret = 1;
			}
			file_piu_repmsg.msgid = fmsg->msgid;
			file_piu_r.type = PIU_REP;
			file_piu_r.len = sizeof(piufile_rep_t);
			memcpy(file_piu_r.p, &file_piu_repmsg, file_piu_r.len);
			break;
		}
		case PIUMSG_FILE_TELL:
		{
			ftell_t *ftcmd = (ftell_t *)cmdbuf;
			fd = (FILE *)fmsg->fd;
			if (fd == NULL)
			{
				printf("invalid file number %d\n", ftcmd->fd);
				file_piu_repmsg.ret = -1;
			}
			else
			{
#ifdef OPTIMIZE_FILEIO
			//printf("ftell %d %d %d %d\n", fmsg->bFileSeek, fmsg->fd, fmsg->offset, fmsg->where);
			if(fmsg->bFileSeek == 1)
				fseek(fd, fmsg->offset, fmsg->where);
#endif
				//fseek(fd, 0, SEEK_END);
				file_piu_repmsg.ret = ftell(fd);
				if (file_piu_repmsg.ret == 0)
				{
					file_piu_repmsg.ret = -1;
				}
			//printf("ft fd %d off %ld\n", ftcmd->fd, (long int)file_piu_repmsg.ret);
			}
			file_piu_repmsg.msgid = fmsg->msgid;
			file_piu_r.type = PIU_REP;
			file_piu_r.len = sizeof(piufile_rep_t);
			memcpy(file_piu_r.p, &file_piu_repmsg, file_piu_r.len);
			break;
		}
		case PIUMSG_MSFILE_NAME:
		{
//printf("ms file name\n");
			if (g_filename)
			{
				strcpy(cmdbuf, g_filename);
printf("file name %s.\n", g_filename);
			}
			else
			{
				printf("please enter file name\n");
				fgets(g_filename, FILENAME_LEN, stdin);
				g_filename[strlen(g_filename) - 1] = '\0';
				printf("your file: %s.\n", g_filename);
			}

			file_piu_repmsg.msgid = fmsg->msgid;
			file_piu_repmsg.ret = 1;
			file_piu_r.type = PIU_REP;
			file_piu_r.len = sizeof(piufile_rep_t);
			memcpy(file_piu_r.p, &file_piu_repmsg, file_piu_r.len);
			break;
		}

		default:
			printf("unknown msg id %x\n", fmsg->msgid);
			break;
	}
	if (fmsg->addr >= 0x420000)
	{
//		printf("um\n");
		unmapm(cmdbuf, fmsg->len);
	}
//	printf("TX\n");
//	fflush(stdout);
	// send reply
	ret = piu_tx(PIU_FILE_QID, &file_piu_r);
	if (ret)
	{
		printf("piu tx error\n");
	}
//	printf(" 4");
//	fflush(stdout);
//	if(fmsg->msgid == PIUMSG_FILE_READ)
//		fread_ctr++;
}
