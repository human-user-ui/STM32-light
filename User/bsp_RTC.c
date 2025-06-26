#include "bsp_RTC.h"
#include "bsp_debug.h"
/**
*@brief 判断是否为闰年
*@param @year年份
*@retval 返回ture or false
*/
int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/**
*@brief 获取某个月的天数
*@param @year 年份 @month 月份
*@retval 返回天数
*/

int days_in_month(int year, int month) {
    
	if (month == 2) {
		return is_leap_year(year) ? 29 : 28;
	}
    
	if (month == 4 || month == 6 || month == 9 || month == 11) {
		return 30;
	}
   
	return 31;
}

/**
*@brief 计算从1970年1月1日到给定日期的总天数
*@param @*dt struct rtc_time结构体 储存 年月日 时分秒
*@retval 返回具体天数
*/

int days_since_epoch(struct rtc_time *dt) {
    int days = 0;

	int y,m;
    // 计算1970年到当前年的总天数
    for (y = 1970; y < dt->tm_year; y++) {
        days += is_leap_year(y) ? 366 : 365;
    }

    // 计算当前年1月到当前月的总天数
    for (m = 1; m < dt->tm_mon; m++) {
        days += days_in_month(dt->tm_year, m);
    }

    // 加上当前月的天数
    days += dt->tm_mday - 1; // 减去1，因为从1日开始计算

    return days;
}

/**
*@brief 计算1970 年1月1日0时0分0秒到目标时间间隔多少秒 北京时间与格林威治时间间隔8H
*@param tm rtc_time结构体
*@retval 返回当前时间戳
*/
static u32 mktimev(struct rtc_time *tm)
{
	
	    // 计算从1970年1月1日到当前日期的总天数
    int days = days_since_epoch(tm);

    // 计算总秒数
    long seconds = days * 86400L; // 每天86400秒
    seconds += tm->tm_hour * 3600L;   // 加上小时的秒数
    seconds += tm->tm_min * 60L;   // 加上分钟的秒数
    seconds += tm->tm_sec;         // 加上秒数

    return seconds;

}


/**
*@brief 将时间戳转换为北京时间
*@param tim 时间戳 
*@param tm rtc_time结构体，来暂存时间
*@retval 
*/
static void to_tm(u32 timestamp, struct rtc_time * dt)
{
	  
		timestamp -= 8 * 3600;
    // 计算秒、分钟、小时
    dt->tm_sec = timestamp % 60;
    timestamp /= 60;
    dt->tm_min = timestamp % 60;
    timestamp /= 60;
    dt->tm_hour = timestamp % 24;
    timestamp /= 24;

    // 计算年份
    dt->tm_year = 1970;
    while (1) {
        int days_in_year = is_leap_year(dt->tm_year) ? 366 : 365;
        if (timestamp < days_in_year) {
            break;
        }
        timestamp -= days_in_year;
        dt->tm_year++;
    }

    // 计算月份和日期
    dt->tm_mon = 1;
    while (1) {
        int days_in_current_month = days_in_month(dt->tm_year, dt->tm_mon);
        if (timestamp < days_in_current_month) {
            break;
        }
        timestamp -= days_in_current_month;
        dt->tm_mon++;
    }
    dt->tm_mday = timestamp + 1; // 日期从1开始

}

static void RTC_Configuration(void)
{
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE);
	//Usart_SendString(USART1, "RTC_Configuration  START\n");
	/*使能RTC和后备寄存器访问*/
	PWR_BackupAccessCmd(ENABLE);
	/*将外设BKP的全部寄存器设置为缺省值*/
	BKP_DeInit();
	
	/*设置外部低速时钟*/
	RCC_LSEConfig(RCC_LSE_ON);
	
	/*等待RCC时钟标志位设置成功*/
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	
	}
	/*设置RTC时钟为外部低速时钟*/
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	/*使能RCT时钟*/
	RCC_RTCCLKCmd(ENABLE);
	/*等待RTC寄存器与RTC的APB时钟同步*/
	
	RTC_WaitForSynchro();

	RTC_WaitForLastTask();
	/*使能指定的RTC中断*/
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
	/*等待最近一次对RTC寄存器的写操作完成*/
	RTC_WaitForLastTask();
	/*设置RTC预分频的值 */
	RTC_SetPrescaler(32767);
	/*等待最近一次对RTC寄存器的写操作完成*/
	RTC_WaitForLastTask();
	//Usart_SendString(USART1, "RTC_Configuration \n");
}

