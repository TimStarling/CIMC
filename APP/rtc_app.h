/************************************************************
 * 版权：2025CIMC Copyright。 
 * 文件：RTC.h
 * 作者: Qiao Qin @ GigaDevice
 * 平台: 2025CIMC IHD-V04
 * 版本: Qiao Qin     2025/4/20     V0.01    original
************************************************************/

#ifndef __RTC_H
#define __RTC_H
#include "bsp_system.h"

void RTC_Init(void);	// RTC初始化
void rtc_setup(void);	// RTC时钟设置
void rtc_show_time(void);	// RTC时间
void rtc_show_alarm(void);	// RTC闹钟
uint8_t usart_input_threshold(uint32_t value);  // 用作输入值有效校验
void rtc_pre_config(void);
void rtc_setup_self(int year,int month,int day,int hour,int minite,int secend);
void RTC_Init_self(int year,int month,int day,int hour,int minite,int secend);
#endif
