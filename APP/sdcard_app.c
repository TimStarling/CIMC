#include "sdcard_app.h"
extern system_parameter sp;

FATFS fs;
FIL fdst;
uint16_t i = 0, count, result = 0;
UINT br, bw;

sd_card_info_struct sd_cardinfo;

BYTE buffer[128];
BYTE filebuffer[128];

ErrStatus memory_compare(uint8_t* src, uint8_t* dst, uint16_t length) 
{
    while(length --){
        if(*src++ != *dst++)
            return ERROR;
    }
    return SUCCESS;
}

void sd_fatfs_init(void)
{
    nvic_irq_enable(SDIO_IRQn, 0, 0);					// 使能SDIO中断，优先级为0
}

/**
 * @brief       通过串口打印SD卡相关信息
 * @param       无
 * @retval      无
 */
void card_info_get(void)
{
    sd_card_info_struct sd_cardinfo;      // SD卡信息结构体
    sd_error_enum status;                 // SD卡操作状态
    uint32_t block_count, block_size;
    
    // 获取SD卡信息
    status = sd_card_information_get(&sd_cardinfo);
    
    if(SD_OK == status)
    {
        printf("\r\n*** SD Card Info ***\r\n");
        
        // 打印卡类型
        switch(sd_cardinfo.card_type)
        {
            case SDIO_STD_CAPACITY_SD_CARD_V1_1:
                printf("Card Type: Standard Capacity SD Card V1.1\r\n");
                break;
            case SDIO_STD_CAPACITY_SD_CARD_V2_0:
                printf("Card Type: Standard Capacity SD Card V2.0\r\n");
                break;
            case SDIO_HIGH_CAPACITY_SD_CARD:
                printf("Card Type: High Capacity SD Card\r\n");
                break;
            case SDIO_MULTIMEDIA_CARD:
                printf("Card Type: Multimedia Card\r\n");
                break;
            case SDIO_HIGH_CAPACITY_MULTIMEDIA_CARD:
                printf("Card Type: High Capacity Multimedia Card\r\n");
                break;
            case SDIO_HIGH_SPEED_MULTIMEDIA_CARD:
                printf("Card Type: High Speed Multimedia Card\r\n");
                break;
            default:
                printf("Card Type: Unknown\r\n");
                break;
        }
        
        // 打印卡容量和块大小
        block_count = (sd_cardinfo.card_csd.c_size + 1) * 1024;
        block_size = 512;
        printf("\r\n## Device size is %dKB (%.2fGB)##", sd_card_capacity_get(), sd_card_capacity_get() / 1024.0f / 1024.0f);
        printf("\r\n## Block size is %dB ##", block_size);
        printf("\r\n## Block count is %d ##", block_count);
        
        // 打印制造商ID和产品名称
        printf("Manufacturer ID: 0x%X\r\n", sd_cardinfo.card_cid.mid);
        printf("OEM/Application ID: 0x%X\r\n", sd_cardinfo.card_cid.oid);
        
        // 打印产品名称 (PNM)
        uint8_t pnm[6];
        pnm[0] = (sd_cardinfo.card_cid.pnm0 >> 24) & 0xFF;
        pnm[1] = (sd_cardinfo.card_cid.pnm0 >> 16) & 0xFF;
        pnm[2] = (sd_cardinfo.card_cid.pnm0 >> 8) & 0xFF;
        pnm[3] = sd_cardinfo.card_cid.pnm0 & 0xFF;
        pnm[4] = sd_cardinfo.card_cid.pnm1 & 0xFF;
        pnm[5] = '\0';
        printf("Product Name: %s\r\n", pnm);
        
        // 打印产品版本和序列号
        printf("Product Revision: %d.%d\r\n", (sd_cardinfo.card_cid.prv >> 4) & 0x0F, sd_cardinfo.card_cid.prv & 0x0F);
        // 序列号以无符号方式显示，避免负数
        printf("Product Serial Number: 0x%08X\r\n", sd_cardinfo.card_cid.psn);
        
        // 打印CSD版本和其它CSD信息
        printf("CSD Version: %d.0\r\n", sd_cardinfo.card_csd.csd_struct + 1);
        
    }
    else
    {
        printf("\r\nFailed to get SD card information, error code: %d\r\n", status);
    }
}

