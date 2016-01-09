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
#include "aacdecwrapper.h"
#ifndef OSCL_MEM_H_INCLUDED
#include "oscl_mem.h"
#endif
#include "oscl_base.h"
#include "oscl_error_codes.h"
#include "oscl_exception.h"

#include "pvmp4audiodecoder_api.h"

#include "e_tmp4audioobjecttype.h"
extern Int PVMP4SetAudioConfig(
			tPVMP4AudioDecoderExternal *pExt, 
			void *pMem, 
			Int upsamplingFactor, 
			Int samp_rate, 
			Int num_ch, 
			tMP4AudioObjectType audioObjectType);

///#define PVAAC_UTILS_DEBUG

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAAC_Init]
 * -----------------------------------------------------------------
 * FUNCTION: initialize the decoder wrapper interface
 *
 * PARAMETERS: 
 *			void *pAudioInfo : structure pointer of AudioInfo
 *
 * RETURN:
 *		 wrapper interface content which includes
 *		 tPVAACDecoderExternal and pMem used by decoder
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
void *PVAAC_Init(void *pAudioInfo)
{
	tPVAACDecWrapper *pPVAACDecWrapper = \
		(tPVAACDecWrapper *)malloc(sizeof(tPVAACDecWrapper));
	if (pPVAACDecWrapper == NULL)
	{
#ifdef PVAAC_UTILS_DEBUG
		printf("%s(): fail to allocate tPVAACDecWrapper\n", __FUNCTION__);
#endif
		return NULL;
	}
	
	pPVAACDecWrapper->iFirstFrame = true;
	
	pPVAACDecWrapper->pExt = malloc(sizeof(tPVMP4AudioDecoderExternal));
	if (pPVAACDecWrapper->pExt == NULL)
	{
		free(pPVAACDecWrapper);
#ifdef PVAAC_UTILS_DEBUG
		printf("%s(): fail to allocate tPVMP4AudioDecoderExternal\n", __FUNCTION__);
#endif
		return NULL;
	}
	
	int32 memreq =  PVMP4AudioDecoderGetMemRequirements();
    pPVAACDecWrapper->pMem = malloc(memreq);
	if (pPVAACDecWrapper->pMem == NULL)
	{
		free(pPVAACDecWrapper->pExt);
		free(pPVAACDecWrapper);
#ifdef PVAAC_UTILS_DEBUG
		printf("%s(): fail to allocate pMem\n", __FUNCTION__);
#endif
		return NULL;
	}
	
	memset(pPVAACDecWrapper->pExt, 0, sizeof(tPVMP4AudioDecoderExternal));
	memset(pPVAACDecWrapper->pMem, 0, memreq);
	
	tPVMP4AudioDecoderExternal *pExt = (tPVMP4AudioDecoderExternal *)(pPVAACDecWrapper->pExt);
	AudioInfo_pvaacdecwapper *pAI = (AudioInfo_pvaacdecwapper *)pAudioInfo;
	
	pExt->desiredChannels          = pAI->Channels;
    pExt->inputBufferCurrentLength = 0;
    pExt->outputFormat             = OUTPUTFORMAT_16PCM_INTERLEAVED;
    pExt->repositionFlag           = TRUE;
    pExt->aacPlusEnabled           = true; /// aAacplusEnabler;  /* Dynamically enable AAC+ decoding */
    pExt->inputBufferUsedLength    = 0;
    pExt->remainderBits            = 0;
	pExt->enableDec               = true; // default is decode mode
	
	if (PVMP4AudioDecoderInitLibrary(
			(tPVMP4AudioDecoderExternal *)pPVAACDecWrapper->pExt, 
			 pPVAACDecWrapper->pMem) != 0)
	{
		free(pPVAACDecWrapper->pMem);
		free(pPVAACDecWrapper->pExt);
		free(pPVAACDecWrapper);
#ifdef PVAAC_UTILS_DEBUG
		printf("%s(): fail to PVMP4AudioDecoderInitLibrary()\n", __FUNCTION__);
#endif
		return NULL;
	}
	
	if (PVMP4SetAudioConfig(
			(tPVMP4AudioDecoderExternal *)pPVAACDecWrapper->pExt,
			pPVAACDecWrapper->pMem,
			pAI->upsamplingFactor/*upsamplingFactor*/,
			pAI->SampleRate/*samp_rate*/,
			pAI->Channels/*num_channels*/,
			(tMP4AudioObjectType)pAI->audioObjectType/*audioObjectType*/) != SUCCESS)
	{
		free(pPVAACDecWrapper->pMem);
		free(pPVAACDecWrapper->pExt);
		free(pPVAACDecWrapper);
#ifdef PVAAC_UTILS_DEBUG
		printf("%s(): fail to PVMP4SetAudioConfig()\n", __FUNCTION__);
#endif
		return NULL;
	}
	
	pPVAACDecWrapper->iNumSamplesPerFrame = KAAC_NUM_SAMPLES_PER_FRAME;
	
	pExt->desiredChannels = pExt->encodedChannels;
	pPVAACDecWrapper->iFirstFrame = false;
	PVMP4SetImplicit_channeling(pPVAACDecWrapper->pMem, 1);
///#ifdef PVAAC_UTILS_DEBUG
	printf("%s(): OK -> %d\n", __FUNCTION__, pExt->desiredChannels);
///#endif
	return pPVAACDecWrapper;
}

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAAC_ConfigMP4]
 * -----------------------------------------------------------------
 * FUNCTION: for mp4 we may need provide the audio config data
 *			 for more perciese parsing about m4a format
 *
 * PARAMETERS: 
 *		 void *pAACDecInfo : wrapper interface content
 *		 unsigned char *buf : data of audio config
 *		 int bufsize : size of audio config data
 *
 * RETURN:
 *		1 is OK
 *		0 is Fail
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
int PVAAC_ConfigMP4(void *pAACDecInfo, unsigned char *buf, int bufsize)
{
	if (pAACDecInfo == NULL)
	{
		printf("you should initialize the PacketVideo AAC Decoder Ext first\n");
		return -1;
	}
	
	tPVMP4AudioDecoderExternal *pPVAACDecoderExt = \
		(tPVMP4AudioDecoderExternal *)(((tPVAACDecWrapper *)pAACDecInfo)->pExt);
	
	pPVAACDecoderExt->pInputBuffer = buf;
	pPVAACDecoderExt->inputBufferCurrentLength = bufsize;
	pPVAACDecoderExt->inputBufferUsedLength = 0;
///	pPVAACDecoderExt->outputFrameSize = (320*1024/2);
///	pPVAACDecoderExt->pOutputBuffer = (int16 *)outBuf;
	
///	PVMP4SetAACPlus(((tPVAACDecWrapper *)pAACDecInfo)->pMem, true);
	PVMP4SetBNO(((tPVAACDecWrapper *)pAACDecInfo)->pMem, 0);
	if (PVMP4AudioDecoderConfig(pPVAACDecoderExt, 
								((tPVAACDecWrapper *)pAACDecInfo)->pMem) != SUCCESS)
		return 0;
	
///	pPVAACDecoderExt->desiredChannels = pPVAACDecoderExt->encodedChannels;
	
	return 1;
}

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAAC_SetEnableDec]
 * -----------------------------------------------------------------
 * FUNCTION: enable decode aac raw frame or just parsing aac raw frame
 *
 * PARAMETERS: 
 *		 void *pAACDecInfo : wrapper interface content
 *		 bool enableDec : enable decode
 *
 * RETURN:
 *		none
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
void PVAAC_SetEnableDec(void *pAACDecInfo, int enableDec)
{
	if (pAACDecInfo == NULL) return;
	tPVMP4AudioDecoderExternal *pPVAACDecoderExt = \
		(tPVMP4AudioDecoderExternal *)(((tPVAACDecWrapper *)pAACDecInfo)->pExt);
	pPVAACDecoderExt->enableDec = enableDec;
}

