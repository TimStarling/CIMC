#include "my_sd.h"


FATFS fs;
FIL fnew;
UINT fnum;
FRESULT res_flash;
BYTE filereadbuffer[128];
BYTE filewritebuffer[] = "�����޵о�TM�ɰ���kiana����[�����ļ�.txt]";
BYTE work[_MAX_SS];

void sdcard_init()
{
	//sd_init();
	printf("FatFs��ʼ��\r\n");
	res_flash = f_mount(&fs,"0:");
	if(res_flash == FR_NO_FILESYSTEM){
		printf("FLASHû���ļ�ϵͳ�������и�ʽ��...\r\n");
		res_flash = f_mkfs("0:",NULL,work);
		if(res_flash == FR_OK){
			printf("FLASH�ɹ���ʽ���ļ�ϵͳ\r\n");
			res_flash = f_mount(NULL,"0:");
			res_flash = f_mount(&fs,"0:");
		}
		else{
			printf("FLASHû�гɹ���ʽ��\r\n");
			while(1);
		}
	}
	else{
		printf("ϵͳ���سɹ�\r\n");
	}
	printf("�����ļ�д�����");
	/*�����ļ�*/
	res_flash = f_open(&fnew,"0:�����ļ�.txt",FA_CREATE_ALWAYS | FA_WRITE);
	if(res_flash == FR_OK){
		printf("�ļ������ɹ�������д������\r\n");
		res_flash = f_write(&fnew,filewritebuffer,sizeof(filewritebuffer),&fnum);
		if(res_flash == FR_OK){
			printf("����д��ɹ�\r\n");
		}else{
			printf("�ļ�д��ʧ��(%d)\r\n",res_flash);
		}
		f_close(&fnew);
	}
	else{
		printf("�ļ�����ʧ��");
	}
	
	/*���ļ�*/
	res_flash = f_open(&fnew,"0:�����ļ�.txt",FA_OPEN_EXISTING | FA_READ);
	if(res_flash == FR_OK){
		printf("�ļ��򿪳ɹ������Զ�������\r\n");
		res_flash = f_read(&fnew,filereadbuffer,sizeof(filereadbuffer),&fnum);
		if(res_flash == FR_OK){
			printf("���ݶ�ȡ�ɹ�,�������ݣ�%s\r\n",filereadbuffer);
		}else{
			printf("���ݶ�ȡʧ��(%d)\r\n",res_flash);
		}
		f_close(&fnew);
	}
	else{
		printf("���ļ�ʧ��");
	}
	/*ȡ�������ļ�*/
	res_flash = f_mount(NULL,"0:");
}