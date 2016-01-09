#ifndef GPIO_FILE
#define GPIO_FILE
	#ifdef __cplusplus
		extern "C"{
	#endif

typedef void (*cb_func_ptr)(void);

void init_gpio_lib();
	
	void exit_gpio_lib();
void set_gpio_input(int port,int pin);
	void set_gpio_output(int port,int pin, int val);
/*

*/


void set_gpio_value(int port,int pin,int val);

int get_gpio_value(int port,int pin);
int register_gpio_callback(int port,int pin,int type,cb_func_ptr cb_func);
int unregister_gpio_callback(int port, int pin);


#ifdef __cplusplus
		}
	#endif
#endif
