#include "rtc_app.h"
extern system_parameter sp;
#define RTC_CLOCK_SOURCE_LXTAL		// 配置RTC时钟源
#define BKP_VALUE    0x32F0

rtc_parameter_struct   rtc_initpara;
rtc_alarm_struct  rtc_alarm;
__IO uint32_t prescaler_a = 0, prescaler_s = 0;
uint32_t RTCSRC_FLAG = 0;



/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void RTC_Init(void)
{
    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enable the access of the RTC registers */
    pmu_backup_write_enable();
	
    rtc_pre_config();
    /* get RTC clock entry selection */
    RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);

    /* check if RTC has aready been configured */
    if((BKP_VALUE != RTC_BKP0) || (0x00 == RTCSRC_FLAG)){
        /* backup data register value is not correct or not yet programmed
        or RTC clock source is not configured (when the first time the program 
        is executed or data in RCU_BDCTL is lost due to Vbat feeding) */
        rtc_setup();
    }else{
        /* detect the reset source */
        if (RESET != rcu_flag_get(RCU_FLAG_PORRST)){
        }else if (RESET != rcu_flag_get(RCU_FLAG_EPRST)){
        }

        rtc_show_time();
    }
    rcu_all_reset_flag_clear();
}

uint32_t dec_to_hex(int num) {
    int high_nibble = num / 10;   // 获取十位上的数字作为高位半字节
    int low_nibble = num % 10;    // 获取个位上的数字作为低位半字节

    return (high_nibble << 4) | low_nibble;  // 组合成十六进制数
}

void rtc_setup_self(int year,int month,int day,int hour,int minite,int secend)
{
    /* setup RTC time value */
    uint32_t tmp_year = 0xFF, tmp_month = 0xFF, tmp_day = 0xFF;
    uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

    rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.year = 0x16;
    rtc_initpara.day_of_week = RTC_SATURDAY;
    rtc_initpara.month = RTC_APR;
    rtc_initpara.date = 0x30;
    rtc_initpara.display_format = RTC_24HOUR;
    rtc_initpara.am_pm = RTC_AM;

    /* RTC时钟配置，用10进制来配置16进制参数*/
    while(tmp_year == 0xFF) {
        tmp_year = dec_to_hex(year);//2025年
        rtc_initpara.year = tmp_year;
    }
    while(tmp_month == 0xFF) {
        tmp_month = dec_to_hex(month);//6月
        rtc_initpara.month = tmp_month;
    }
    while(tmp_day == 0xFF) {
        tmp_day = dec_to_hex(day);//1日
        rtc_initpara.date = tmp_day;
    }
    while (0xFF == tmp_hh){
        tmp_hh = dec_to_hex(hour);//11时
        rtc_initpara.hour = tmp_hh;
    }
    while (0xFF == tmp_mm){
        tmp_mm = dec_to_hex(minite);//05分
        rtc_initpara.minute = tmp_mm;
    }
    while (0xFF == tmp_ss){
        tmp_ss = dec_to_hex(secend);//秒
        rtc_initpara.second = tmp_ss;
    }

    /* RTC current time configuration */
    if(ERROR == rtc_init(&rtc_initpara)){
    }else{
        rtc_show_time();
        RTC_BKP0 = BKP_VALUE;
    }
}

void RTC_Init_self(int year,int month,int day,int hour,int minite,int secend)
{
    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enable the access of the RTC registers */
    pmu_backup_write_enable();
	
    rtc_pre_config();
    /* get RTC clock entry selection */
    RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);

    /* check if RTC has aready been configured */
    if((BKP_VALUE != RTC_BKP0) || (0x00 == RTCSRC_FLAG)){
        /* backup data register value is not correct or not yet programmed
        or RTC clock source is not configured (when the first time the program 
        is executed or data in RCU_BDCTL is lost due to Vbat feeding) */
        rtc_setup_self(year,month,day,hour,minite,secend);
    }else{
        /* detect the reset source */
        if (RESET != rcu_flag_get(RCU_FLAG_PORRST)){
        }else if (RESET != rcu_flag_get(RCU_FLAG_EPRST)){
        }

        rtc_show_time();
    }
    rcu_all_reset_flag_clear();
}

/*!
    \brief      RTC configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_pre_config(void)
{
    #if defined (RTC_CLOCK_SOURCE_IRC32K)
          rcu_osci_on(RCU_IRC32K);
          rcu_osci_stab_wait(RCU_IRC32K);
          rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);

          prescaler_s = 0x13F;
          prescaler_a = 0x63;
    #elif defined (RTC_CLOCK_SOURCE_LXTAL)
          rcu_osci_on(RCU_LXTAL);
          rcu_osci_stab_wait(RCU_LXTAL);
          rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

          prescaler_s = 0xFF;
          prescaler_a = 0x7F;
    #else
    #error RTC clock source should be defined.
    #endif /* RTC_CLOCK_SOURCE_IRC32K */

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}

/*!
    \brief      use hyperterminal to setup RTC time and alarm
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_setup(void)
{
    /* setup RTC time value */
		uint32_t tmp_year = 0xFF, tmp_month = 0xFF, tmp_day = 0xFF;
    uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

    rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn = prescaler_s;
    rtc_initpara.year = 0x16;
    rtc_initpara.day_of_week = RTC_SATURDAY;
    rtc_initpara.month = RTC_APR;
    rtc_initpara.date = 0x30;
    rtc_initpara.display_format = RTC_24HOUR;
    rtc_initpara.am_pm = RTC_AM;

    /* RTC时钟配置，用10进制来配置16进制参数*/
    while(tmp_year == 0xFF) {
        tmp_year = 0x25;//2025年
        rtc_initpara.year = tmp_year;
    }
    while(tmp_month == 0xFF) {
        tmp_month = 0x06;//6月
        rtc_initpara.month = tmp_month;
    }
    while(tmp_day == 0xFF) {
        tmp_day = 0x01;//1日
        rtc_initpara.date = tmp_day;
    }
    while (0xFF == tmp_hh){
        tmp_hh = 0x11;//11时
        rtc_initpara.hour = tmp_hh;
    }
    while (0xFF == tmp_mm){
        tmp_mm = 0x05;//05分
        rtc_initpara.minute = tmp_mm;
    }
    while (0xFF == tmp_ss){
        tmp_ss = 0x00;//秒
        rtc_initpara.second = tmp_ss;
    }

    /* RTC current time configuration */
    if(ERROR == rtc_init(&rtc_initpara)){
    }else{
        rtc_show_time();
        RTC_BKP0 = BKP_VALUE;
    }
}



/*
	时间读取函数
*/
void rtc_show_time(void)
{

    rtc_current_time_get(&rtc_initpara);

	sp.year = rtc_initpara.year;
	sp.month =  rtc_initpara.month;
	sp.day = rtc_initpara.date;
	sp.hour = rtc_initpara.hour;
	sp.minute = rtc_initpara.minute;
	sp.second = rtc_initpara.second;
}

/*!
    \brief      display the alram value
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_show_alarm(void)
{
    rtc_alarm_get(RTC_ALARM0,&rtc_alarm);
    printf("The alarm: %0.2x:%0.2x:%0.2x \n\r", rtc_alarm.alarm_hour, rtc_alarm.alarm_minute,\
           rtc_alarm.alarm_second);
}



