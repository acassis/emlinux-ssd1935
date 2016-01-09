/*
Description: interface for outside control

*/

//#include <linux/workqueue.h> 
#include"wm8978.h"
#include <asm/gpio.h>
#define WM8978_DEBUG 0

#if WM8978_DEBUG
#define dbg(format, arg...) \
	printk(KERN_ERR"magus-wm8978.c:%s() : "format,__FUNCTION__,##arg)
#else
#define dbg(format, arg...) do {} while (0)
#endif

  struct work_struct update_volume_work;
 int gVolume;
 void update_volume_worker_task(void)
 {
	 int value = gVolume;

	dbg("enter UpdateVolume_WorkerTask,value=%d\n",value);
	struct snd_soc_codec *codec = g_socdev->codec;
  		if(value > 255)
			value = 255;
		else if(value < 0)
			value = 0;
	value |= 1<<8; 
	wm8978_write(codec,WM8978_DACVOLL,value);
	wm8978_write(codec,WM8978_DACVOLR,value);
	dbg("exit UpdateVolume_WorkerTask\n");
}

/*
name:SetVolume
parameter:
	int value: the value to be set
	0~48:mute
	127: max volume
return: 
	the volume after being set
*/
int gVal;
int SetVolume(int value)
{
dbg("Enter SetVolume()\n");
	if(gVal == value)
	{
		dbg("the same value,value = %d!!!!!!!!!!!!!!!\n",value);
		return -1;
	}
	gVal = value;
	gVolume = value;
	schedule_work(&update_volume_work);
dbg("exit SetVolume()\n");
return value;
}
EXPORT_SYMBOL(SetVolume);
/*
return the current volume
*/
int ReadVolume()
{
	int value = 0;
struct snd_soc_codec *codec = g_socdev->codec;
	value =  wm8978_read_reg_cache(codec, WM8978_DACVOLL) & 0xff;
	return value;
}
EXPORT_SYMBOL(ReadVolume);
/*
return the max volume supports.
*/
int GetMaxVolume()
{
	return 255;
}
EXPORT_SYMBOL(GetMaxVolume);
/*
return the max volume supports.
*/
int GetMinVolume()
{
	return 0;
}
EXPORT_SYMBOL(GetMinVolume);
#if 0
/*
control inteface for FM
flag:	0 close fm
		1 open fm
*/
int fm_control(int flag)
{
	int state;
  struct snd_soc_codec *codec = g_socdev->codec;
	u16 pwr_reg = wm8978_read_reg_cache(codec, WM8987_PWR1) & 0x7f;
	u16 loutm1_reg = wm8978_read_reg_cache(codec, WM8987_LOUTM1) & 0x1ff;
	u16 routm2_reg = wm8978_read_reg_cache(codec, WM8987_ROUTM2) & 0x1ff;
	u16 montm1_reg = wm8978_read_reg_cache(codec, WM8987_MOUTM1) & 0x1ff;
	u16 mute_reg = wm8978_read_reg_cache(codec, WM8987_ADCDAC) & 0x1f7;
   switch(flag)
   {
	   case 0:
		#if DETECT_AMPLIFIER
			gpio_set_value(GPIO_NUM(3,13),0);
			g_running = 0;
		#endif
	   	 wm8978_write(codec,WM8987_ADCDAC,mute_reg|0x8);
		 wm8978_write(codec,WM8987_PWR2,0x0);
	   	 wm8978_write(codec, WM8987_PWR1,pwr_reg|0x0100);
		 wm8978_write(codec, WM8987_LOUTM1,loutm1_reg&(~(1<<7)));
		 wm8978_write(codec, WM8987_ROUTM2,routm2_reg&(~(1<<7)));
		 wm8978_write(codec, WM8987_MOUTM1,montm1_reg&(~(1<<7)));
		 break;
	  case 1:
	   	 wm8978_write(codec, WM8987_PWR1,pwr_reg|0x00c6);
		 wm8978_write(codec,WM8987_PWR2,0x01be);
		 wm8978_write(codec, WM8987_LOUTM1,loutm1_reg|(1<<7));
		 wm8978_write(codec, WM8987_ROUTM2,routm2_reg|(1<<7));	
		 wm8978_write(codec, WM8987_MOUTM1,montm1_reg|(1<<7));	
		 wm8978_write(codec,WM8987_ADCDAC,mute_reg);
		#if DETECT_AMPLIFIER	
			gpio_get_value(GPIO_NUM(3,14));
			state = gpio_get_value(GPIO_NUM(3,14));
			if(state)
				gpio_set_value(GPIO_NUM(3,13),0);
			else
				gpio_set_value(GPIO_NUM(3,13),1);
			g_running = 1;
			break;
		#endif
	  default:
	  	break;
   };
}
EXPORT_SYMBOL(fm_control);
#endif