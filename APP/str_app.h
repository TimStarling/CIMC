#ifndef __STRAPP_H__
#define __STRAPP_H__

#include "bsp_system.h"
void rtc_reset(int year,int month,int day,int hour,int minite,int secend);
void system_test();
void read_Ratio();
void show_conf();
void set_Ratio(char *Ratio_data);
void read_Limit();
void set_Limit(char *Limit_data);
void config_save();
void config_read();
void hide_to_unix();
#endif
