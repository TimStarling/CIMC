#include "system_init.h"
extern system_parameter sp;
uint8_t Device_ID[40];

/*
    spi_flash_sector_erase(0x0);//ɾ��flash����������
    spi_flash_sector_erase(0);//����10����ַ������
    spi_flash_buffer_write("Device_ID:2025-CIMC-2025244579",0,30);//��flash_buff������д��flash
*/

void system_init()
{
    systick_config();//����ʱ��
     
    led_init();//led��ʼ��
    key_init();//������ʼ��
    uart0_init();//���ڳ�ʼ��
    printf("====system init====\r\n");
    spi_flash_init();
    delay_1ms(10);
    spi_flash_buffer_read(Device_ID,0,30);//�����豸��ַ
    printf("%s\r\n",Device_ID);
    printf("====system ready====\r\n");
    RTC_Init();//RTC��ʼ��
    ADC_port_init(); //ADC��ʼ��
    OLED_Init();//oled��ʼ�� 
    sdcard_init(); 
    log_write("system init");
    log_write("system ready"); 
}

