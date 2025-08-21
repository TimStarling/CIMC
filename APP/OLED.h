/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：oled.h
 * 作者: Lingyu Meng
 * 平台: 2025CIMC IHD-V04
 * 版本: Lingyu Meng     2025/3/10     V0.02    original
************************************************************/

#ifndef __OLED_H
#define __OLED_H

/************************* 头文件 *************************/

#include "bsp_system.h"

//-----------------OLED端口定义---------------- 


#define u8 unsigned char
#define u32 unsigned int

/* 引脚定义 - 模拟IIC */
#define OLED_SCL_PORT    GPIOB
#define OLED_SCL_PIN     GPIO_PIN_8
#define OLED_SDA_PORT    GPIOB
#define OLED_SDA_PIN     GPIO_PIN_9
/* SCL和SDA引脚控制宏定义 */
#define OLED_SCLK_Set()     gpio_bit_set(OLED_SCL_PORT, OLED_SCL_PIN)
#define OLED_SCLK_Clr()     gpio_bit_reset(OLED_SCL_PORT, OLED_SCL_PIN)
#define OLED_SDIN_Set()     gpio_bit_set(OLED_SDA_PORT, OLED_SDA_PIN)
#define OLED_SDIN_Clr()     gpio_bit_reset(OLED_SDA_PORT, OLED_SDA_PIN)
#define IIC_READ_SDA  gpio_input_bit_get(OLED_SDA_PORT, OLED_SDA_PIN)


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_WaitAck(void);
void Send_Byte(u8 dat);
void OLED_WR_Byte(u8 dat,u8 mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1);
void OLED_ScrollDisplay(u8 num,u8 space);
void OLED_WR_BP(u8 x,u8 y);
void OLED_ShowPicture(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[]);
void OLED_Init(void);
		    
#endif


/****************************End*****************************/

