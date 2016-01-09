/* ------------------------------------------------------------------
 * Copyright (C) 2009 U-Media
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */

/* The include file for the codec class */
#include "pvmp3_decoder.h"
#include "decwrapper.h"
#include "oscl_error_codes.h"
#include "oscl_exception.h"
#include "pvmp3_framedecoder.h"
#include "pvmp3_seek_synch.h"

///#define PVMP3_UTILS_DEBUG

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVMP3_Init]
 * -----------------------------------------------------------------
 * FUNCTION: initialize the decoder wrapper interface
 *
 * PARAMETERS: 
 *		 none
 *
 * RETURN:
 *		 wrapper interface content which includes
 *		 tPVMP3DecoderExternal and pMem used by decoder
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
void *PVMP3_Init(void)
{
	tPVMP3DecWrapper *pPVMP3DecWrapper = \
		(tPVMP3DecWrapper *)malloc(sizeof(tPVMP3DecWrapper));
	if (pPVMP3DecWrapper == NULL)
	{
		printf("%s(): fail to allocate tPVMP3DecWrapper\n", __FUNCTION__);
		return NULL;
	}
	
	pPVMP3DecWrapper->pExt = malloc(sizeof(tPVMP3DecoderExternal));
	if (pPVMP3DecWrapper->pExt == NULL)
	{
		free(pPVMP3DecWrapper);
		printf("%s(): fail to allocate tPVMP3DecoderExternal\n", __FUNCTION__);
		return NULL;
	}
	
	int32 memreq =  pvmp3_decoderMemRequirements();
    pPVMP3DecWrapper->pMem = malloc(memreq);
	if (pPVMP3DecWrapper->pMem == NULL)
	{
		free(pPVMP3DecWrapper->pExt);
		free(pPVMP3DecWrapper);
		printf("%s(): fail to allocate pMem\n", __FUNCTION__);
		return NULL;
	}
	
	memset(pPVMP3DecWrapper->pExt, 0, sizeof(tPVMP3DecoderExternal));
	memset(pPVMP3DecWrapper->pMem, 0, memreq);
	pvmp3_InitDecoder((tPVMP3DecoderExternal *)(pPVMP3DecWrapper->pExt), 
					  pPVMP3DecWrapper->pMem);
	
	printf("%s(): OK\n", __FUNCTION__);
	
	return pPVMP3DecWrapper;
}

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVMP3_Finalize]
 * -----------------------------------------------------------------
 * FUNCTION: finalize the decoder content
 *
 * PARAMETERS: 
 *		 void *pMP3DecInfo : wrapper interface content
 *
 * RETURN:
 *		none
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
void PVMP3_Finalize(void *pMP3DecInfo)
{
	if (pMP3DecInfo == NULL) return;
	
	tPVMP3DecWrapper *pPVMP3DecWrapper = (tPVMP3DecWrapper *)pMP3DecInfo;
	if (pPVMP3DecWrapper->pMem)
	{
		pvmp3_resetDecoder(pPVMP3DecWrapper->pMem);
		free(pPVMP3DecWrapper->pMem);
	}
	if (pPVMP3DecWrapper->pExt) free(pPVMP3DecWrapper->pExt);
	free(pPVMP3DecWrapper);
	
	printf("%s(): OK\n", __FUNCTION__);
}

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVMP3_Decode]
 * -----------------------------------------------------------------
 * FUNCTION: decode a block (chunk) of mp3 frames 
 *	     with packetvideo mp3 decoder
 *
 * PARAMETERS: 
 *		 unsigned char *pStart : mp3 data start
 *		 unsigned char *pEnd : mp3 data end
 *		 unsigned char **ppPCMOutput : buffer for pcm data output
 *		 unsigned int *pUndecodedbytes : undecoded bytes of mp3 data
 *		 void *pMP3DecInfo : mp3 decoder content
 *
 * RETURN:
 *		 size of pcm data that decoder has decoded out
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
int PVMP3_Decode(
		unsigned char *pStart,
		unsigned char *pEnd,
		unsigned char **ppPCMOutput,
		unsigned int *pUndecodedbytes,
		void *pMP3DecInfo)
{
	int bytesLeft, totalBytes, outOfData, pcmSize, skipFrames;
	unsigned char *outBuf = *ppPCMOutput;
	ERROR_CODE errorCode = NO_DECODING_ERROR;
	
	if (pMP3DecInfo == NULL)
	{
		printf("you should initialize the PacketVideo MP3 Decoder Ext first\n");
		return -1;
	}
	
	tPVMP3DecoderExternal *pPVMP3DecoderExt = \
		(tPVMP3DecoderExternal *)(((tPVMP3DecWrapper *)pMP3DecInfo)->pExt);
	
	outOfData = skipFrames = 0;
	totalBytes = pEnd - pStart;
	bytesLeft = totalBytes;
	
	pPVMP3DecoderExt->pInputBuffer = pStart;
	pPVMP3DecoderExt->inputBufferCurrentLength = totalBytes;
	pPVMP3DecoderExt->inputBufferUsedLength = 0;
///	pPVMP3DecoderExt->equalizerType = flat;
	pPVMP3DecoderExt->outputFrameSize = (320*1024/2);
	pPVMP3DecoderExt->pOutputBuffer = (int16 *)outBuf;
	
#ifdef PVMP3_UTILS_DEBUG
	printf("need to decode %d bytes mp3 data\n", bytesLeft);
#endif
	pcmSize = 0;
	
	do {
		if (bytesLeft > 0) *pUndecodedbytes = bytesLeft;
		
#ifdef PVMP3_UTILS_DEBUG
		printf("bytes lefts : %d\n", bytesLeft);
		
		{
			printf("[%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\n"
				   "[%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\n", 
				   pPVMP3DecoderExt->pInputBuffer[0], pPVMP3DecoderExt->pInputBuffer[1], 
				   pPVMP3DecoderExt->pInputBuffer[2], pPVMP3DecoderExt->pInputBuffer[3], 
				   pPVMP3DecoderExt->pInputBuffer[4], pPVMP3DecoderExt->pInputBuffer[5], 
				   pPVMP3DecoderExt->pInputBuffer[6], pPVMP3DecoderExt->pInputBuffer[7], 
				   pPVMP3DecoderExt->pInputBuffer[8], pPVMP3DecoderExt->pInputBuffer[9], 
				   pPVMP3DecoderExt->pInputBuffer[10], pPVMP3DecoderExt->pInputBuffer[11], 
				   pPVMP3DecoderExt->pInputBuffer[12], pPVMP3DecoderExt->pInputBuffer[13], 
				   pPVMP3DecoderExt->pInputBuffer[14], pPVMP3DecoderExt->pInputBuffer[15]);
		}
#endif
		/* decode one MP3 frame */
		errorCode = pvmp3_framedecoder(pPVMP3DecoderExt, ((tPVMP3DecWrapper *)pMP3DecInfo)->pMem);
		
		switch (errorCode)
    	{
        	case NO_DECODING_ERROR:
///            	status = MP3DEC_SUCCESS;
	            break;
	        case NO_ENOUGH_MAIN_DATA_ERROR:
///				printf("%s(): errorCode:%d\n", __FUNCTION__, errorCode);
///    	        status = MP3DEC_INCOMPLETE_FRAME;
				/* need to provide more data on next call to pvmp3_framedecoder() (if possible) */
				outOfData = 1;
        	    break;
	        case OUTPUT_BUFFER_TOO_SMALL:
///				printf("%s(): errorCode:%d\n", __FUNCTION__, errorCode);
///            	status = MP3DEC_OUTPUT_BUFFER_TOO_SMALL;
	            /* need to provide more data on next call to pvmp3_framedecoder() (if possible) */
				outOfData = 1;
        	    break;
	        case UNSUPPORTED_LAYER:
    	    case UNSUPPORTED_FREE_BITRATE:
        	case CHANNEL_CONFIG_ERROR:
	        case SYNTHESIS_WINDOW_ERROR:
    	    case SIDE_INFO_ERROR:
        	case HUFFMAN_TABLE_ERROR:
	        case SYNCH_LOST_ERROR:
    	    default:
///				printf("%s(): errorCode:%d\n", __FUNCTION__, errorCode);
///    	        status = MP3DEC_INVALID_FRAME;
				pvmp3_resetDecoder(((tPVMP3DecWrapper *)pMP3DecInfo)->pMem);
				bytesLeft--;
#if 0
				pPVMP3DecoderExt->pInputBuffer = pStart + totalBytes - bytesLeft;
				pPVMP3DecoderExt->inputBufferCurrentLength = bytesLeft;
				pPVMP3DecoderExt->inputBufferUsedLength = 0;
#endif
				skipFrames++;
        	    break;
	    }
		
		if (outOfData) break;
		
		/* no error */
		if (skipFrames == 0)
		{
			outBuf += pPVMP3DecoderExt->outputFrameSize*2;
			pcmSize += pPVMP3DecoderExt->outputFrameSize*2;
#ifdef PVMP3_UTILS_DEBUG
			printf("pcmSize : %d bytes\n", pcmSize);
			printf("bytes lefts : %d\n", bytesLeft);
#endif
			bytesLeft -= pPVMP3DecoderExt->inputBufferUsedLength;
#if 0
			pPVMP3DecoderExt->pInputBuffer = pStart + totalBytes - bytesLeft;
			pPVMP3DecoderExt->inputBufferCurrentLength = bytesLeft;
			pPVMP3DecoderExt->inputBufferUsedLength = 0;
#endif
			pPVMP3DecoderExt->outputFrameSize = (320*1024/2) - pcmSize/2;
			pPVMP3DecoderExt->pOutputBuffer = (int16 *)outBuf;
		}
		else skipFrames--;
		
		pPVMP3DecoderExt->pInputBuffer = pStart + totalBytes - bytesLeft;
		pPVMP3DecoderExt->inputBufferCurrentLength = bytesLeft;
		pPVMP3DecoderExt->inputBufferUsedLength = 0;
		
	} while (bytesLeft > 0);
	
	*pUndecodedbytes = bytesLeft;
	
#ifdef PVMP3_UTILS_DEBUG
	if (pcmSize > 0)
		printf("mp3 compress ratio : %d%%\n", (totalBytes - bytesLeft)*100 / pcmSize);
	printf("bytes lefts : %d\n", bytesLeft);
	if (bytesLeft >= 16)
	{
		unsigned char *leftdata = pStart + totalBytes - bytesLeft;
		printf("[%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\n"
			   "[%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x][%.2x]\n", 
			   leftdata[0], leftdata[1], leftdata[2], leftdata[3], 
			   leftdata[4], leftdata[5], leftdata[6], leftdata[7], 
			   leftdata[8], leftdata[9],  leftdata[10], leftdata[11], 
			   leftdata[12], leftdata[13], leftdata[14], leftdata[15]);
	}
#endif
	
	return pcmSize;
}

