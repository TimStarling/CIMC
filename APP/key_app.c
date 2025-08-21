#include "key_app.h"
extern system_parameter sp;
uint8_t key_val = 0;
uint8_t key_old = 0;
uint8_t key_down = 0;
uint8_t key_up = 0;
/*
	���ܣ�key��ʼ��
	���ţ�PE_2 PE_3 PE_4 PE_5

*/

void key_init(void)
{
	
    rcu_periph_clock_enable(RCU_GPIOE);								// ��ʼ��KEY����ʱ��
    
    gpio_mode_set(GPIOE, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP,
                 GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);		//����GPIOģʽΪ����
	
}

/*
	���ܣ�����ֵɨ�裨�ڲ�������
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
	���ܣ�����ֵ��ȡ
	ͨ������switch�����ж�Ӧ�Ĳ������޸Ķ�Ӧ�������º�ʵ�ֵĹ���
	ע�⣺�������ϵİ����Ǵ����������е�
*/
void key_proc()
{
    key_val = key_read();
    key_down = key_val & (key_old ^ key_val);
    key_up = ~key_val & (key_old ^ key_val);
    key_old = key_val;
	
	switch(key_down){
		case 1://����1�л�ģʽ
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