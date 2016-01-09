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
#ifndef AACDECWRAPPER_H
#define AACDECWRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 *  \brief  number of samples per frame (decoded frames)
 */

#define KAAC_NUM_SAMPLES_PER_FRAME      1024
#define KAAC_MAX_STREAMING_BUFFER_SIZE  (PVMP4AUDIODECODER_INBUFSIZE * 1)

#define KCAI_CODEC_NO_MEMORY -1
#define KCAI_CODEC_INIT_FAILURE -2

    /*----------------------------------------------------------------------------
    ; STRUCTURES TYPEDEF'S
    ----------------------------------------------------------------------------*/
	
	typedef struct
#ifdef __cplusplus
			AudioInfo_pvaacdecwapper
#endif
	{
		unsigned short Channels;
		unsigned int BitsPerSample;
		unsigned int SampleRate;
		unsigned int upsamplingFactor;
		unsigned int audioObjectType;
	} AudioInfo_pvaacdecwapper;

    typedef struct
#ifdef __cplusplus
                tPVAACDecWrapper
#endif
    {
		void *pExt;
		void *pMem;
		// number of samples per frame per channel (decoded)
        unsigned int iNumSamplesPerFrame;
        int iFirstFrame;
		unsigned int frameCount; // for parsing mode use
		
    } tPVAACDecWrapper;

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
void *PVAAC_Init(void *pAudioInfo);

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
int PVAAC_ConfigMP4(void *pAACDecInfo, unsigned char *buf, int bufsize);

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
void PVAAC_SetEnableDec(void *pAACDecInfo, int enableDec);
#if 0
void PVAAC_PVADTSSetOn(void *pAACDecInfo);
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
void PVAAC_SetframeCount(void *pAACDecInfo, unsigned int frameCount);

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
unsigned int PVAAC_GetframeCount(void *pAACDecInfo);

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
int PVAAC_GetNch(void *pAACDecInfo);

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
int PVAAC_GetSR(void *pAACDecInfo);

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
int PVAAC_hasSBRData(void *pAACDecInfo);

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
void PVAAC_resetSBRData(void *pAACDecInfo);

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
int PVAAC_hasPSData(void *pAACDecInfo);

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
int PVAAC_GetdesiredChannels(void *pAACDecInfo);

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
void PVAAC_Finalize(void *pAACDecInfo);

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVAACDecode]
 * -----------------------------------------------------------------
 * FUNCTION: decode a block (chunk) of aac frames 
 *	     with packetvideo aac decoder
 *
 * PARAMETERS: 
 *		 unsigned char *pStart : aac raw frame data start
 *		 unsigned char *pEnd : aac raw frame data end
 *		 unsigned char *poutbuf : buffer for pcm data output
 *		 unsigned int *pUndecodedbytes : undecoded bytes of aac raw frame data
 *		 void *pMP3DecInfo : aac decoder content
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
		unsigned int NumOfSamplesNeed);

#ifdef __cplusplus
}
#endif

#endif

