#include "bsp_RTC.h"
#include "bsp_debug.h"
/**
*@brief �ж��Ƿ�Ϊ����
*@param @year���
*@retval ����ture or false
*/
int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/**
*@brief ��ȡĳ���µ�����
*@param @year ��� @month �·�
*@retval ��������
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
*@brief �����1970��1��1�յ��������ڵ�������
*@param @*dt struct rtc_time�ṹ�� ���� ������ ʱ����
*@retval ���ؾ�������
*/

int days_since_epoch(struct rtc_time *dt) {
    int days = 0;

	int y,m;
    // ����1970�굽��ǰ���������
    for (y = 1970; y < dt->tm_year; y++) {
        days += is_leap_year(y) ? 366 : 365;
    }

    // ���㵱ǰ��1�µ���ǰ�µ�������
    for (m = 1; m < dt->tm_mon; m++) {
        days += days_in_month(dt->tm_year, m);
    }

    // ���ϵ�ǰ�µ�����
    days += dt->tm_mday - 1; // ��ȥ1����Ϊ��1�տ�ʼ����

    return days;
}

/**
*@brief ����1970 ��1��1��0ʱ0��0�뵽Ŀ��ʱ���������� ����ʱ�����������ʱ����8H
*@param tm rtc_time�ṹ��
*@retval ���ص�ǰʱ���
*/
static u32 mktimev(struct rtc_time *tm)
{
	
	    // �����1970��1��1�յ���ǰ���ڵ�������
    int days = days_since_epoch(tm);

    // ����������
    long seconds = days * 86400L; // ÿ��86400��
    seconds += tm->tm_hour * 3600L;   // ����Сʱ������
    seconds += tm->tm_min * 60L;   // ���Ϸ��ӵ�����
    seconds += tm->tm_sec;         // ��������

    return seconds;

}


/**
*@brief ��ʱ���ת��Ϊ����ʱ��
*@param tim ʱ��� 
*@param tm rtc_time�ṹ�壬���ݴ�ʱ��
*@retval 
*/
static void to_tm(u32 timestamp, struct rtc_time * dt)
{
	  
		timestamp -= 8 * 3600;
    // �����롢���ӡ�Сʱ
    dt->tm_sec = timestamp % 60;
    timestamp /= 60;
    dt->tm_min = timestamp % 60;
    timestamp /= 60;
    dt->tm_hour = timestamp % 24;
    timestamp /= 24;

    // �������
    dt->tm_year = 1970;
    while (1) {
        int days_in_year = is_leap_year(dt->tm_year) ? 366 : 365;
        if (timestamp < days_in_year) {
            break;
        }
        timestamp -= days_in_year;
        dt->tm_year++;
    }

    // �����·ݺ�����
    dt->tm_mon = 1;
    while (1) {
        int days_in_current_month = days_in_month(dt->tm_year, dt->tm_mon);
        if (timestamp < days_in_current_month) {
            break;
        }
        timestamp -= days_in_current_month;
        dt->tm_mon++;
    }
    dt->tm_mday = timestamp + 1; // ���ڴ�1��ʼ

}

static void RTC_Configuration(void)
{
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR, ENABLE);
	//Usart_SendString(USART1, "RTC_Configuration  START\n");
	/*ʹ��RTC�ͺ󱸼Ĵ�������*/
	PWR_BackupAccessCmd(ENABLE);
	/*������BKP��ȫ���Ĵ�������Ϊȱʡֵ*/
	BKP_DeInit();
	
	/*�����ⲿ����ʱ��*/
	RCC_LSEConfig(RCC_LSE_ON);
	
	/*�ȴ�RCCʱ�ӱ�־λ���óɹ�*/
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
	{
	
	}
	/*����RTCʱ��Ϊ�ⲿ����ʱ��*/
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	/*ʹ��RCTʱ��*/
	RCC_RTCCLKCmd(ENABLE);
	/*�ȴ�RTC�Ĵ�����RTC��APBʱ��ͬ��*/
	
	RTC_WaitForSynchro();

	RTC_WaitForLastTask();
	/*ʹ��ָ����RTC�ж�*/
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	
	/*�ȴ����һ�ζ�RTC�Ĵ�����д�������*/
	RTC_WaitForLastTask();
	/*����RTCԤ��Ƶ��ֵ */
	RTC_SetPrescaler(32767);
	/*�ȴ����һ�ζ�RTC�Ĵ�����д�������*/
	RTC_WaitForLastTask();
	//Usart_SendString(USART1, "RTC_Configuration \n");
}

void NVIC_Configuration(void) {
	
    NVIC_InitTypeDef NVIC_InitStruct;
    // ����RTCȫ���ж�
    NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}


static void Time_Adjust(struct rtc_time *tm)
{
	//Usart_SendString(USART1, "Time_Adjust == START\n");
	/*RTC����*/
	RTC_Configuration();
	
	/*�ȴ����һ��RTC��д�������*/
	RTC_WaitForLastTask();
	/*����RTC��������ֵ*/
	RTC_SetCounter(mktimev(tm)+TIME_ZOOM);
	/*�ȴ����һ��RTC��д�������*/
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
		
		/* ������� Backup ���� */
		PWR_BackupAccessCmd(ENABLE);
		
		//RTC_EnterConfigMode();
		 /* ����Ƿ�������� */
			 if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET) {

			 }
			 /* ����Ƿ� Reset ��λ */
			 else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET) {

			 }

			 /* �ȴ��Ĵ���ͬ�� */
			 RTC_WaitForSynchro();
			 /* ���� RTC ���ж� */
			 RTC_ITConfig(RTC_IT_SEC|RTC_IT_ALR, ENABLE);
			 /* �ȴ��ϴ� RTC �Ĵ���д������� */
				RTC_WaitForLastTask();
			// Usart_SendString(USART1, "RTC_BKP_DATA != END\n");
		}
		 /*�����λ��־flags*/
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
	//ʱ
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
	//��
	if(ti.tm_min < 10)
	{
		oled_Write_Num(0,14+TIME_D,0,1);
		oled_Write_Num(0,20+TIME_D,ti.tm_min,1);
	}else{
		
		oled_Write_Num(0,14+TIME_D,ti.tm_min,2);
		
	}
	
	oled_Write_String(0,27+TIME_D,":");
	//��
	if(ti.tm_sec <10)
	{
		oled_Write_Num(0,34+TIME_D,0,1);
	//	delay(1);
		oled_Write_Num(0,41+TIME_D,ti.tm_sec,1);
	}else{
		oled_Write_Num(0,34+TIME_D,ti.tm_sec,2);
	}
	
	//��
	oled_Write_Num(1,40+TIME_M,ti.tm_year,4);
	
	delay(1);
	oled_Write_String(1,65+TIME_M,"/");
	//��
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
	//��
	if(ti.tm_mday < 10)
	{
		oled_Write_Num(1,92+TIME_M,0,1);
		//delay(1);
		oled_Write_Num(1,99+TIME_M,ti.tm_mday,1);
	}else{
		oled_Write_Num(1,92+TIME_M,ti.tm_mday,2);
	}

	
}

