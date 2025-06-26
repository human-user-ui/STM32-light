#include "light_config.h"


extern int light;
extern int LED_VALUE;
/*灯光亮度的三个挡位*/
#define LIGHT_MAX 1000
#define LIGHT_MEDIUM 500
#define LIGHT_MIN 250

#define LED_PORT GPIOC
#define LED_ON GPIO_Pin_11
#define LED_ON_2 GPIO_Pin_10

#define PEOPEL GPIO_Pin_0
#define PEOPEL_PORT GPIOD

/*亮度对应电压 3.3*(4.7/(4.7+R))*/
#define LIGHT_OFF  2.7211


extern uint16_t ADC_ConvertedValue_Light;

void light_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	/*灯光控制引脚 1为白光 3为暖光*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = LED_ON | LED_ON_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_PORT,&GPIO_InitStruct);
	
	/*人体检测接口*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = PEOPEL;
	GPIO_Init(PEOPEL_PORT,&GPIO_InitStruct);	
	
	/*机械按键*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(LED_PORT,&GPIO_InitStruct);
	
}

#include "bsp_OLED.h"
/*开启初始模式*/
void light_config(void)
{		
		light = LIGHT_MEDIUM;
		//light_GPIO_Init();
	//ADC_ConvertedValue_Light < LIGHT_OFF && GPIO_ReadInputDataBit(PEOPEL_PORT,PEOPEL) == 0
		
		LED_VALUE = 1;
		/*打开白光*/
		//GPIO_SetBits(LED_PORT,LED_ON);
		GPIO_SetBits(LED_PORT,LED_ON_2);
		/*将光照跳到最亮*/
		PWM_Config(light);
	
}

/*照明模式*/
void light_config_zhaoming(void)
{
		light = LIGHT_MAX;
		LED_VALUE = 6;
		GPIO_SetBits(LED_PORT,LED_ON);
		GPIO_SetBits(LED_PORT,LED_ON_2);	
	/*将光照调到最亮*/
		PWM_Config(light);
	
}


/*护眼模式*/
void light_config_huyan(void)
{
	light = LIGHT_MIN;
	LED_VALUE = 1;
	GPIO_ResetBits(LED_PORT,LED_ON_2);
	GPIO_SetBits(LED_PORT,LED_ON);
	PWM_Config(light);
}


/*关灯*/
void light_off(void)
{
	LED_VALUE = 0;
	light = 0;
	
	GPIO_ResetBits(LED_PORT,LED_ON_2);
	//GPIO_ResetBits(LED_PORT,LED_ON);
	PWM_Config(light);	
}

/*调高亮度*/
void light_up(void)
{
	light += 100;
	if(light >= 1000)
	{
		light = 1000;
	}
	PWM_Config(light);
}

void light_down(void)
{
	light -= 100;
	if(light <= 250)
	{
		light = 250;
	}
	PWM_Config(light);
}

void zidong_config(void)
{

}