#if 0
void PVAAC_PVADTSSetOn(void *pAACDecInfo)
{
	if (pAACDecInfo == NULL) return;
	PVMP4SetAACPlus(((tPVAACDecWrapper *)pAACDecInfo)->pMem, true);
	tPVMP4AudioDecoderExternal *pPVAACDecoderExt = \
		(tPVMP4AudioDecoderExternal *)(((tPVAACDecWrapper *)pAACDecInfo)->pExt);
	pPVAACDecoderExt->aacPlusEnabled = true;
	return PVADTSSetOn(((tPVAACDecWrapper *)pAACDecInfo)->pMem);
}
#endif

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAAC_SetframeCount]
 * -----------------------------------------------------------------
 * FUNCTION: set frameCount in tPVAACDecWrapper structure
 *
 * PARAMETERS: 
 *		 void *pAACDecInfo : wrapper interface content
 *		 unsigned int frameCount : frameCount
 *
 * RETURN:
 *		none
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
void PVAAC_SetframeCount(void *pAACDecInfo, unsigned int frameCount)
{
	if (pAACDecInfo == NULL) return;
	((tPVAACDecWrapper *)pAACDecInfo)->frameCount = frameCount;
}

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAAC_GetframeCount]
 * -----------------------------------------------------------------
 * FUNCTION: get frameCount in tPVAACDecWrapper structure
 *
 * PARAMETERS: 
 *		 void *pAACDecInfo : wrapper interface content
 *
 * RETURN:
 *		none
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
unsigned int PVAAC_GetframeCount(void *pAACDecInfo)
{
	if (pAACDecInfo == NULL) return 0;
	return ((tPVAACDecWrapper *)pAACDecInfo)->frameCount;
}

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAAC_GetNch]
 * -----------------------------------------------------------------
 * FUNCTION: get num of channels of file (aac data)
 *
 * PARAMETERS: 
 *		 void *pAACDecInfo : wrapper interface content
 *
 * RETURN:
 *		none
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
int PVAAC_GetNch(void *pAACDecInfo)
{
	if (pAACDecInfo == NULL) return 0;
	return PVMP4GetNch(((tPVAACDecWrapper *)pAACDecInfo)->pMem);
}

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAAC_GetSR]
 * -----------------------------------------------------------------
 * FUNCTION: get sample rate of this file will output
 *
 * PARAMETERS: 
 *		 void *pAACDecInfo : wrapper interface content
 *
 * RETURN:
 *		0 if fail otherwise the sample rate
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
int PVAAC_GetSR(void *pAACDecInfo)
{
	if (pAACDecInfo == NULL) return 0;
	return PVMP4GetSampleRate(((tPVAACDecWrapper *)pAACDecInfo)->pMem);
}


