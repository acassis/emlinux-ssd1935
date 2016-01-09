/**
* @file cal_muc.c
* @brief CAL MCU interface API definitions.
*
* This file contains definitions of MCU CAL interface API definitions.
* @author prashanthb@solomonsystech.com
*
*/
/* file contains CAL ARM layer definitions */

/* standard includes */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* platform specific includes*/
#include "cal_interface_types.h"
#include "cal_interface_srv.h"
#include "cal_mcu.h"
#include "piu_types.h"
#include "piu_msg.h"
#include "piulib.h"

/**
*
* \b Description - Function sends audio cal init message to DSP.
* @param[in] cal_msg: pointer to audio init message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return Returns success in case DSP reads the message from shared memory region, 
          busy in case DSP was busy, str large if the stream size >64 bytes
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_open_send(cal_mcumsg_aud_init *cal_msg,S32 dspcmd_id)
{
	/* local variables */
	S32 ret;
	piu_msg_t	calmcu_piu_r;
	
	calmcu_piu_r.type = PIU_CMD;
	calmcu_piu_r.len = sizeof(cal_mcumsg_aud_init);
	memcpy(calmcu_piu_r.p, cal_msg, calmcu_piu_r.len);

	/* send message to DSP for audio decode */
	ret = piu_tx(dspcmd_id, &calmcu_piu_r);	
	if(ret)
	{
		return CAL_MCU_SNDRCV_STATUS_DSPBUSY;
	}
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function to do any postprocessing after receiving init ack message from dsp
* @param[in] cal_msg: pointer to audio init dsp message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return As of now there is no processing done in this API so by default returns success
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_open_rcv(cal_dspmsg_aud_init  **cal_msg,S32 *p_dspcmd_id)
{
	/* codec specfic things are done here, as of now nothing is done here */
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function sends audio cal decode message to DSP.
* @param[in] cal_msg: pointer to audio decode message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return Returns success in case DSP reads the message from shared memory region, 
          busy in case DSP was busy, str large if the stream size >64 bytes
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_decode_send(cal_mcumsg_aud_decode *cal_msg,S32 dspcmd_id)
{
	/* local variables */
	S32 ret;
	piu_msg_t	calmcu_piu_r;
	
	calmcu_piu_r.type = PIU_CMD;
	calmcu_piu_r.len = sizeof(cal_mcumsg_aud_decode);
	memcpy(calmcu_piu_r.p, cal_msg, calmcu_piu_r.len);

	/* send message to DSP for audio decode */
	ret = piu_tx(dspcmd_id, &calmcu_piu_r);	
	if(ret)
	{
		return CAL_MCU_SNDRCV_STATUS_DSPBUSY;
	}
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function to do any postprocessing after receiving decode ack message from dsp
* @param[in] cal_msg: pointer to audio decode dsp message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return As of now there is no processing done in this API so returns success
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_decode_rcv(cal_dspmsg_aud_decode **cal_msg,S32 *p_dspcmd_id)
{
	/* codec specfic things are done here, as of now nothing is done here */
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function sends audio cal exit message to DSP.
* @param[in] cal_msg: pointer to audio exit message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return Returns success in case DSP reads the message from shared memory region, 
          busy in case DSP was busy, str large if the stream size >64 bytes
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_close_send(cal_mcumsg_aud_exit *cal_msg,S32 dspcmd_id)
{
	/* local variables */
	S32 ret;
	piu_msg_t	calmcu_piu_r;
	
	calmcu_piu_r.type = PIU_CMD;
	calmcu_piu_r.len = sizeof(cal_mcumsg_aud_exit);
	memcpy(calmcu_piu_r.p, cal_msg, calmcu_piu_r.len);

	/* send message to DSP for audio decode */
	ret = piu_tx(dspcmd_id, &calmcu_piu_r);	
	if(ret)
	{
		return CAL_MCU_SNDRCV_STATUS_DSPBUSY;
	}
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function to do any postprocessing after receiving exit ack message from dsp
* @param[in] cal_msg: pointer to audio exit dsp message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return As of now there is no processing done in this API so by default returns success
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_close_rcv(cal_dspmsg_aud_exit **cal_msg,S32 *p_dspcmd_id)
{
	/* codec specfic things are done here, as of now nothing is done here */
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function sends audio cal set params message to DSP.
* @param[in] cal_msg: pointer to audio set params message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return Returns success in case DSP reads the message from shared memory region, 
          busy in case DSP was busy, str large if the stream size >64 bytes
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_setparams_send(cal_mcumsg_aud_setparam *cal_msg,S32 dspcmd_id)
{
	/* local variables */
	S32 ret;
	piu_msg_t	calmcu_piu_r;
	
	calmcu_piu_r.type = PIU_CMD;
	calmcu_piu_r.len = sizeof(cal_mcumsg_aud_setparam);
	memcpy(calmcu_piu_r.p, cal_msg, calmcu_piu_r.len);

	/* send message to DSP for audio decode */
	ret = piu_tx(dspcmd_id, &calmcu_piu_r);	
	if(ret)
	{
		return CAL_MCU_SNDRCV_STATUS_DSPBUSY;
	}
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function to do any postprocessing after receiving set params ack message from dsp
* @param[in] cal_msg: pointer to audio set params dsp message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return As of now there is no processing done in this API so by default returns success
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_setparams_rcv(cal_dspmsg_aud_setparam **cal_msg,S32 *p_dspcmd_id)
{
	/* codec specfic things are done here, as of now nothing is done here */
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function sends audio cal get params message to DSP.
* @param[in] cal_msg: pointer to audio get params message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return Returns success in case DSP reads the message from shared memory region, 
          busy in case DSP was busy, str large if the stream size >64 bytes
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_getparams_send(cal_mcumsg_aud_getparam *cal_msg,S32 dspcmd_id)
{
	/* local variables */
	S32 ret;
	piu_msg_t	calmcu_piu_r;
	
	calmcu_piu_r.type = PIU_CMD;
	calmcu_piu_r.len = sizeof(cal_mcumsg_aud_getparam);
	memcpy(calmcu_piu_r.p, cal_msg, calmcu_piu_r.len);

	/* send message to DSP for audio decode */
	ret = piu_tx(dspcmd_id, &calmcu_piu_r);	
	if(ret)
	{
		return CAL_MCU_SNDRCV_STATUS_DSPBUSY;
	}
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function to do any postprocessing after receiving get params ack message from dsp
* @param[in] cal_msg: pointer to audio get params dsp message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return As of now there is no processing done in this API so by default returns success
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_getparams_rcv(cal_dspmsg_aud_getparam **cal_msg,S32 *p_dspcmd_id)
{
	/* codec specfic things are done here, as of now nothing is done here */
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function sends audio cal get version message to DSP.
* @param[in] cal_msg: pointer to audio get version message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return Returns success in case DSP reads the message from shared memory region, 
          busy in case DSP was busy, str large if the stream size >64 bytes
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_getver_send(cal_mcumsg_aud_getver *cal_msg,S32 dspcmd_id)
{
	/* local variables */
	S32 ret;
	piu_msg_t	calmcu_piu_r;
	
	calmcu_piu_r.type = PIU_CMD;
	calmcu_piu_r.len = sizeof(cal_mcumsg_aud_getver);
	memcpy(calmcu_piu_r.p, cal_msg, calmcu_piu_r.len);

	/* send message to DSP for audio decode */
	ret = piu_tx(dspcmd_id, &calmcu_piu_r);	
	if(ret)
	{
		return CAL_MCU_SNDRCV_STATUS_DSPBUSY;
	}
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function to do any postprocessing after receiving get version ack message from dsp
* @param[in] cal_msg: pointer to audio get version dsp message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return As of now there is no processing done in this API so by default returns success
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_getver_rcv(cal_dspmsg_aud_getver **cal_msg,S32 *p_dspcmd_id)
{
	/* codec specfic things are done here, as of now nothing is done here */
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}
/**
*
* \b Description - Function sends video cal init message to DSP.
* @param[in] cal_msg: pointer to video init message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return Returns success in case DSP reads the message from shared memory region, 
          busy in case DSP was busy, str large if the stream size >64 bytes
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_open_send(cal_mcumsg_vid_init *cal_msg,S32 dspcmd_id)
{
	/* local variables */
	S32 ret;
	piu_msg_t	calmcu_piu_r;
	
	calmcu_piu_r.type = PIU_CMD;
	calmcu_piu_r.len = sizeof(cal_mcumsg_vid_init);
	printf("video init msg size is %d\n", calmcu_piu_r.len);
	memcpy(calmcu_piu_r.p, cal_msg, calmcu_piu_r.len);

	/* send message to DSP for video init */
	ret = piu_tx(dspcmd_id, &calmcu_piu_r);	
	if(ret)
	{
		return CAL_MCU_SNDRCV_STATUS_DSPBUSY;
	}
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}
/**
*
* \b Description - Function to do any postprocessing after receiving open ack message from dsp
* @param[in] cal_msg: pointer to video open dsp message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return As of now there is no processing done in this API so by default returns success
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_open_rcv(cal_dspmsg_vid_init  **cal_msg,S32 *p_dspcmd_id)
{
	/* codec specfic things are done here, as of now nothing is done here */
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}
/**
*
* \b Description - Function sends video cal decode message to DSP.
* @param[in] cal_msg: pointer to video deocode message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return Returns success in case DSP reads the message from shared memory region, 
          busy in case DSP was busy, str large if the stream size >64 bytes
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_decode_send(cal_mcumsg_vid_decode *cal_msg,S32 dspcmd_id)
{
	/* local variables */
	S32 ret;
	piu_msg_t	calmcu_piu_r;
	
	calmcu_piu_r.type = PIU_CMD;
	calmcu_piu_r.len = sizeof(cal_mcumsg_vid_decode);
	memcpy(calmcu_piu_r.p, cal_msg, calmcu_piu_r.len);

	/* send message to DSP for video decode */
	ret = piu_tx(dspcmd_id, &calmcu_piu_r);	
	if(ret)
	{
		return CAL_MCU_SNDRCV_STATUS_DSPBUSY;
	}
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}
/**
*
* \b Description - Function to do any postprocessing after receiving decode ack message from dsp
* @param[in] cal_msg: pointer to video decode dsp message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return As of now there is no processing done in this API so by default returns success
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_decode_rcv(cal_dspmsg_vid_decode **cal_msg,S32 *p_dspcmd_id)
{
	/* codec specfic things are done here, as of now nothing is done here */
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}
/**
*
* \b Description - Function sends video cal close message to DSP.
* @param[in] cal_msg: pointer to video close message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return Returns success in case DSP reads the message from shared memory region, 
          busy in case DSP was busy, str large if the stream size >64 bytes
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_close_send(cal_mcumsg_vid_exit *cal_msg,S32 dspcmd_id)
{
	/* local variables */
	S32 ret;
	piu_msg_t	calmcu_piu_r;
	
	calmcu_piu_r.type = PIU_CMD;
	calmcu_piu_r.len = sizeof(cal_mcumsg_vid_exit);
	memcpy(calmcu_piu_r.p, cal_msg, calmcu_piu_r.len);

	/* send message to DSP for video close */
	ret = piu_tx(dspcmd_id, &calmcu_piu_r);	
	if(ret)
	{
		return CAL_MCU_SNDRCV_STATUS_DSPBUSY;
	}
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}
/**
*
* \b Description - Function to do any postprocessing after receiving close ack message from dsp
* @param[in] cal_msg: pointer to video close dsp message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return As of now there is no processing done in this API so by default returns success
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_close_rcv(cal_dspmsg_vid_exit **cal_msg,S32 *p_dspcmd_id)
{
	/* codec specfic things are done here, as of now nothing is done here */
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}
/**
*
* \b Description - Function sends video cal set params message to DSP.
* @param[in] cal_msg: pointer to video set params message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return Returns success in case DSP reads the message from shared memory region, 
          busy in case DSP was busy, str large if the stream size >64 bytes
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_setparams_send(cal_mcumsg_vid_setparams *cal_msg,S32 dspcmd_id)
{
	/* local variables */
	S32 ret;
	piu_msg_t	calmcu_piu_r;
	
	calmcu_piu_r.type = PIU_CMD;
	calmcu_piu_r.len = sizeof(cal_mcumsg_vid_setparams);
	memcpy(calmcu_piu_r.p, cal_msg, calmcu_piu_r.len);

	/* send message to DSP for video set params */
	ret = piu_tx(dspcmd_id, &calmcu_piu_r);	
	if(ret)
	{
		return CAL_MCU_SNDRCV_STATUS_DSPBUSY;
	}
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}
/**
*
* \b Description - Function to do any postprocessing after receiving get version ack message from dsp
* @param[in] cal_msg: pointer to video set params dsp message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return As of now there is no processing done in this API so by default returns success
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_setparams_rcv(cal_dspmsg_vid_setparams **cal_msg,S32 *p_dspcmd_id)
{
	/* codec specfic things are done here, as of now nothing is done here */
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}
/**
*
* \b Description - Function sends video cal get params message to DSP.
* @param[in] cal_msg: pointer to video get params message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return Returns success in case DSP reads the message from shared memory region, 
          busy in case DSP was busy, str large if the stream size >64 bytes
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_getparams_send(cal_mcumsg_vid_getparams *cal_msg,S32 dspcmd_id)
{
	/* local variables */
	S32 ret;
	piu_msg_t	calmcu_piu_r;
	
	calmcu_piu_r.type = PIU_CMD;
	calmcu_piu_r.len = sizeof(cal_mcumsg_vid_getparams);
	memcpy(calmcu_piu_r.p, cal_msg, calmcu_piu_r.len);

	/* send message to DSP for video get params */
	ret = piu_tx(dspcmd_id, &calmcu_piu_r);	
	if(ret)
	{
		return CAL_MCU_SNDRCV_STATUS_DSPBUSY;
	}
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}
/**
*
* \b Description - Function to do any postprocessing after receiving get params ack message from dsp
* @param[in] cal_msg: pointer to video get params dsp message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return As of now there is no processing done in this API so by default returns success
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_getparams_rcv(cal_dspmsg_vid_getparams **cal_msg,S32 *p_dspcmd_id)
{
	/* codec specfic things are done here, as of now nothing is done here */
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}
/**
*
* \b Description - Function sends video cal get version message to DSP.
* @param[in] cal_msg: pointer to video get version message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return Returns success in case DSP reads the message from shared memory region, 
          busy in case DSP was busy, str large if the stream size >64 bytes
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_getver_send(cal_mcumsg_vid_getver *cal_msg,S32 dspcmd_id)
{
	/* local variables */
	S32 ret;
	piu_msg_t	calmcu_piu_r;
	
	calmcu_piu_r.type = PIU_CMD;
	calmcu_piu_r.len = sizeof(cal_mcumsg_vid_getver);
	memcpy(calmcu_piu_r.p, cal_msg, calmcu_piu_r.len);

	/* send message to DSP for video codec version */
	ret = piu_tx(dspcmd_id, &calmcu_piu_r);	
	if(ret)
	{
		return CAL_MCU_SNDRCV_STATUS_DSPBUSY;
	}
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function to do any postprocessing after receiving get version ack message from dsp
* @param[in] cal_msg: pointer to video get version dsp message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return As of now there is no processing done in this API so by default returns success
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_getver_rcv(cal_dspmsg_vid_getver **cal_msg,S32 *p_dspcmd_id)
{
	/* codec specfic things are done here, as of now nothing is done here */
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function sends PMU control message to DSP.
* @param[in] cal_msg: pointer to PMU MCU contol message
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return Returns success in case DSP reads the message from shared memory region, 
          busy in case DSP was busy, str large if the stream size >64 bytes
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_pmu_ctrl_send(cal_mcumsg_pmu_ctrl *cal_msg,S32 dspcmd_id)
{
	/* local variables */
	S32 ret;
	piu_msg_t	calmcu_piu_r;
	
	calmcu_piu_r.type = PIU_CMD;
	calmcu_piu_r.len = sizeof(cal_mcumsg_pmu_ctrl);
	memcpy(calmcu_piu_r.p, cal_msg, calmcu_piu_r.len);

	/* send message to DSP for PMU control */
	ret = piu_tx(dspcmd_id, &calmcu_piu_r);	
	if(ret)
	{
		return CAL_MCU_SNDRCV_STATUS_DSPBUSY;
	}
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/**
*
* \b Description - Function to do any processing after receiving PMU ctrl ack message from dsp
* @param[in] cal_msg: pointer to DSP PMU ctrl msg
* @param[in] dspcmd_id: process ID.
* @param[out] None
* @param[in,out] None
* @return As of now there is no processing done in this API so by default returns success
* @retval CAL_MCU_SNDRCV_STATUS_ENUM is returned.
*/
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_pmu_ctrl_rcv(cal_dspmsg_pmu_ctrl **cal_msg,S32 *p_dspcmd_id)
{
	/* as of now nothing is done here */
	return CAL_MCU_SNDRCV_STATUS_SUCESS;
}

/* nothing beyond this */

