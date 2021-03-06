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
 * Copyright (C) Cisco Systems Inc. 2000, 2001.  All Rights Reserved.
 * 
 * Contributor(s): 
 *		Dave Mackie		dmackie@cisco.com
 *		Bill May 		wmay@cisco.com
 */

#ifndef __MEDIA_FRAME_H__
#define __MEDIA_FRAME_H__

#include <sys/types.h>
#include "mpeg4ip_sdl_includes.h"
#include "media_time.h"

typedef u_int16_t MediaType;

#define UNDEFINEDFRAME 		0

#define PCMAUDIOFRAME		1
#define MP3AUDIOFRAME 		2
#define AACAUDIOFRAME 		3
#define VORBISAUDIOFRAME	5
#define AMRNBAUDIOFRAME         6
#define AMRWBAUDIOFRAME         7

#define YUVVIDEOFRAME		11
#define RGBVIDEOFRAME		12
#define MPEG4VIDEOFRAME		14
#define H26LVIDEOFRAME          13
#define RECONSTRUCTYUVVIDEOFRAME 	15
#define H261VIDEOFRAME          16
#define MPEG2VIDEOFRAME         17
#define RAWPCMAUDIOFRAME	18
#define H263VIDEOFRAME          19

typedef void (*media_free_f)(void *);

class CMediaFrame {
public:
	CMediaFrame(
		MediaType type = UNDEFINEDFRAME, 
		void* pData = NULL, 
		u_int32_t dataLength = 0, 
		Timestamp dts = 0, 
		Duration duration = 0, 
		u_int32_t durationScale = TimestampTicks,
		Timestamp pts = 0) {

		m_pMutex = SDL_CreateMutex();
		if (m_pMutex == NULL) {
			debug_message("CMediaFrame CreateMutex error");
		}
		m_refcnt = 1;
		m_type = type;
		m_pData = pData;
		m_dataLength = dataLength;
		m_dts = dts;
		m_pts = pts;
		m_duration = duration;
		m_durationScale = durationScale;
		m_media_free = NULL;
	}

	~CMediaFrame() {
	  if (m_refcnt != 0) abort();
	  if (m_media_free != NULL) {
	    (m_media_free)(m_pData);
	  } else {
	    free(m_pData);
	  }
	  SDL_DestroyMutex(m_pMutex);
	}

	void SetMediaFreeFunction(media_free_f m) {
	  m_media_free = m;
	};
	void AddReference(void) {
		if (SDL_LockMutex(m_pMutex) == -1) {
			debug_message("AddReference LockMutex error");
		}
		m_refcnt++;
		if (SDL_UnlockMutex(m_pMutex) == -1) {
			debug_message("AddReference UnlockMutex error");
		}
	}

	bool RemoveReference(void) {
	  u_int16_t ref;
		if (SDL_LockMutex(m_pMutex) == -1) {
			debug_message("RemoveReference LockMutex error");
		}
		m_refcnt--;
		ref = m_refcnt;
		if (SDL_UnlockMutex(m_pMutex) == -1) {
			debug_message("RemoveReference UnlockMutex error");
		}
		return ref == 0;
	}


	// predefined types of frames
	// get methods for properties

	MediaType GetType(void) {
		return m_type;
	}
	void* GetData(void) {
		return m_pData;
	}
	u_int32_t GetDataLength(void) {
		return m_dataLength;
	}
	Timestamp GetTimestamp(void) {
		return m_dts;
	}
	void SetTimestamp(Timestamp t) {
	  m_dts = t;
	}
	Timestamp GetPtsTimestamp (void) {
	  return m_pts;
	};
	void SetPtsTimestamp(Timestamp t) {
	  m_pts = t;
	};
	Duration GetDuration(void) {
		return m_duration;
	}
	void SetDuration(Duration d) {
		m_duration = d;
	}
	u_int32_t GetDurationScale(void) {
		return m_durationScale;
	}

	u_int32_t ConvertDuration(u_int32_t newScale) {
		if (m_durationScale == newScale) {
			return m_duration;	// for newer code
		}
		// for older code
		return (((m_duration * newScale) / (m_durationScale >> 1)) + 1) >> 1; 
	}

protected:
	SDL_mutex*	m_pMutex;
	u_int16_t	m_refcnt;
	MediaType	m_type;
	void* 		m_pData;
	u_int32_t 	m_dataLength;
	Timestamp	m_pts;
	Timestamp       m_dts;
	Duration 	m_duration;
	u_int32_t	m_durationScale;
	media_free_f    m_media_free;
};

#endif /* __MEDIA_FRAME_H__ */

