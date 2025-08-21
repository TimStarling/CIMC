#include "str_app.h"
#include "rtc_app.h"
#include "ff.h"
extern system_parameter sp;
extern uint32_t RTCSRC_FLAG;
char text_old[256] = "[Ratio]\nCh0 = 33.0\n\n[Limit]\nCh0 = 30.0"; //ԭʼ���ı���Ϣ
char text_new[256] = "";//���ĺ���ı���Ϣ
char text_tail[256] = "\n\n[Limit]\nCh0 = ";//�����ı���Ϣ�����ڴ洢����λ��֮����ı���Ϣ
char text_hand[256] = "[Ratio]\nCh0 = ";//�����ı���Ϣ�����ڴ洢����λ��֮ǰ���ı���Ϣ


/*
    ��������get_ch0
    ���ܣ���ȡCh0ֵ
    ������
        char *text���ı���Ϣ
        int num��Ch0����� 0-1 ��Ӧ [Ratio] �� [Limit]
        char *str��ch0�������ŵ��ַ���
    ����ֵ����
*/
void get_ch0(char *text, int num, char *str) {
    char *pos = strstr(text, "[Ratio]"); // ���� [Ratio] λ��
    if (num == 0) { // �ҵ� [Ratio] λ��
        pos = strstr(pos + 7, "Ch0 = "); // �ҵ� Ch0 λ��
        sscanf(pos + 6, "%s", str); // ��ȡ Ch0 ֵ
    } else { // �ҵ� [Limit] λ��
        pos = strstr(text, "[Limit]"); // �ҵ� [Limit] λ��
        pos = strstr(pos + 7, "Ch0 = "); // �ҵ� Ch0 λ��
        sscanf(pos + 6, "%s", str); // ��ȡ Ch0 ֵ
    }
}

/*
    ��������insert
    ���ܣ�ʵ�ֶ��ı���Ϣ���޸Ĳ���
    ������
        char *text_old��ԭʼ���ı���Ϣ
        char *text_new���µ��ı���Ϣ
        char *text_hand: �����ı���Ϣ�����ڴ洢����λ��֮ǰ���ı���Ϣ
        char *text_tail���������������ڴ洢����λ��֮����ı���Ϣ
        char *str0����һ�� Ch0 ��ֵ
        char *str1���ڶ��� Ch0 ��ֵ
    ����ֵ����
*/
void insert(char *text_old, char *text_new, char *text_hand, char *text_tail, char *str0, char *str1) {
    // ƴ���µ��ı���Ϣ
    sprintf(text_new, "%s%s%s%s", text_hand, str0, text_tail, str1);
}



void rtc_reset(int year,int month,int day,int hour,int minite,int secend)
{
	rcu_periph_clock_enable(RCU_PMU);
	pmu_backup_write_enable();
	rtc_pre_config();
	RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);
    rtc_setup_self(year,month,day,hour,minite,secend);
	rcu_all_reset_flag_clear();
			
    printf("RTC Config success\r\n");
    printf("Time:20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second);
}

