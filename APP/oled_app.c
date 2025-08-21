#include "oled_app.h"
extern system_parameter sp;
unsigned char data[]=" ";

void oled_proc()
{
	if(sp.led_flash == 1)sp.system_mode=1;
	else sp.system_mode=0;
	switch(sp.system_mode)
	{
		case 0:
			OLED_ShowString(0,0,"system idle    ",16);	
			OLED_ShowString(0,16,"              ",16);		
			break;
		case 1:
			sprintf(data,"%0.2x:%0.2x:%0.2x   ",sp.hour,sp.minute,sp.second);
			OLED_ShowString(0,0,data,16);
			sprintf(data,"%.2fV     ",sp.adc_vlot);
			OLED_ShowString(0,16,data,16);
			break;
		case 2:
			OLED_ShowString(0,0,"Usart_data:   ",16);
			sprintf(data,"%s  ",sp.uart_buff);
			OLED_ShowString(0,16,data,16);
			break;
		case 3:
			OLED_ShowString(0,0,"FlashData:  ",16);
			sprintf(data,"%s  ",sp.flash_buff_old);
			OLED_ShowString(0,16,data,16);
//			OLED_ShowString(0,0,"SDCardData:  ",16);
//			sprintf(data,"Size:%dMB  ",sp.sdcard_stortage);
//			OLED_ShowString(0,16,data,16);
			break;
	}
	OLED_Refresh();
	
}