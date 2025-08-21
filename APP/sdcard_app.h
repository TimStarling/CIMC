#ifndef __SDCARDAPP_H__
#define __SDCARDAPP_H__

#include "bsp_system.h"

void sdcard_init();
void write_log_id_to_flash();
void read_log_id_from_flash();
void log_write(const char* write_data);  
void sample_proc(char *str);
void overlimit_proc(char *str);
void hidedata_proc(char *str);
#endif