/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAAC_hasSBRData]
 * -----------------------------------------------------------------
 * FUNCTION: check if there is any SBR data in the stream
 *
 * PARAMETERS: 
 *		 void *pAACDecInfo : wrapper interface content
 *
 * RETURN:
 *		none
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
int PVAAC_hasSBRData(void *pAACDecInfo)
{
	if (pAACDecInfo == NULL) return 0;
	tPVMP4AudioDecoderExternal *pPVAACDecoderExt = \
		(tPVMP4AudioDecoderExternal *)(((tPVAACDecWrapper *)pAACDecInfo)->pExt);
	if (pPVAACDecoderExt == NULL) return 0;
	
	return PVMP4GetSBRPresentFlag(((tPVAACDecWrapper *)pAACDecInfo)->pMem);
}

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAAC_resetSBRData]
 * -----------------------------------------------------------------
 * FUNCTION: check if there is any SBR data in the stream
 *
 * PARAMETERS: 
 *		 void *pAACDecInfo : wrapper interface content
 *
 * RETURN:
 *		none
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
void PVAAC_resetSBRData(void *pAACDecInfo)
{
	if (pAACDecInfo == NULL) return;
	tPVMP4AudioDecoderExternal *pPVAACDecoderExt = \
		(tPVMP4AudioDecoderExternal *)(((tPVAACDecWrapper *)pAACDecInfo)->pExt);
	if (pPVAACDecoderExt == NULL) return;
	
	PVMP4SetSBRPresentFlag(((tPVAACDecWrapper *)pAACDecInfo)->pMem, 0);
}

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAAC_hasPSData]
 * -----------------------------------------------------------------
 * FUNCTION: check if there is any PS data in the stream
 *
 * PARAMETERS: 
 *		 void *pAACDecInfo : wrapper interface content
 *
 * RETURN:
 *		none
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
int PVAAC_hasPSData(void *pAACDecInfo)
{
	if (pAACDecInfo == NULL) return 0;
	tPVMP4AudioDecoderExternal *pPVAACDecoderExt = \
		(tPVMP4AudioDecoderExternal *)(((tPVAACDecWrapper *)pAACDecInfo)->pExt);
	if (pPVAACDecoderExt == NULL) return 0;
	
	return PVMP4GetPSPresentFlag(((tPVAACDecWrapper *)pAACDecInfo)->pMem);
}

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAAC_GetdesiredChannels]
 * -----------------------------------------------------------------
 * FUNCTION: get the desiredChannels value
 *
 * PARAMETERS: 
 *		 void *pAACDecInfo : wrapper interface content
 *
 * RETURN:
 *		desiredChannels value
 *		-1 if error
 *
 * NOTE:
 *      
 * -----------------------------------------------------------------
 */
