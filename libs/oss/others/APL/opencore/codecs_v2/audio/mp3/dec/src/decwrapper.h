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
#ifndef DECWRAPPER_H
#define DECWRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif

    /*----------------------------------------------------------------------------
    ; STRUCTURES TYPEDEF'S
    ----------------------------------------------------------------------------*/

    typedef struct
#ifdef __cplusplus
                tPVMP3DecWrapper
#endif
    {
		void *pExt;
		void *pMem;
    } tPVMP3DecWrapper;

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
void *PVMP3_Init(void);

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
void PVMP3_Finalize(void *pMP3DecInfo);

/* -----------------------------------------------------------------
 * ROUTINE NAME : [PVMP3Decode]
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
		void *pMP3DecInfo);

#ifdef __cplusplus
}
#endif

#endif

