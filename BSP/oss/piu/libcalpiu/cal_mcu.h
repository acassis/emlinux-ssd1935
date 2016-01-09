/**
* @file cal_muc.h
* @brief CAL MCU interface header.
*
* This file encapsulates definitions, enums, structures and API 
  prototypes used in CAL layer.
* @author prashanthb@solomonsystech.com
*
*/
#ifndef CAL_MCU_H
#define CAL_MCU_H

/* ARM DSP COMMUNICATION STATUS IDS */
typedef enum
{
	CAL_MCU_SNDRCV_STATUS_IN_STR_LARGE = -3,
	CAL_MCU_SNDRCV_STATUS_DSPBUSY = -2,
	CAL_MCU_SNDRCV_STATUS_PROCESSID_INVALID = -1,
	CAL_MCU_SNDRCV_STATUS_SUCESS = 0
}CAL_MCU_SNDRCV_STATUS_ENUM;

/* enum for piu messages */
typedef enum
{
	CAL_MCU_PIUSNDMSG_FAILURE = -1,
	CAL_MCU_PIUSNDMSG_SUCESS = 0,
	CAL_MCU_PIUSNDMSG_BUSY = 1
}CAL_MCU_PIUSNDMSG_ENUM;

typedef enum
{
	CAL_MCU_PIURCVMSG_FAILURE = -1,
	CAL_MCU_PIURCVMSG_SUCESS = 0,
	CAL_MCU_PIURCVMSG_BUSY = 1
}CAL_MCU_PIURCVMSG_ENUM;

/* CAL MCU function prototypes */

/* CAL MCU function prototypes */

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_open_send(cal_mcumsg_aud_init *cal_msg,S32 dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_open_rcv(cal_dspmsg_aud_init  **cal_msg,S32 *p_dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_decode_send(cal_mcumsg_aud_decode *cal_msg,S32 dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_decode_rcv(cal_dspmsg_aud_decode **cal_msg,S32 *p_dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_close_send(cal_mcumsg_aud_exit *cal_msg,S32 dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_close_rcv(cal_dspmsg_aud_exit **cal_msg,S32 *p_dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_setparams_send(cal_mcumsg_aud_setparam *cal_msg,S32 dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_setparams_rcv(cal_dspmsg_aud_setparam **cal_msg,S32 *p_dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_getparams_send(cal_mcumsg_aud_getparam *cal_msg,S32 dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_getparams_rcv(cal_dspmsg_aud_getparam **cal_msg,S32 *p_dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_getver_send(cal_mcumsg_aud_getver *cal_msg,S32 dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_aud_getver_rcv(cal_dspmsg_aud_getver **cal_msg,S32 *p_dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_open_send(cal_mcumsg_vid_init *cal_msg,S32 dspcmd_id);
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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_open_rcv(cal_dspmsg_vid_init  **cal_msg,S32 *p_dspcmd_id);
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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_decode_send(cal_mcumsg_vid_decode *cal_msg,S32 dspcmd_id);
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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_decode_rcv(cal_dspmsg_vid_decode **cal_msg,S32 *p_dspcmd_id);
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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_close_send(cal_mcumsg_vid_exit *cal_msg,S32 dspcmd_id);
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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_close_rcv(cal_dspmsg_vid_exit **cal_msg,S32 *p_dspcmd_id);
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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_setparams_send(cal_mcumsg_vid_setparams *cal_msg,S32 dspcmd_id);
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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_setparams_rcv(cal_dspmsg_vid_setparams **cal_msg,S32 *p_dspcmd_id);
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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_getparams_send(cal_mcumsg_vid_getparams *cal_msg,S32 dspcmd_id);
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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_getparams_rcv(cal_dspmsg_vid_getparams **cal_msg,S32 *p_dspcmd_id);
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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_getver_send(cal_mcumsg_vid_getver *cal_msg,S32 dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_vid_getver_rcv(cal_dspmsg_vid_getver **cal_msg,S32 *p_dspcmd_id);

/******************************************************************************/
/* this function is used only for the demonstration purpose, when cal is      */
/* integrated into gstreamer, this needs to be changed                        */
/******************************************************************************/
void wait_for_dsp_cmd(void **cal_msg, S32 *p_dspcmd_id);


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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_pmu_ctrl_send(cal_mcumsg_pmu_ctrl *cal_msg,S32 dspcmd_id);

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
CAL_MCU_SNDRCV_STATUS_ENUM cal_mcu_pmu_ctrl_rcv(cal_dspmsg_pmu_ctrl **cal_msg,S32 *p_dspcmd_id);

#endif /* #ifndef CAL_MCU_H */

/* nothing beyond this */

