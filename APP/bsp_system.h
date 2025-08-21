#ifndef __BSPSYSTEM__H
#define __BSPSYSTEM__H

#include "gd32f4xx.h"
#include "gd32f4xx_libopt.h"
#include "gd32f4xx_usart.h"
#include "systick.h"
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include "string.h"
#include "OLED.h"
#include "sdio_sdcard.h"
#include "ff.h"
#include "sdcard_app.h"
#include "diskio.h"
#include "SPI_FLASH.h"

void system_init();	
void scheduler_init();
void scheduler_run();

void led_init(void);
void led_proc();
void led_flash();
void key_init(void);
void key_proc();
void oled_proc();
void OLED_Init(void);


void uart0_init(void);
void uart_proc();
void adc_proc();
void print_adc_5s();
void print_adc_10s();
void print_adc_15s();

void uart_proc();
int  fputc(int ch, FILE *f);
void RTC_Init(void);
void rtc_show_time(void);
void RTC_Init_self(int year,int month,int day,int hour,int minite,int secend);
void rtc_setup_self(int year,int month,int day,int hour,int minite,int secend);

void write_file(void);
void sdcard_init();
void sample_proc(char *str);
void overlimit_proc(char *str);
void hidedata_proc(char *str);

typedef struct{//-------------------------ȫ�ֱ���
	uint8_t uart_buff[50];//���ڳ�����ʱ����
	uint8_t flash_buff[20];//д��flash������
	uint8_t flash_buff_old[20];//flash�ڵ����ݣ�������к�������
	
	uint16_t system_mode;//ϵͳģʽ����
	uint16_t year,month,day,hour,minute,second;//RTCʱ��ֵ
	uint16_t uart_guide;//����ָ��
	uint16_t led_flash;//led��˸��־λ
	uint16_t sample_cycle;//��������
	uint16_t encrypt_flag;//���ܱ�־λ 1ʱ���ݼ���
	uint16_t sample_log_num;//sample_log������������ ��10����
	uint16_t overlimit_log_num;//overlimit_log������������ ��10����
	uint16_t hidedata_log_num;//hidedata_log_num������������ ��10����
	uint32_t sdcard_stortage;//�洢����С
	uint32_t log_id; //�ϵ����
	
	FIL log_file; //��ǰ��־�ĵ�ַ
	FIL sample_file;//sample�ĵ�ַ
	FIL overlimit_file;//overlimit�ĵ�ַ
	FIL hidedata_file;//hidedata�ĵ�ַ
	uint8_t log_file_val;//�Ƿ��ȡ��
	float adc_vlot;//adc���ɼ���ѹֵ
	double ratio_data,limit_data;
	
}system_parameter;

typedef union{
  float f;
  uint32_t i;
}flash_data;

extern FATFS fs;
#endif


