#ifndef __ADCAPP_H
#define __ADCAPP_H



#include "bsp_system.h"



void ADC_port_init(void); //adc�˿ڳ�ʼ��
void ADC_Init(void);     //  ADC ��ʼ��
void adc_proc();
void print_adc_5s();
void print_adc_10s();
void print_adc_15s();
#endif



