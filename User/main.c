#include <stm32f10x.h>
#include "bsp_OLED.h"
#include "bsp_AD_Light.h"
#include "bsp_PWM.h"
#include "light_config.h"
#include "bsp_debug.h"
#include "bsp_RTC.h"
#include "hs_ceju.h"

#define DEBUG 1
/*AD转换的值*/
volatile uint16_t ADC_ConvertedValue_Light = 0.00;
int light = 0;

int time = 0;

int zi;
	
static uint16_t ucTemp = '2';
int LED_VALUE  = 0;
int zuozi = 0;
int zuo = 0;
int zidong_light = 0;
int Value_Times = 0;
volatile uint32_t rise_time = 0;
volatile uint32_t fall_time = 0;
volatile uint32_t overflow_rise = 0;
volatile uint32_t overflow_fall = 0;
volatile uint32_t timer_overflow = 0;
uint32_t pulse_duration = 0;


#define LED_ON  '1'
#define LED_OFF '2'
#define LED_UP  '3'
#define LED_DOWN '4'
#define LED_HUYAN '6'
#define LED_ZHAOMING '5'
#define ZI_DOND '7'
#define TIME_D 50
#define TIME_M -8

void delay_us(uint32_t us){
 
    SysTick_Config(SystemCoreClock/1000000);    //固件库函数初始化+设置重装值
    while(us--){
        while(!((SysTick->CTRL)&(SysTick_CTRL_COUNTFLAG))); //不断查询计数标志位
    }
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;    //关闭SysTick
}
 
void delay_ms(uint32_t ms){
    while(ms--) delay_us(1000);        
}
 
void delay_s(uint32_t s){
    while(s--) delay_ms(1000);
}

int main(void)
{

	struct rtc_time tm;
//	int last_value = 4;
	int f = 0;
	PWM_EN();
	OLED_EN();
	ADC_EN();
	HS_Init();
	Usart1Init();
	light_GPIO_Init();

	tm.tm_year = 2025;
	tm.tm_mon = 5;
	tm.tm_mday = 25;
	tm.tm_hour = 17;
	tm.tm_min =  11;
	tm.tm_sec = 0;
	
	RTC_CheckAndConfig(&tm);
	oled_Write_String(2,0,"Status:");
	oled_Write_String(3,0,"Light:");
	//Usart_SendString(USART1, "ABC");
	//Usart_SendInt(USART1,4);
	while(1)
	{ 
		delay_s(1);
		//refresh();
		if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_0) == RESET)
		{
			light_off();
			oled_Write_String(2,45,"LED_OFF ");
			oled_Write_Num(3,40,light,3);
			oled_Write_String(3,59," ");
			zidong_light = 0;
			ucTemp = 9;
		}else{
			/*机械按键部分*/
			if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6) == RESET)
			{
				f++;
				if(f % 2 == 0)
					ucTemp = '2';
				else
					ucTemp = '7';
		}
#if DEBUG
			switch(ucTemp)
			{
				case LED_ON:
						light_config();
						oled_Write_String(2,45,"LED_ON  ");
						oled_Write_Num(3,40,light,3);
						oled_Write_String(3,59," ");
						zidong_light = 0;
				break;				
				case LED_OFF:
						light_off();
						oled_Write_String(2,45,"LED_OFF ");
						oled_Write_Num(3,40,light,3);
						oled_Write_String(3,59," ");
						zidong_light = 0;
				break;
				case LED_UP:
						light_up();
						oled_Write_Num(3,40,light,3);
						oled_Write_String(3,59," ");
				zidong_light = 0;
				ucTemp = 9;
				break;
				case LED_DOWN:
						light_down();
						oled_Write_Num(3,40,light,3);
						oled_Write_String(3,59," ");
						zidong_light = 0;
				ucTemp = 9;
				break;
				case LED_HUYAN:
						light_config_huyan();
						oled_Write_String(2,45,"HUYAN   ");
						oled_Write_Num(3,40,light,3);
						oled_Write_String(3,59," ");
						zidong_light = 0;
				break;
				
				case LED_ZHAOMING:
						light_config_zhaoming();	
						oled_Write_String(2,45,"ZHAOMING");
						oled_Write_Num(3,40,light,4);
						zidong_light = 0;
				break;
					
				case ZI_DOND:
						zidong_light = 1;
						zi = 800 - (0.2 * ADC_ConvertedValue_Light);	
						PWM_Config(zi);
						oled_Write_String(2,45,"ZIDONG  ");
						oled_Write_Num(3,40,zi,4);
						
				break;
			}
			
#endif
		zuozi = HS_read();
		//Usart_SendInt(USART1,zuozi);			
		if(zuozi < 100 && zuozi > 0)
		{
		//	Usart_SendInt(USART1,zuozi);
			zuo += zuozi;
			Value_Times++;
		}
	  if(Value_Times >=5)
		{
			zuo = zuo/Value_Times;
				if(zuo >= 30 && zuo <=45)
				{
					
					//Usart_SendString(USART1,"坐姿正常");
				}else{
					//Usart_SendString(USART1,"坐姿不正常");
					//Usart_SendInt(USART1,zuo);
					Usart_SendString(USART1,"on");
				}
				Value_Times = 0;
				zuo = 0;
		}
		//1小时提醒一次
		if(time > 3600)
		{
			Usart_SendString(USART1,"time");
			time = 0;
		}
	
		}

		//ucTemp = 9;
	}

}


/**
*@brief  ADC电压采集中断函数
*@param  void
*@retval void
*/
void ADC1_2_IRQHandler(void)
{
	if(ADC_GetITStatus(ADC1,ADC_IT_EOC) == SET)
	{
		ADC_ConvertedValue_Light= ADC_GetConversionValue(ADC1);
	}
		
	ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
}


/**
*@brief  RTC实时时钟中断处理函数
*@param	 void
*@retval void
*/
void RTC_IRQHandler(void)
{
	//ADC_ConvertedValue_Light= ADC_GetConversionValue(ADC1);
	if(RTC_GetFlagStatus(RTC_IT_SEC) != RESET)
	{
		RTC_ClearITPendingBit(RTC_IT_SEC);
		
		Time_Display();
		time++;

		RTC_WaitForLastTask();
		refresh();
	}
}

/**
*@brief USART1中断处理函数
*@param NULL
*@retval void
*/
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET){
		ucTemp = USART_ReceiveData(USART1);
		//USART_SendData(USART1,ucTemp);
  }
}


/**
*@brief 超声波测距时间计算
*@param NULL
*@retval void
*/
void TIM2_IRQHandler(void) {
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        timer_overflow++;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }

    if (TIM_GetITStatus(TIM2, TIM_IT_CC4) != RESET) {
        if (TIM2->CCER & TIM_CCER_CC4P) {  // 当前是下降沿触发
            fall_time = TIM_GetCapture4(TIM2);
            overflow_fall = timer_overflow;
            
            // 计算脉冲持续时间（微秒）
            pulse_duration = (fall_time - rise_time) + 
                           (overflow_fall - overflow_rise) * 65536;
            
            // 切换回上升沿检测
            TIM2->CCER &= ~TIM_CCER_CC4P;
        } else {  // 当前是上升沿触发
            rise_time = TIM_GetCapture4(TIM2);
            overflow_rise = timer_overflow;
            
            // 切换为下降沿检测
            TIM2->CCER |= TIM_CCER_CC4P;
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
    }
}