int PVAAC_GetdesiredChannels(void *pAACDecInfo)
{
	if (pAACDecInfo == NULL) return -1;
	tPVMP4AudioDecoderExternal *pPVAACDecoderExt = \
		(tPVMP4AudioDecoderExternal *)(((tPVAACDecWrapper *)pAACDecInfo)->pExt);
	if (pPVAACDecoderExt == NULL) return -1;
	
	return pPVAACDecoderExt->desiredChannels;
}

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAAC_Finalize]
 * -----------------------------------------------------------------
 * FUNCTION: finalize the decoder content
 *
 * PARAMETERS: 
 *		 void *pAACDecInfo : wrapper interface content
 *
 * RETURN:
 *		none
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
void PVAAC_Finalize(void *pAACDecInfo)
{
	if (pAACDecInfo == NULL) return;
	
	tPVAACDecWrapper *pPVAACDecWrapper = (tPVAACDecWrapper *)pAACDecInfo;
	if (pPVAACDecWrapper->pMem)
	{
		PVMP4AudioDecoderResetBuffer(pPVAACDecWrapper->pMem);
		free(pPVAACDecWrapper->pMem);
	}
	if (pPVAACDecWrapper->pExt) free(pPVAACDecWrapper->pExt);
	free(pPVAACDecWrapper);
///#ifdef PVAAC_UTILS_DEBUG
	printf("%s(): OK\n", __FUNCTION__);
///#endif
}

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAAC_Decode]
 * -----------------------------------------------------------------
 * FUNCTION: decode a block (chunk) of aac raw frames 
 *	     with packetvideo aac decoder
 *
 * PARAMETERS: 
 *		 unsigned char *pStart : aac raw frame data start
 *		 unsigned char *pEnd : aac raw frame data end
 *		 unsigned char *poutbuf : buffer for pcm data output
 *		 unsigned int *pUndecodedbytes : undecoded bytes of aac raw frame data
 *		 void *pAACDecInfo : aac decoder content
 *		 unsigned int NumOfSampleNeed : number of sample need
 *
 * RETURN:
 *		 size of pcm data that decoder has decoded out
 *
 * NOTE:
 * -----------------------------------------------------------------
 */
