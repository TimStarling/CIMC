#include "led_app.h"
extern system_parameter sp;
uint8_t ucLed[4] = {0,0,0,0};

/*
	���ܣ�LED��ʼ��
	���ţ�PA_4 PA_5 PA_6 PA_7
	�͵�ƽ�� �ߵ�ƽ��
*/
void led_init(void)
{
	
	rcu_periph_clock_enable(RCU_GPIOA);    // ��ʼ��GPIO_A����ʱ��
	
	//��ʼ��LED1~4�˿ڣ�������ʼ���˿ڣ�
	gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);// GPIOģʽ����Ϊ���
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);// �����������
	gpio_bit_reset(GPIOA, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);// ���ų�ʼ��ƽΪ�͵�ƽ
	
}


/*
	���ܣ�LED������� ���ڲ�������
	ucLed[n]=0 ��Ӧ��ledϨ�� =1 ��Ӧ��led����
*/
void led_disp(uint8_t *ucLed)
{
	uint8_t temp=0x00;
	static uint8_t temp_old = 0xff;
	for(int i=0;i<4;i++){
		temp |= ucLed[i]<<i;
	}
	if(temp != temp_old){
		if(ucLed[0])gpio_bit_set(GPIOA,GPIO_PIN_4);
		else gpio_bit_reset(GPIOA,GPIO_PIN_4);
		if(ucLed[1])gpio_bit_set(GPIOA,GPIO_PIN_5);
		else gpio_bit_reset(GPIOA,GPIO_PIN_5);
		if(ucLed[2])gpio_bit_set(GPIOA,GPIO_PIN_6);
		else gpio_bit_reset(GPIOA,GPIO_PIN_6);
		if(ucLed[3])gpio_bit_set(GPIOA,GPIO_PIN_7);
		else gpio_bit_reset(GPIOA,GPIO_PIN_7);
	}
}

/*
	���ܣ�LEDִ��
	�ں����ڲ��޸�ucLed[n]��ֵ���޸�LED������
*/
void led_proc()
{
	led_disp(ucLed);
    if(sp.led_flash == 0)ucLed[0] = 0;
	if(sp.limit_data < sp.adc_vlot)ucLed[1] = 1;
	else ucLed[1] = 0;
}

void led_flash()
{
	led_disp(ucLed);
	if(sp.led_flash == 1)
	{
		ucLed[0] = !ucLed[0];
	}
}