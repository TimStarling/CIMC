#include "bsp_system.h"

system_parameter sp = //ȫ�ֱ�����ʼ��
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
	system_init();//ϵͳ��ʼ��
	scheduler_init();//��������ʼ��
	while(1)
	{
		scheduler_run();//����������
	}
	return 0;
}