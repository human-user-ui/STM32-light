#ifndef _BSP_RTC_H_
#define _BSP_RTC_H_

#include <stm32f10x.h>
#include <stm32f10x_rtc.h>
#include <stm32f10x_bkp.h>
#include "bsp_OLED.h"
#define RTC_CLOCK_SOURCE_LSE

#define RTC_BKP_DRX BKP_DR1
#define RTC_BKP_DATA 0xA5A5
#define TIME_ZOOM (8*60*60)

struct rtc_time{
	/*秒 分 时 天 月 年 周*/
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
};

void RTC_CheckAndConfig(struct rtc_time *tm);

void Time_Display(void);

#endif