void sd_fatfs_test(void)
{
    uint16_t k = 5;
    DSTATUS stat = 0;
    do
    {
        stat = disk_initialize(0); 			//初始化SD卡（设备号0）,物理驱动器编号,每个物理驱动器（如硬盘、U 盘等）通常都被分配一个唯一的编号。
    }while((stat != 0) && (--k));			//如果初始化失败，重试最多k次。
    
    card_info_get();
    
    printf("SD Card disk_initialize:%d\r\n",stat);
    f_mount(0, &fs);						 //挂载SD卡的文件系统（设备号0）。
    printf("SD Card f_mount:%d\r\n",stat);
    
    if(RES_OK == stat)						 //返回挂载结果（FR_OK 表示成功）。
    {        
        printf("\r\nSD Card Initialize Success!\r\n");
     
        result = f_open(&fdst, "0:/TEST.TXT", FA_CREATE_ALWAYS | FA_WRITE);		//在SD卡上创建文件FATFS.TXT。
     
        sprintf((char *)filebuffer, "Long live the ultimate invincible giant, the adorable kiana!");

        //result = f_write(&fdst, textfilebuffer, sizeof(textfilebuffer), &bw); 	//将textfilebuffer中的数据写入文件。
        result = f_write(&fdst, filebuffer, sizeof(filebuffer), &bw);				//将filebuffer中的数据写入文件。
        
        /**********检查写入结果 begin****************/
        if(FR_OK == result)		
            printf("TEST FILE write Success!\r\n");
        else
        {
            printf("TEST FILE write failed!\r\n");
        }
        /**********检查写入结果 end****************/
        
        f_close(&fdst);//关闭文件
        
        
        f_open(&fdst, "0:/TEST.TXT", FA_OPEN_EXISTING | FA_READ);	//以只读方式重新打开文件
        br = 1;
        
        /**********循环读取文件内容 begin****************/
        for(;;)
        {
            // 清空缓冲区
            for (count=0; count<128; count++)
            {
                buffer[count]=0;
            }
            // 读取文件内容到buffer
            result = f_read(&fdst, buffer, sizeof(buffer), &br);
            if ((0 == result)|| (0 == br))
            {
                break;
            }
        }
        /**********循环读取文件内容 end****************/
        
        // 比较读取的内容与写入的内容是否一致
        if(SUCCESS == memory_compare(buffer, filebuffer, 128))
        {
            printf("TEST Read File Success!\r\nThe content is:%s\r\n",buffer);
        }
        else
        {
            printf("TEST FILE read failed!\n");            
        }
         f_close(&fdst);//关闭文件
    }
}

void write_log_id_to_flash()
{
    // 擦除扇区
    spi_flash_sector_erase(0x2000);
    spi_flash_wait_for_write_end();
    
    // 写入日志ID
    spi_flash_buffer_write((uint8_t*)&sp.log_id, 0x2000,4);
}

void read_log_id_from_flash()
{
    spi_flash_buffer_read((uint8_t*)&sp.log_id,0x2000, 4);
}

//日志写入函数
void log_write(const char* write_data)
{    
    char log_entry[256];
    if(!sp.log_file_val) return;
    // 格式：2025-01-01 10:00:01 system init
    sprintf(log_entry,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x %s\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,write_data);    
    
    UINT bytes_written;
    FRESULT result = f_write(&sp.log_file, log_entry, strlen(log_entry), &bytes_written);
    if(result == FR_OK && bytes_written == strlen(log_entry)) {
        f_sync(&sp.log_file);
    } else {
//        my_printf(DEBUG_USART, "Log write error: %d, bytes: %d/%d\r\n", 
//                 result, bytes_written, strlen(log_entry));
    }
}


