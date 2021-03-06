/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 * 
 * The Original Code is MPEG4IP.
 * 
 * The Initial Developer of the Original Code is Cisco Systems Inc.
 * Portions created by Cisco Systems Inc. are
 * Copyright (C) Cisco Systems Inc. 2003.  All Rights Reserved.
 * 
 * Contributor(s): 
 *		Bill May 		wmay@cisco.com
 */

#include "mp4live.h"
#include "audio_encoder.h"
#include "audio_lame.h"
#include "audio_faac.h"
#ifdef HAVE_FFMPEG
#include "audio_ffmpeg.h"
#endif

audio_encoder_table_t **audio_encoder_table = NULL;

uint32_t audio_encoder_table_size = 0;

void InitAudioEncoders (void)
{
  AddAudioEncoderTable(&lame_audio_encoder_table);
  AddAudioEncoderTable(&faac_audio_encoder_table);
#ifdef HAVE_FFMPEG
  AddAudioEncoderTable(&ffmpeg_audio_encoder_table);
  InitFFmpegAudio();
#endif
}

void AddAudioEncoderTable (audio_encoder_table_t *new_table)
{
  audio_encoder_table_size++;
  audio_encoder_table = 
    (audio_encoder_table_t **)realloc(audio_encoder_table,
				     sizeof(audio_encoder_table_t *) *
				     audio_encoder_table_size);
  audio_encoder_table[audio_encoder_table_size - 1] = new_table;

}
