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

typedef struct{//-------------------------全局变量
	uint8_t uart_buff[50];//串口程序临时数组
	uint8_t flash_buff[20];//写入flash的内容
	uint8_t flash_buff_old[20];//flash内的内容，方便进行后续操作
	
	uint16_t system_mode;//系统模式变量
	uint16_t year,month,day,hour,minute,second;//RTC时钟值
	uint16_t uart_guide;//串口指针
	uint16_t led_flash;//led闪烁标志位
	uint16_t sample_cycle;//采样周期
	uint16_t encrypt_flag;//加密标志位 1时数据加密
	uint16_t sample_log_num;//sample_log辅助计数函数 满10清零
	uint16_t overlimit_log_num;//overlimit_log辅助计数函数 满10清零
	uint16_t hidedata_log_num;//hidedata_log_num辅助计数函数 满10清零
	uint32_t sdcard_stortage;//存储卡大小
	uint32_t log_id; //上电次数
	
	FIL log_file; //当前日志的地址
	FIL sample_file;//sample的地址
	FIL overlimit_file;//overlimit的地址
	FIL hidedata_file;//hidedata的地址
	uint8_t log_file_val;//是否读取到
	float adc_vlot;//adc所采集电压值
	double ratio_data,limit_data;
	
}system_parameter;

typedef union{
  float f;
  uint32_t i;
}flash_data;

extern FATFS fs;
#endif