int PVAAC_Decode(
		unsigned char *pStart,
		unsigned char *pEnd,
		unsigned char *poutbuf,
		unsigned int *pUndecodedbytes,
		void *pAACDecInfo, 
		unsigned int NumOfSamplesNeed)
{
	int bytesLeft, totalBytes, outOfData, pcmSize, skipFrames;
	unsigned char *outBuf = poutbuf;
	int32 nResult = FALSE;
	unsigned int NrSampleCnt = 0;
	unsigned int outBufLen = 0x07FFFFFF;
	
	if (pAACDecInfo == NULL)
	{
		printf("you should initialize the PacketVideo AAC Decoder Ext first\n");
		return -1;
	}
	
	tPVMP4AudioDecoderExternal *pPVAACDecoderExt = \
		(tPVMP4AudioDecoderExternal *)(((tPVAACDecWrapper *)pAACDecInfo)->pExt);
	
	outOfData = skipFrames = 0;
	totalBytes = pEnd - pStart;
	bytesLeft = totalBytes;
	
	pPVAACDecoderExt->pInputBuffer = pStart;
	pPVAACDecoderExt->inputBufferCurrentLength = totalBytes;
	pPVAACDecoderExt->inputBufferUsedLength = 0;
///	pPVAACDecoderExt->outputFrameSize = (320*1024/2);
	pPVAACDecoderExt->pOutputBuffer = (int16 *)outBuf;
#ifdef AAC_PLUS
	if (pPVAACDecoderExt->pOutputBuffer)
		pPVAACDecoderExt->pOutputBuffer_plus = &pPVAACDecoderExt->pOutputBuffer[2048];
#endif
	
#ifdef PVAAC_UTILS_DEBUG
	printf("need to decode %d bytes aac raw data\n", bytesLeft);
#endif
	pcmSize = 0;
	
	// know the outBuf size in bytes
	if (pUndecodedbytes) outBufLen = *pUndecodedbytes;
	if (outBufLen == 0) outBufLen = 0x07FFFFFF;
	//
	
	do {
		if (bytesLeft > 0 && pUndecodedbytes != NULL) *pUndecodedbytes = bytesLeft;
		
#ifdef PVAAC_UTILS_DEBUG
		printf("bytes lefts : %d\n", bytesLeft);
		int i;
		for(i = 0; i < (bytesLeft > 64 ? 64 : bytesLeft); i++)
		{
			if (i != 0 && (i%8) == 0) printf("\n");
			printf("[%.2x]", pPVAACDecoderExt->pInputBuffer[i]);
		}
		printf("\n");
		printf("IN ============\n");
#endif
		
		/* decode one aac raw frame */
		nResult = PVMP4AudioDecodeFrame(pPVAACDecoderExt, ((tPVAACDecWrapper *)pAACDecInfo)->pMem);
		
#ifdef PVAAC_UTILS_DEBUG
		printf("OUT ===========\n");
#endif
		
		switch (nResult)
    	{
        	case MP4AUDEC_SUCCESS:
	            break;
	        case MP4AUDEC_INCOMPLETE_FRAME:
#ifdef PVAAC_UTILS_DEBUG
				printf("%s(1): nResult:%d\n", __FUNCTION__, nResult);
#endif
				PVMP4AudioDecoderResetBuffer(((tPVAACDecWrapper *)pAACDecInfo)->pMem);
				/* need to provide more data on next call to PVMP4AudioDecodeFrame() (if possible) */
				outOfData = 1;
        	    break;
	        case 1:
			case MP4AUDEC_INVALID_FRAME:
			case MP4AUDEC_LOST_FRAME_SYNC:
    	    default:
#ifdef PVAAC_UTILS_DEBUG
				printf("%s(2): nResult:%d\n", __FUNCTION__, nResult);
#endif
				PVMP4AudioDecoderResetBuffer(((tPVAACDecWrapper *)pAACDecInfo)->pMem);
				// 2010.01.11 :
				// reset bno to 2, otherwise nch in pMcInfo will have a chance
				// to be changed to other value (when meets corrupted file). 
				// Once it is changed, decoding process may cause problem since
				// there is no actually data there to be decoded
				PVMP4SetBNO(((tPVAACDecWrapper *)pAACDecInfo)->pMem, 2);
				PVMP4SetImplicit_channeling(((tPVAACDecWrapper *)pAACDecInfo)->pMem, 1);
				//
				bytesLeft--;
				skipFrames++;
        	    break;
	    }
		
		if (outOfData) break;
		
		/* no error */
		if (skipFrames == 0)
		{
#if 0
			printf("bytes lefts : %d\n", bytesLeft);
			int i;
			for(i = 0; i < (bytesLeft > 64 ? 64 : bytesLeft); i++)
			{
				if (i != 0 && (i%8) == 0) printf("\n");
				printf("[%.2x]", pPVAACDecoderExt->pInputBuffer[i]);
			}
			printf("\n");
			printf("IN ============\n");
#endif
			// sbr data is detected, we need offset double size
			int shift = PVMP4GetSBRPresentFlag(((tPVAACDecWrapper *)pAACDecInfo)->pMem);
			if (outBuf) outBuf += ((pPVAACDecoderExt->frameLength*4) << shift);
			pcmSize += ((pPVAACDecoderExt->frameLength*4) << shift);
#ifdef PVAAC_UTILS_DEBUG
			printf("nResult:%d, pcmSize:%d bytes, shift:%d, pPVAACDecoderExt->frameLength:%d\n", 
					nResult, pcmSize, shift, pPVAACDecoderExt->frameLength);
#endif
			bytesLeft -= pPVAACDecoderExt->inputBufferUsedLength;
///			pPVAACDecoderExt->outputFrameSize = (320*1024/2) - pcmSize/2;
			pPVAACDecoderExt->pOutputBuffer = (int16 *)outBuf;
#ifdef AAC_PLUS
			if (pPVAACDecoderExt->pOutputBuffer)
				pPVAACDecoderExt->pOutputBuffer_plus = &pPVAACDecoderExt->pOutputBuffer[2048];
#endif
			((tPVAACDecWrapper *)pAACDecInfo)->frameCount++;
			NrSampleCnt += (pPVAACDecoderExt->frameLength << shift);
		}
		else skipFrames--;
		
		pPVAACDecoderExt->pInputBuffer = pStart + totalBytes - bytesLeft;
		pPVAACDecoderExt->inputBufferCurrentLength = bytesLeft;
		pPVAACDecoderExt->inputBufferUsedLength = 0;
		
		// we finish decode out nr of samples that the caller
		// wants us to do, so just break out
///		printf("NrSampleCnt:%d, NumOfSamplesNeed:%d\n", NrSampleCnt, NumOfSamplesNeed);
		if (NrSampleCnt >= NumOfSamplesNeed) break;
		
		// check outBufLen
		if (outBufLen <= pcmSize + 2048*4) break;
		
	} while (bytesLeft > 0);
	
	if (pUndecodedbytes)
	{
		*pUndecodedbytes = bytesLeft;
		// strange case : 
		// we consider it as error in this block, reset it
		if (bytesLeft > 30*1024) *pUndecodedbytes = 0;
	}
	
#ifdef PVAAC_UTILS_DEBUG
	if (pcmSize > 0)
		printf("aac compress ratio : %d%%\n", (totalBytes - bytesLeft)*100 / pcmSize);
	printf("bytes lefts : %d\n", bytesLeft);
	#if 0
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
#endif

	return pcmSize;
}

