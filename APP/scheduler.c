#include "bsp_system.h"
extern system_parameter sp;
uint16_t tast_num;
typedef struct{
	void (*task_func)(void);
	uint32_t rate_ms;
	uint32_t lats_run;
} task_t;

static task_t scheduler[]={
	{led_proc,50,0},
	{key_proc,50,0},
	{oled_proc,100,0},
	{adc_proc,100,0},
	{rtc_show_time,1000,0},
	{uart_proc,200,0},
	{led_flash,500,0},
	{print_adc_5s,500,0},
	{print_adc_10s,10000,0},
	{print_adc_15s,15000,0}
};

void scheduler_init()
{
	tast_num = sizeof(scheduler)/ sizeof(task_t);
}

void scheduler_run()
{
	for(uint16_t i=0;i<tast_num;i++)
	{
		uint32_t now_time=GetSysRunTime();
		if(now_time>=scheduler[i].lats_run + scheduler[i].rate_ms){
			scheduler[i].lats_run=now_time;
			scheduler[i].task_func();
		}
	}
}