void system_test()
{  
  printf("======system selftest======\r\n");
  DSTATUS sd_status = disk_initialize(0);
  uint32_t flash_id = spi_flash_read_id();
  printf("flash............ok\r\n");
  if(sd_status == 0) {  
      printf("TF card......ok\r\n");
      printf("flash ID:0x%06X\r\n", flash_id);
      uint32_t capacity = sd_card_capacity_get();
      printf("TF card memory: %d KB\r\n", capacity);
  } else {
      printf("TF card.......error\r\n");
      printf("flash ID:0x%06X\r\n", flash_id);
      printf("can not find TF card\r\n");
  }
  printf("Time:20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second);
  log_write("system selftest");
}

void show_conf()
{
  FIL config_file;
  FRESULT result;
  char line_buffer[128];
   uint8_t ratio__found = 0;
    uint8_t limit__found = 0;
    uint8_t ratio_ok = 0;
    uint8_t limit_ok = 0;
  

     result = f_open(&config_file, "config.ini", FA_READ);
    if(result != FR_OK) {
        printf("config.ini file not found.\r\n");
        return;
    }
    
    while(f_gets(line_buffer, sizeof(line_buffer), &config_file) != NULL) {
        // �Ƴ���ĩ�Ļ��з�
//      printf("%s",line_buffer);
        char* newline = strchr(line_buffer, '\n');
        if(newline) *newline = '\0';
        char* carriage = strchr(line_buffer, '\r');
        if(carriage) *carriage = '\0';
        
        // �������к�ע��
        if(strlen(line_buffer) == 0 || line_buffer[0] == ';' || line_buffer[0] == '#') {
            continue;
        }
        
        // ����Ƿ��ǽڱ���
        if(line_buffer[0] == '[') {
            if(strstr(line_buffer, "[Ratio]") != NULL) {
                ratio__found = 1;
                limit__found = 0;
            } else if(strstr(line_buffer, "[Limit]") != NULL) {
                limit__found = 1;
                ratio__found = 0;
            } else {
                ratio__found = 0;
                limit__found = 0;
            }
        }
        // ����������
        else if(ratio__found && strstr(line_buffer, "Ch0") != NULL) {
            char* equal_sign = strchr(line_buffer, '=');
            if(equal_sign != NULL) {
                equal_sign++; // �����Ⱥ�
                while(*equal_sign == ' ') equal_sign++; // �����ո�
                sp.ratio_data = atof(equal_sign);
                ratio_ok = 1;
            }
        }
        else if(limit__found && strstr(line_buffer, "Ch0") != NULL) {
            char* equal_sign = strchr(line_buffer, '=');
            if(equal_sign != NULL) {
                equal_sign++; // �����Ⱥ�
                while(*equal_sign == ' ') equal_sign++; // �����ո�
                sp.limit_data = atof(equal_sign);
                limit_ok = 1;
            }
        }
    }
    

    f_close(&config_file);
    
    
    if(ratio_ok && limit_ok)
    {
       printf("ratio: %.2f\r\n", sp.ratio_data);
       printf("limit: %.2f\r\n", sp.limit_data);
    }
}

void read_Ratio()
{
	printf("Ratio = %0.2f\r\n",sp.ratio_data);
	printf("Input value(1~100)\r\n");
}

void set_Ratio(char *Ratio_data)
{
	double temp = atof(Ratio_data);
	if(temp >= 0.9f && temp <= 100.1f){
    printf("ratio modified success\r\n");
    sp.ratio_data =  temp;
		printf("Ratio = %0.1f\r\n",sp.ratio_data);
	}else{
		printf("ratio invaild\r\n");
		printf("Ratio = %0.2f\r\n",sp.ratio_data);
	}
	
}

void read_Limit()
{
	printf("Limit = %0.1f\r\n",sp.limit_data);
	printf("Input value(0~500)\r\n");
}

void set_Limit(char *Limit_data)
{
	double temp = atof(Limit_data);
	if(temp >= 0 && temp <= 500.1f){
		printf("limit modified success\r\n");
		sp.limit_data = temp;
		printf("Limit = %0.1f\r\n",sp.limit_data);
	}
	else{
		printf("limit invaild\r\n");
		printf("Limit = %0.1f\r\n",sp.limit_data);
	}
}

void config_save()
{
  flash_data temp_ratio,temp_limit;
	printf("ratio = %.2f\r\n",sp.ratio_data);
	printf("limit = %.2f\r\n",sp.limit_data);
  temp_ratio.f = (float)sp.ratio_data;
  temp_limit.f = (float)sp.limit_data;
	printf("save parameters to flash\r\n");
	spi_flash_sector_erase(0);//����10����ַ������
	spi_flash_buffer_write("Device_ID:2025-CIMC-2025244579",0,30);//��flash_buff������д��flash
  spi_flash_wait_for_write_end();
	spi_flash_buffer_write((uint8_t *)&temp_ratio,30,4);//��str0������д��flash
  spi_flash_wait_for_write_end();
	spi_flash_buffer_write((uint8_t *)&temp_limit,34,4);//��str1������д��flash
  spi_flash_wait_for_write_end();
}

void config_read(){
  flash_data temp_ratio,temp_limit;
	printf("save parameters from flash\r\n");
	spi_flash_buffer_read((uint8_t *)&temp_ratio,30,4);
	spi_flash_buffer_read((uint8_t *)&temp_limit,34,4);
  sp.ratio_data = temp_ratio.f;
  sp.limit_data = temp_limit.f;
	printf("ratio = %0.2f\r\n",sp.ratio_data);
	printf("limit = %0.2f\r\n",sp.limit_data );
}


//Сдת��д����
void convert_to_uppercase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        // ����Ƿ�ΪСд��ĸ��ת��Ϊ��д
        if (islower((unsigned char)str[i])) {
            str[i] = toupper((unsigned char)str[i]);
        }
    }
}


// ���ո��滻Ϊ '0'
void peach_to_0(char *str) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == ' ') {
            str[i] = '0'; 
        }
        i++;
    }
}


//��ʱ���ת��ΪUnixʱ���
void hide_to_unix()
{
    struct tm time_info;
    time_t unix_timestamp;
    char out[100];
    char buf[100];
    char zhengshu[50];//ADC��������
    char xiaoshu[50];//ADCС������
    char year[2];
    char month[2];
    char day[2];
    char hour[2];
    char minute[2];
    char second[2];
	char hide_print[100];
    
    int volt_temp = sp.adc_vlot * 10;
    sprintf(zhengshu, "%4x", (volt_temp - (volt_temp % 10)) / 10);
    peach_to_0(zhengshu);
    float temp =  (volt_temp % 10) / 10.0f;
    sprintf(xiaoshu, "%4x", (int)(temp * 65536) );
    peach_to_0(xiaoshu);
    sprintf(year,"%x",sp.year);
    sprintf(month,"%x",sp.month);
    sprintf(day,"%x",sp.day);
    sprintf(hour,"%x",sp.hour);
    sprintf(minute,"%x",sp.minute);
    sprintf(second,"%x",sp.second);
    // ����ʱ����Ϣ������ 2024 �� 9 �� 19 �� 15:30:45��
    time_info.tm_year = 2000 + atoi(year) - 1900; // tm_year �Ǵ� 1900 �꿪ʼ������
    time_info.tm_mon = atoi(month) - 1;        // tm_mon �Ǵ� 1 �¿�ʼ��������0-11��
    time_info.tm_mday = atoi(day);
    time_info.tm_hour = atoi(hour)-8;
    time_info.tm_min = atoi(minute);
    time_info.tm_sec = atoi(second);
    // ���ṹ��ʱ��ת��Ϊ Unix ʱ���
    // ���ṹ��ʱ��ת��Ϊ Unix ʱ���
    unix_timestamp = mktime(&time_info);

    //�ַ���ƴ��
    sprintf(buf, "%4x",unix_timestamp);
	sprintf(out, "%s%s%s",buf, zhengshu, xiaoshu);
	convert_to_uppercase(out);
	sprintf(hide_print,"20%0.2x-%0.2x-%0.2x %0.2x-%0.2x-%0.2x %0.1fV\r\nhide:%s\r\n",sp.year,sp.month,sp.day,sp.hour,sp.minute,sp.second,sp.adc_vlot,out);
    hidedata_proc(hide_print);
	if(sp.limit_data < sp.adc_vlot)sprintf(out, "%s*",out);
    printf("%s\r\n", out);
    
}