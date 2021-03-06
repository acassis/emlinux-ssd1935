/**
@file		gpt.h - control API for General Purpose Timer (GPT) module
@author 	shaowei@solomon-systech.com
@version 	1.0
@date		created: 25JUL06, last modified: 21AUG06
@remark		(a) three exclusive working modes: TIMER, PERIODIC, MEASURE
@		(b) add apbclk in gpt_t 
@		(c) delete frs in gpt_cfg_t
@		21 AUG 06
@		(d) Change working modes to: CAPTURE, PROFILE and SYSTIMER 
@		(e) CAPTURE: same as previous MEASURE (capture mode)
@		(f) PROFILE: time measurement, high resolution timer (compare mode, but with small pslr and handles with overflow of counter)
@		(g) SYSTIMER: system timer, used for multithreading (compare mode)
@todo		
*/

#ifndef _GPT_H_
#define _GPT_H_


/** GPT API returns */
typedef enum
{
	GPT_ERR_NONE = 0,			/**< sucessful **/
	GPT_ERR_HW = -1,			/**< hardware error **/
	GPT_ERR_PARM = -2,			/**< parameter error **/
	GPT_ERR_CFG = -3			/**< configuration error **/
} 
gpt_err_e;

/** GPT events */
typedef enum
{
	GPT_EV_COMP,				/**< compare event */
	GPT_EV_CAP				/**< capture event */
}
gpt_evt_e;

/** GPT configuration flag */
typedef enum
{
	GPT_M_SYSTEM = 1,			/**< system timer - compare, always restart counter (frs=0), always enable interrupt */
	GPT_M_PROFILE = 1 << 1,			/**< profiling timer - neither compare nor capture, free-running counter (frs=1 ), always disable compare interrupt (intr_comp=1) and compen=0 */
	GPT_M_CAPTURE = 1 << 2			/**< tigger time measure - capture */ 
}
gpt_mode_e;


/** Callback for gpt interrupt */
typedef struct
{
	void	*ctx;
	void	(*evt)(void *ctx, gpt_mode_e evt);
}
gpt_callback_t, *gpt_callback_p;

/** Context of GPT */
typedef struct
{
/** public - initialized by caller before gpt_init */
	void	*reg;				/**< logical register base address */
	/* system configuration */
	uint32_t	apbclk;			/**< apb_clk frequency */
	uint32_t	perclk;			/**< perclk freuquency: must higher than 32KHz */
	uint32_t	clk32k;	/**< 32000 or 32768 */

/** public - system timer, private - profile timer */	
	gpt_callback_t	callback;
	uint32_t	freq;			/**< tick frequency */

/** private - opaque */	
	/* version */
	uint8_t	major;				/**< major version */
	uint8_t	minor;				/**< minor version */
	/* capabilities */
	gpt_mode_e	mode;			/**< GPT working mode: system timer or profile timer */
	uint8_t	usage;				/**< how many host request to the profile timer */
}
gpt_t, *gpt_p;


/* device APIs */

gpt_err_e gpt_init( gpt_p t );
/**<
initialize GPT module.
@param[in] t	context
@return		GPT_ERR_XXX
*/

int gpt_sys_start( gpt_p t, uint32_t freq ); //, gpt_callback_p callback )
/**<
configure gpt as system timer ( HZ = 100Hz~1000Hz )
@param[in] t		context
@param[in] freq		user required frequency
//@param[in] callback	callback event and context
@return			<0, GPT_ERR_XXX; 0, choose clk_32k; 1, choose perclk
@remark			If no reset before setting compare value, the first clk period would not be accurate.
*/

gpt_err_e gpt_profile_start( gpt_p t, uint32_t *tick );
/**<
configure gpt as a profile timer (free-running counter), read the current tick and increase the reference count by 1
@param[in] t		context
@param[out] tick	current tick value (counter value)
@return 		GPT_ERR_XXX
@remark			This function is used to meansure the time. If time resolution is set to 100 nanosecond (freq = 10M), 
@			the longest elapsed time it can measure is ~6.67s
*/

void gpt_profile_end( gpt_p t, uint32_t *tick );
/**<
read the current tick, and stop the profile timer if reference count decreased to 0
@param[in] t		context
@param[out] tick	current tick (counter value)
*/

uint32_t gpt_tick( gpt_p t );
/**<
read current tick (counter value)
@param[in] t	context
return counter value (ticks)
*/

#if 0
static inline uint32_t gpt_time_elapse(uint32_t stick, uint32_t etick, 
					uint32_t freq )
/**<
Calculate elaped time (in usec) based on tick value and profile timer frequency (tick frequency)
Support no more than ~6.67s if 1/freq is 100nanosec
@param[in] t		Context
@param[in] stick	Start tick
@param[in] etick	End tick
@param[in] freq		profile timer's frequency (tick frequency), unit: Hz
@return 		time value, in microsecond (usec)
*/
{
	if (!freq)
	{
		dbg("gpt: time_elapse err - div0\n");
		return 0;
	}
	if (etick > stick)
	{
		return ((etick - stick ) / (freq / 1000000));
	}
	else
	{
		return (~0 + etick - stick) / (freq / 1000000);
	}
}
#endif


gpt_err_e gpt_time_udelay( gpt_p t, uint32_t usec, gpt_callback_p callback );
/**<
Time delay using profile timer
@param[in]     t	Context
@param[in]     msec	microseconds to delay
@param[in]     callback	callback isr for timer
@return 		GPT_ERR_XXX
@remark			if callback = NULL, busy-waiting by polling
@			else, timer interrupt for callback event
*/


gpt_err_e gpt_profile_capture( gpt_p t, uint8_t edgedet, gpt_callback_p callback, uint32_t *tick );
/**<
configure the profile timer as capture
@param[in]     t		context
@param[in]     edgedet		edge detection option for capture event
@param[in]     callback		callback event and context
@param[out]    tick		initial counter value
@return GPT_ERR_XXX
*/

uint32_t gpt_compare(gpt_p t);
/**<
@param[in] t	context
@return		tick compare value for timer
*/

uint32_t gpt_capture(gpt_p t);
/**<
@param[in] t	context
@return 	catpured tick value (capture register)
*/

gpt_err_e gpt_exit( gpt_p t );
/**<
reset and disable GPT module
@param[in] t	context
@return 	GPT_ERR_XXX
*/

int gpt_isr( gpt_p t );
/**<
GPT interrupt service routine
@param[in] t	context
@return		0 = not processed, else processed
*/

int gpt_sys_ofs(gpt_p t);

#endif

