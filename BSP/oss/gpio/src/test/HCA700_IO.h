#ifndef _HCA700_IO_
#define _HCA700_IO_

void HCA700_IO_init();
unsigned int HCA700_get_key();
unsigned int HCA700_get_battery();
void HCA700_wifi_power_on();
void HCA700_wifi_power_off();
void HCA700_AMP_power_on();
void HCA700_AMP_power_off();
void HCA700_USB_switch_off();
void HCA700_USB_switch_on();
void HCA700_USB_function_WIFI();
void HCA700_USB_function_NONE();
void HCA700_WM8750_on();
void HCA700_WM8750_off();
void HCA700_DEBUG_LED_on();
void HCA700_DEBUG_LED_off();
void HCA700_SD_card_power_on();
void HCA700_SD_card_power_off();
unsigned short HCA700_Get_Light_Sensor();
unsigned short HCA700_Get_Battery_Level();
unsigned short HCA700_Get_Backlight_level();
void HCA700_Set_Backlight_level(unsigned char value);
#endif
