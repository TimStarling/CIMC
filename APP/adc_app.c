#include "adc_app.h"
#include "str_app.h"
extern system_parameter sp;
uint32_t adc_print_slowdown = 0;
char time_print[50];

/************************************************************ 
 * Function :       ADC_Init
 * Comment  :       用于初始化ADC（不使用dma）
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void ADC_Init(void)
{
    adc_deinit();    // 复位ADC
	
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);    	// 使能连续转换模式
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);   			// 数据右对齐 
    adc_channel_length_config(ADC0, ADC_ROUTINE_CHANNEL, 1);  			// 通道配置，规则组

    adc_routine_channel_config(ADC0, 0, ADC_CHANNEL_10, ADC_SAMPLETIME_56);   // 对规则组进行配置

    adc_external_trigger_source_config(ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_INSERTED_T0_CH3);   // ADC 触发配置，利用定时器触发
    adc_external_trigger_config(ADC0, ADC_ROUTINE_CHANNEL, ENABLE);   							  // 启用触发
	
    adc_enable(ADC0);   		// 使能ADC接口
	
    delay_1ms(1);  				// 等待1ms

    adc_calibration_enable(ADC0);    // ADC校准和复位ADC校准
}



/****************************End*****************************/

/************************************************************ 
 * Function :       ADC_Init
 * Comment  :       用于初始化ADC（不使用dma）
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-02-30 V0.1 original
************************************************************/

void ADC_port_init(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);   // GPIOC时钟使能
	rcu_periph_clock_enable(RCU_ADC0);    // 使能ADC时钟
	
	gpio_mode_set(GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);   // 配置PC0为模拟输入
	
	adc_clock_config(ADC_ADCCK_PCLK2_DIV8);   // adc时钟配置
	
	ADC_Init();  // ADC配置
	
	adc_software_trigger_enable(ADC0, ADC_ROUTINE_CHANNEL); //  规则采样软件触发
	

}

/*
    ADC采样函数
*/
void adc_proc()
{
    adc_flag_clear(ADC0,ADC_FLAG_EOC);                  //  清除结束标志
    while(SET != adc_flag_get(ADC0,ADC_FLAG_EOC)){}      //  获取转换结束标志
        
    int adc_input = ADC_RDATA(ADC0);                        // 读取ADC数据
    sp.adc_vlot = adc_input*sp.ratio_data/4095;                      // 把数字量转化为工程量
}


void print_adc_5s()
{
    //start后，打印时间和adc值
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
    //start后，打印时间和adc值
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
    //start后，打印时间和adc值
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