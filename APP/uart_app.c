#include "uart_app.h"
#include "str_app.h"

extern system_parameter sp;
uint8_t data_recv = 0;  // ����
uint8_t uart_data[50] = "";
/************************************************************ 
 * Function :       uart0_init
 * Comment  :       ���ڳ�ʼ��usart0 �������
 ������115200
************************************************************/
void uart0_init(void)
{
    // GPIOʱ��ʹ��
    rcu_periph_clock_enable(RCU_GPIOA);
    // USARTʱ��ʹ��
    rcu_periph_clock_enable(RCU_USART0);
    
	gpio_af_set(GPIOA, GPIO_AF_7, GPIO_PIN_9 | GPIO_PIN_10);				//���ö˿ڸ���
	
	gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);  		//�˿���������Ϊ����
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);  

	gpio_mode_set(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO_PIN_10);  	//�˿���������Ϊ����	
	gpio_output_options_set(GPIOA, GPIO_MODE_ANALOG, GPIO_OSPEED_50MHZ, GPIO_PIN_10); 
    
    // ���ô��ڵĹ�������
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200U);                        // ������
    usart_word_length_set(USART0, USART_WL_8BIT);               // ֡�����ֳ�
    usart_stop_bit_set(USART0, USART_STB_1BIT);                 // ֹͣλ
    usart_parity_config(USART0, USART_PM_NONE);                 // ��żУ��λ
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);  // Ӳ��������RTS
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);  // Ӳ��������CTS
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);         // ʹ�ܽ���
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);       // ʹ�ܷ���
    usart_enable(USART0);                                       // ʹ�ܴ���    
    
    // ʹ��USART�ж�
    nvic_irq_enable(USART0_IRQn, 0, 0);
    // ʹ�ܴ��ڽ����ж�
    usart_interrupt_enable(USART0, USART_INT_RBNE);
}


/**
 @brief ����д������
 @param pData -[in] д������
 @param dataLen -[in] д�����ݳ���
 @return ��
*/
void UART_Write(uint8_t *pData, uint32_t dataLen)
{
    uint8_t i;	
    for(i = 0; i < dataLen; i++)
    {
        usart_data_transmit(USART0, pData[i]);                  // ����һ���ֽ�����
        while(RESET == usart_flag_get(USART0, USART_FLAG_TBE)); // ��������ж�
    }
}

/**
  * @brief �ض���c�⺯��printf��USARTx
  * @retval None
  */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
    return ch;
}

/**
  * @brief �ض���c�⺯��getchar,scanf��USARTx
  * @retval None
  */
int fgetc(FILE *f)
{
    uint8_t ch = 0;
    ch = usart_data_receive(USART0);
    return ch;
}

/************************************************************ 
	���ܣ��жϴ�����
	dataΪ���յ�������
	��ǰ���ܣ��յ��ַ���������ͬ�ַ��������յ���123456�����ڷ��͡�123456��
************************************************************/
void process_data(uint8_t data)
{
	uart_data[sp.uart_guide] = data;// �������ݴ�����ʱ����
	sp.uart_guide++;
}

/*
	���ܣ����ڽ�������
	���ⳤ���ַ�������uart_buff���Ա���к�������
*/
void uart_proc()
{
	static int rtc_set_flag = 0;//rtc�������ñ��� ��ʱ������ģʽ����1
	static int ratio_set_flag = 0;//ratio�������ñ��� ��ʱ������ģʽ����1
	static int limit_set_flag = 0;//limit�������ñ��� ��ʱ������ģʽ����1
	if(strcmp((char *)uart_data,"")!=0)
	{
		strcpy((char *)sp.uart_buff,(char *)uart_data);//��ʱ�������봮�ڴ洢��
		/*-----------------�������Զ��岿��-----------------*/
		
		if(strstr((char *)sp.uart_buff, "RTC Config") != NULL)
        {
            printf("Input Datetime\r\n");
			      rtc_set_flag=1;
        }
		if((strstr((char *)sp.uart_buff, "2025-06")!= NULL) && rtc_set_flag==1){
			//---------------------����ʱ��
			int year, month, day, hour, minite, secend;//2025-06-15 12:12:12
			year = (sp.uart_buff[2] - 0x30)*10 + (sp.uart_buff[3] - 0x30);
			month = (sp.uart_buff[5] - 0x30)*10 + (sp.uart_buff[6] - 0x30);
			day = (sp.uart_buff[8] - 0x30)*10 + (sp.uart_buff[9] - 0x30);
			hour = (sp.uart_buff[11] - 0x30)*10 + (sp.uart_buff[12] - 0x30);
			minite = (sp.uart_buff[14] - 0x30)*10 + (sp.uart_buff[15] - 0x30);
			secend = (sp.uart_buff[17] - 0x30)*10 + (sp.uart_buff[18] - 0x30);
			rtc_reset(year,month, day, hour, minite, secend);
			rtc_set_flag=0;
		}
    if(strstr((char *)sp.uart_buff, "RTC now") != NULL){
      printf("Current Time:20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second);
		}
		if(strstr((char *)sp.uart_buff, "test") != NULL){
			system_test();
		}
		if((strstr((char *)sp.uart_buff, "conf") != NULL) && (strstr((char *)sp.uart_buff, "e") == NULL)){
			show_conf();
		}
		if(strstr((char *)sp.uart_buff, "ratio") != NULL){
			read_Ratio();
			ratio_set_flag = 1;
		}
		if(strstr((char *)sp.uart_buff, "limit") != NULL){
			read_Limit();
			limit_set_flag = 1;
		}
		if(strstr((char *)sp.uart_buff, ".") != NULL){
			if(ratio_set_flag==1)
			{
				set_Ratio((char *)sp.uart_buff);
				ratio_set_flag=0;
			}
			else if(limit_set_flag==1)
			{
				set_Limit((char *)sp.uart_buff);
				limit_set_flag=0;
			}
		}
		if(strstr((char *)sp.uart_buff, "config save") != NULL){
			config_save();
		}
		if(strstr((char *)sp.uart_buff, "config read") != NULL){
			config_read();
		}
		if(strstr((char *)sp.uart_buff, "start") != NULL){
			sp.led_flash = 1;
			printf("Periodic Sampling\r\n");
			printf("sample cycle: %ds\r\n",sp.sample_cycle);
		}
		if(strstr((char *)sp.uart_buff, "stop") != NULL){
			sp.led_flash = 0;
			printf("Periodic Sampling STOP\r\n");
		}
		if(strstr((char *)sp.uart_buff, "hide") != NULL){
            sp.encrypt_flag = 1;
        }
        if(strstr((char *)sp.uart_buff, "unhide") != NULL){
            sp.encrypt_flag = 0;
    }
		/*-----------------�������Զ��岿��-----------------*/
		strcpy((char *)sp.uart_buff,"");
	}
	strcpy((char *)uart_data,"");//��ʱ��������
	sp.uart_guide=0;//����ָ������
	

}

/************************************************************ 
 * Function :       USART0_IRQHandler
 * Comment  :       �����жϷ��������������մ�������
 * Parameter:       null
 * Return   :       null
 * Author   :       Lingyu Meng
 * Date     :       2025-03-14 V0.2 original
************************************************************/
void USART0_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
	{			
		data_recv = usart_data_receive(USART0);    //  ���մ�������
		usart_interrupt_flag_clear(USART0, USART_INT_FLAG_RBNE); 	//��������жϱ�־λ
        process_data(data_recv); 		//��������
				
    }    
}