//sample执行函数
void sample_proc(char *str)
{
	FRESULT result;
	static char sample_filename[64];
	if(sp.sample_log_num == 0){
		sprintf(sample_filename, "sample/sampleData20%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x.txt",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second);
		result = f_open(&sp.sample_file, sample_filename, FA_CREATE_ALWAYS | FA_WRITE);
		f_close(&sp.sample_file);
		result = f_open(&sp.sample_file, sample_filename, FA_CREATE_ALWAYS | FA_WRITE);
	}
	else if(sp.sample_log_num == 10)
	{
		f_close(&sp.sample_file);
		sp.sample_log_num = 0;
		return;
	}
	sp.sample_log_num++;
	UINT bytes_written;
    result = f_write(&sp.sample_file, str, strlen(str), &bytes_written);
    if(result == FR_OK && bytes_written == strlen(str)) {
        f_sync(&sp.sample_file);
    }
}

//overlimit执行函数
void overlimit_proc(char *str)
{
	FRESULT result;
	static char overlimit_filename[64];
	if(sp.overlimit_log_num == 0){
		sprintf(overlimit_filename, "overLimit/overLimit20%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x.txt",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second);
		result = f_open(&sp.overlimit_file, overlimit_filename, FA_CREATE_ALWAYS | FA_WRITE);
		f_close(&sp.overlimit_file);
		result = f_open(&sp.overlimit_file, overlimit_filename, FA_CREATE_ALWAYS | FA_WRITE);
	}
	else if(sp.overlimit_log_num == 10)
	{
		f_close(&sp.overlimit_file);
		sp.overlimit_log_num = 0;
		return;
	}
	sp.overlimit_log_num++;
	UINT bytes_written;
    result = f_write(&sp.overlimit_file, str, strlen(str), &bytes_written);
    if(result == FR_OK && bytes_written == strlen(str)) {
        f_sync(&sp.overlimit_file);
    }
}

//hidedata_proc执行函数
void hidedata_proc(char *str)
{
	FRESULT result;
	static char hidedata_filename[64];
	if(sp.hidedata_log_num == 0){
		sprintf(hidedata_filename, "hideData/hideData20%0.2x%0.2x%0.2x%0.2x%0.2x%0.2x.txt",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second);
		result = f_open(&sp.hidedata_file, hidedata_filename, FA_CREATE_ALWAYS | FA_WRITE);
		f_close(&sp.hidedata_file);
		result = f_open(&sp.hidedata_file, hidedata_filename, FA_CREATE_ALWAYS | FA_WRITE);
	}
	else if(sp.hidedata_log_num == 10)
	{
		f_close(&sp.hidedata_file);
		sp.hidedata_log_num = 0;
		return;
	}
	sp.hidedata_log_num++;
	UINT bytes_written;
    result = f_write(&sp.hidedata_file, str, strlen(str), &bytes_written);
    if(result == FR_OK && bytes_written == strlen(str)) {
        f_sync(&sp.hidedata_file);
    }
}

/*SD卡初始化*/
void sdcard_init()
{
	sd_fatfs_init();
	FRESULT result;
    
    // 确保SD卡已经初始化
    DSTATUS sd_status = disk_initialize(0);
    if(sd_status != RES_OK) {
        return;
    }
    
    // 挂载文件系统并保持挂载状态
    result = f_mount(0, &fs);
    
    if(result != FR_OK) {
        printf("Failed to mount TF card for storage init: %d\r\n", result);
        return;
    }
    
    //创建存储数据的文件夹
    f_mkdir("sample");
    f_mkdir("overLimit");
    f_mkdir("log");
    f_mkdir("hideData");

    char log_filename[64];
    
    read_log_id_from_flash();
    sprintf(log_filename, "log/log%u.txt",sp.log_id);
    result = f_open(&sp.log_file, log_filename, FA_CREATE_ALWAYS | FA_WRITE);
    if(result == FR_OK) {
        sp.log_file_val = 1;
    } else {
       sp.log_file_val = 0;
    }
    f_close(&sp.log_file);
    sp.log_id++;
    write_log_id_to_flash();
    sp.log_id--;
    result = f_open(&sp.log_file, log_filename, FA_CREATE_ALWAYS | FA_WRITE);
    if(result == FR_OK) {
        sp.log_file_val = 1;
      printf("OK");
    } else {
       sp.log_file_val = 0;
    }
}

