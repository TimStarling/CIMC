#ifndef __ADCAPP_H
#define __ADCAPP_H



#include "bsp_system.h"



void ADC_port_init(void); //adc端口初始化
void ADC_Init(void);     //  ADC 初始化
void adc_proc();
void print_adc_5s();
void print_adc_10s();
void print_adc_15s();
#endif