void NVIC_Configuration(void) {
	
    NVIC_InitTypeDef NVIC_InitStruct;
    // 配置RTC全局中断
    NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


static void Time_Adjust(struct rtc_time *tm)
{
	//Usart_SendString(USART1, "Time_Adjust == START\n");
	/*RTC配置*/
	RTC_Configuration();
	
	/*等待最后一次RTC的写操作完成*/
	RTC_WaitForLastTask();
	/*设置RTC计数器的值*/
	RTC_SetCounter(mktimev(tm)+TIME_ZOOM);
	/*等待最后一次RTC的写操作完成*/
	RTC_WaitForLastTask();
	//Usart_SendString(USART1, "Time_Adjust \n");
}

void RTC_CheckAndConfig(struct rtc_time *tm)
{
	
	NVIC_Configuration();
	if(BKP_ReadBackupRegister(RTC_BKP_DRX) != RTC_BKP_DATA)
	{
		//Usart_SendString(USART1, "RTC_BKP_DATA == START\n");
		Time_Adjust(tm);
		 //Usart_SendString(USART1, "RTC_BKP_DATA == END\n");
		
		BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);
	}else{
		//Usart_SendString(USART1, "RTC_BKP_DATA != START\n");
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR| RCC_APB1Periph_BKP, ENABLE);
		
		/* 允许访问 Backup 区域 */
		PWR_BackupAccessCmd(ENABLE);
		
		//RTC_EnterConfigMode();
		 /* 检查是否掉电重启 */
			 if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {

			 }
			 /* 检查是否 Reset 复位 */
			 else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {

			 }

			 /* 等待寄存器同步 */
			 RTC_WaitForSynchro();
			 /* 允许 RTC 秒中断 */
			 RTC_ITConfig(RTC_IT_SEC|RTC_IT_ALR, ENABLE);
			 /* 等待上次 RTC 寄存器写操作完成 */
				RTC_WaitForLastTask();
			// Usart_SendString(USART1, "RTC_BKP_DATA != END\n");
		}
		 /*清除复位标志flags*/
		 RCC_ClearFlag();
		//Usart_SendString(USART1, "RTC_CheckAndConfig\n");
}

void delay( int n)
{
	int i,j;
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < 123; j++)
		;
	}
}
#define TIME_D 50
#define TIME_M -8
#include "bsp_debug.h"
void Time_Display(void)
{
	u32 tim;
	struct rtc_time ti = {0};
	tim = RTC_GetCounter();
	to_tm(tim, &ti);
	//Usart_SendString(USART1, "ABCD");
	//时
	//if(ti.tm_hour < 10)
	//{
	//	oled_Write_Num(0,0+TIME_D,0,1);
		//delay(1);
	//	oled_Write_Num(0,7+TIME_D,ti.tm_hour,1);
	//}else{
		
		oled_Write_Num(0,-6+TIME_D,ti.tm_hour,2);
	//}

	oled_Write_String(0,8+TIME_D,":");
	delay(1);
	//分
	if(ti.tm_min < 10)
	{
		oled_Write_Num(0,14+TIME_D,0,1);
		oled_Write_Num(0,20+TIME_D,ti.tm_min,1);
	}else{
		
		oled_Write_Num(0,14+TIME_D,ti.tm_min,2);
		
	}
	
	oled_Write_String(0,27+TIME_D,":");
	//秒
	if(ti.tm_sec <10)
	{
		oled_Write_Num(0,34+TIME_D,0,1);
	//	delay(1);
		oled_Write_Num(0,41+TIME_D,ti.tm_sec,1);
	}else{
		oled_Write_Num(0,34+TIME_D,ti.tm_sec,2);
	}
	
	//年
	oled_Write_Num(1,40+TIME_M,ti.tm_year,4);
	
	delay(1);
	oled_Write_String(1,65+TIME_M,"/");
	//月
	if(ti.tm_mon < 10)
	{
		oled_Write_Num(1,72+TIME_M,0,1);
		//delay(1);
		oled_Write_Num(1,79+TIME_M,ti.tm_mon,1);
	}else{
		oled_Write_Num(1,72+TIME_M,ti.tm_mon,2);
	}
	//delay(1);
	oled_Write_String(1,85+TIME_M,"/");
	//天
	if(ti.tm_mday < 10)
	{
		oled_Write_Num(1,92+TIME_M,0,1);
		//delay(1);
		oled_Write_Num(1,99+TIME_M,ti.tm_mday,1);
	}else{
		oled_Write_Num(1,92+TIME_M,ti.tm_mday,2);
	}

	
}

