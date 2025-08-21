#include "adc_app.h"
#include "str_app.h"
extern system_parameter sp;
uint32_t adc_print_slowdown = 0;
char time_print[50];

/************************************************************ 
 * Function :       ADC_Init
 * Comment  :       ���ڳ�ʼ��ADC����ʹ��dma��
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void ADC_Init(void)
{
    adc_deinit();    // ��λADC
	
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);    	// ʹ������ת��ģʽ
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);   			// �����Ҷ��� 
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 1);  			// ͨ�����ã�������

    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_10, ADC_SAMPLETIME_56);   // �Թ������������

    adc_external_trigger_source_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_INSERTED_T0_CH3);   // ADC �������ã����ö�ʱ������
    adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, ENABLE);   							  // ���ô���
	
    adc_enable(ADC0);   		// ʹ��ADC�ӿ�
	
    delay_1ms(1);  				// �ȴ�1ms

    adc_calibration_enable(ADC0);    // ADCУ׼�͸�λADCУ׼
}



/****************************End*****************************/

/************************************************************ 
 * Function :       ADC_Init
 * Comment  :       ���ڳ�ʼ��ADC����ʹ��dma��
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void ADC_port_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);   // GPIOCʱ��ʹ��
	rcu_periph_clock_enable(RCU_ADC0);    // ʹ��ADCʱ��
	
	gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);   // ����PC0Ϊģ������
	
	adc_clock_config(ADC_ADCCK_PCLK2_DIV8);   // adcʱ������
	
	ADC_Init();  // ADC����
	
	adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL); //  ��������������
	

}

/*
    ADC��������
*/
void adc_proc()
{
    adc_flag_clear(ADC0,ADC_FLAG_EOC);                  //  ���������־
    while(SET != adc_flag_get(ADC0,ADC_FLAG_EOC)){}      //  ��ȡת��������־
        
    int adc_input = ADC_RDATA(ADC0);                        // ��ȡADC����
    sp.adc_vlot = adc_input*sp.ratio_data/4095;                      // ��������ת��Ϊ������
}


void print_adc_5s()
{
    //start�󣬴�ӡʱ���adcֵ
    if(sp.led_flash && (sp.sample_cycle == 5))
    {
        if(sp.encrypt_flag == 0){
            if(sp.limit_data < sp.adc_vlot){
                sprintf(time_print,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x %0.0fV limit %0.0fV\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,sp.adc_vlot,sp.limit_data);    
				overlimit_proc(time_print);
				sprintf(time_print,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x ch0=%0.1fV OverLimit(%0.2f) !\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,sp.adc_vlot,sp.limit_data);
				printf("%s",time_print); 
			}
            else{
				sprintf(time_print,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x %0.1fV\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,sp.adc_vlot);
				sample_proc(time_print);
				sprintf(time_print,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x ch0=%0.1fV\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,sp.adc_vlot);
                printf("%s",time_print);    
            }
        }
        else hide_to_unix();
    }
}

void print_adc_10s()
{
    //start�󣬴�ӡʱ���adcֵ
    if(sp.led_flash && (sp.sample_cycle == 10))
    {
        if(sp.encrypt_flag == 0){
            if(sp.limit_data < sp.adc_vlot){
                sprintf(time_print,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x %0.0fV limit %0.0fV\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,sp.adc_vlot,sp.limit_data);    
				overlimit_proc(time_print);
				sprintf(time_print,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x ch0=%0.1fV OverLimit(%0.2f) !\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,sp.adc_vlot,sp.limit_data);
				printf("%s",time_print); 
			}
            else{
                sprintf(time_print,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x %0.1fV\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,sp.adc_vlot);
				sample_proc(time_print);
				sprintf(time_print,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x ch0=%0.1fV\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,sp.adc_vlot);
                printf("%s",time_print);
			}
        }
        else hide_to_unix();
    }
}

void print_adc_15s()
{
    //start�󣬴�ӡʱ���adcֵ
    if(sp.led_flash && (sp.sample_cycle == 15))
    {
        if(sp.encrypt_flag == 0){
            if(sp.limit_data < sp.adc_vlot){
                sprintf(time_print,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x %0.0fV limit %0.0fV\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,sp.adc_vlot,sp.limit_data);    
				overlimit_proc(time_print);
				sprintf(time_print,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x ch0=%0.1fV OverLimit(%0.2f) !\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,sp.adc_vlot,sp.limit_data);
				printf("%s",time_print); 
			}
            else{
                sprintf(time_print,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x %0.1fV\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,sp.adc_vlot);
				sample_proc(time_print);
				sprintf(time_print,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x ch0=%0.1fV\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,sp.adc_vlot);
                printf("%s",time_print);
			}
        }
        else hide_to_unix();
    }
}