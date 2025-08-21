#include "key_app.h"
extern system_parameter sp;
uint8_t key_val = 0;
uint8_t key_old = 0;
uint8_t key_down = 0;
uint8_t key_up = 0;
/*
	功能：key初始化
	引脚：PE_2 PE_3 PE_4 PE_5

*/

void key_init(void)
{
	
    rcu_periph_clock_enable(RCU_GPIOE);								// 初始化KEY总线时钟
    
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
                 GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);		//配置GPIO模式为输入
	
}

/*
	功能：按键值扫描（内部函数）
*/
uint8_t key_read()
{
	uint8_t temp = 0;
	if(gpio_input_bit_get(GPIOE,GPIO_PIN_7) == RESET)temp = 1;
	if(gpio_input_bit_get(GPIOE,GPIO_PIN_8) == RESET)temp = 2;
	if(gpio_input_bit_get(GPIOE,GPIO_PIN_9) == RESET)temp = 3;
	if(gpio_input_bit_get(GPIOE,GPIO_PIN_10) == RESET)temp = 4;
	return temp;
}

/*
	功能：按键值读取
	通过更改switch函数中对应的部分来修改对应按键按下后实现的功能
	注意：开发板上的按键是从下往上排列的
*/
void key_proc()
{
    key_val = key_read();
    key_down = key_val & (key_old ^ key_val);
    key_up = ~key_val & (key_old ^ key_val);
    key_old = key_val;
	
	switch(key_down){
		case 1://按键1切换模式
			sp.led_flash = !sp.led_flash;
			if(sp.led_flash==0)
			{
				printf("Periodic Sampling STOP\r\n");
			}
			else 
			{
				printf("Periodic Sampling\r\n");
				printf("sample cycle: %ds\r\n",sp.sample_cycle);
			}
			break;
		case 2:
			sp.sample_cycle = 5;
			printf("sample cycle: %ds\r\n",sp.sample_cycle);
			break;
		case 3:
			sp.sample_cycle = 10;
			printf("sample cycle: %ds\r\n",sp.sample_cycle);
			break;
		case 4:
			sp.sample_cycle = 15;
			printf("sample cycle: %ds\r\n",sp.sample_cycle);
			break;
		
	}
  
}