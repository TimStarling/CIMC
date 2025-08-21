/************************************************************
 * ��Ȩ��2025CIMC Copyright�� 
 * �ļ���RTC.h
 * ����: Qiao Qin @ GigaDevice
 * ƽ̨: 2025CIMC IHD-V04
 * �汾: Qiao Qin     2025/4/20     V0.01    original
************************************************************/

#ifndef __RTC_H
#define __RTC_H
#include "bsp_system.h"

void RTC_Init(void);	// RTC��ʼ��
void rtc_setup(void);	// RTCʱ������
void rtc_show_time(void);	// RTCʱ��
void rtc_show_alarm(void);	// RTC����
uint8_t usart_input_threshold(uint32_t value);  // ��������ֵ��ЧУ��
void rtc_pre_config(void);
void rtc_setup_self(int year,int month,int day,int hour,int minite,int secend);
void RTC_Init_self(int year,int month,int day,int hour,int minite,int secend);
#endif
