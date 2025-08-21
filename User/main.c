#include "bsp_system.h"

system_parameter sp = //全局变量初始化
{
	.system_mode = 0,
	.uart_guide = 0,
	.led_flash = 0,
	.sample_cycle = 5,
	.sample_log_num = 0,
	.overlimit_log_num = 0,
	.hidedata_log_num = 0
};

int main(void)
{
	system_init();//系统初始化
	scheduler_init();//调度器初始化
	while(1)
	{
		scheduler_run();//调度器运行
	}
	return 0;
}