#include "my_sd.h"


FATFS fs;
FIL fnew;
UINT fnum;
FRESULT res_flash;
BYTE filereadbuffer[128];
BYTE filewritebuffer[] = "究极无敌巨TM可爱的kiana万岁[测试文件.txt]";
BYTE work[_MAX_SS];

void sdcard_init()
{
	//sd_init();
	printf("FatFs初始化\r\n");
	res_flash = f_mount(&fs,"0:");
	if(res_flash == FR_NO_FILESYSTEM){
		printf("FLASH没有文件系统，将进行格式化...\r\n");
		res_flash = f_mkfs("0:",NULL,work);
		if(res_flash == FR_OK){
			printf("FLASH成功格式化文件系统\r\n");
			res_flash = f_mount(NULL,"0:");
			res_flash = f_mount(&fs,"0:");
		}
		else{
			printf("FLASH没有成功格式化\r\n");
			while(1);
		}
	}
	else{
		printf("系统挂载成功\r\n");
	}
	printf("进行文件写入测试");
	/*创建文件*/
	res_flash = f_open(&fnew,"0:测试文件.txt",FA_CREATE_ALWAYS | FA_WRITE);
	if(res_flash == FR_OK){
		printf("文件创建成功，可以写入数据\r\n");
		res_flash = f_write(&fnew,filewritebuffer,sizeof(filewritebuffer),&fnum);
		if(res_flash == FR_OK){
			printf("数据写入成功\r\n");
		}else{
			printf("文件写入失败(%d)\r\n",res_flash);
		}
		f_close(&fnew);
	}
	else{
		printf("文件创建失败");
	}
	
	/*打开文件*/
	res_flash = f_open(&fnew,"0:测试文件.txt",FA_OPEN_EXISTING | FA_READ);
	if(res_flash == FR_OK){
		printf("文件打开成功，可以读出数据\r\n");
		res_flash = f_read(&fnew,filereadbuffer,sizeof(filereadbuffer),&fnum);
		if(res_flash == FR_OK){
			printf("数据读取成功,读出数据：%s\r\n",filereadbuffer);
		}else{
			printf("数据读取失败(%d)\r\n",res_flash);
		}
		f_close(&fnew);
	}
	else{
		printf("打开文件失败");
	}
	/*取消挂载文件*/
	res_flash = f_mount(NULL,"0:");
}