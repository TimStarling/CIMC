#include "system_init.h"
extern system_parameter sp;
uint8_t Device_ID[40];

/*
    spi_flash_sector_erase(0x0);//删除flash中所有数据
    spi_flash_sector_erase(0);//擦除10个地址的内容
    spi_flash_buffer_write("Device_ID:2025-CIMC-2025244579",0,30);//将flash_buff的内容写入flash
*/

void system_init()
{
    systick_config();//启动时钟
     
    led_init();//led初始化
    key_init();//按键初始化
    uart0_init();//串口初始化
    printf("====system init====\r\n");
    spi_flash_init();
    delay_1ms(10);
    spi_flash_buffer_read(Device_ID,0,30);//读出设备地址
    printf("%s\r\n",Device_ID);
    printf("====system ready====\r\n");
    RTC_Init();//RTC初始化
    ADC_port_init(); //ADC初始化
    OLED_Init();//oled初始化 
    sdcard_init(); 
    log_write("system init");
    log_write("system ready"); 
}

