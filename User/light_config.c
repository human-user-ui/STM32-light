#include "light_config.h"


extern int light;
extern int LED_VALUE;
/*�ƹ����ȵ�������λ*/
#define LIGHT_MAX 1000
#define LIGHT_MEDIUM 500
#define LIGHT_MIN 250

#define LED_PORT GPIOC
#define LED_ON GPIO_Pin_11
#define LED_ON_2 GPIO_Pin_10

#define PEOPEL GPIO_Pin_0
#define PEOPEL_PORT GPIOD

/*���ȶ�Ӧ��ѹ 3.3*(4.7/(4.7+R))*/
#define LIGHT_OFF  2.7211


extern uint16_t ADC_ConvertedValue_Light;

void light_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	/*�ƹ�������� 1Ϊ�׹� 3Ϊů��*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = LED_ON | LED_ON_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_PORT,&GPIO_InitStruct);
	
	/*������ӿ�*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = PEOPEL;
	GPIO_Init(PEOPEL_PORT,&GPIO_InitStruct);	
	
	/*��е����*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(LED_PORT,&GPIO_InitStruct);
	
}

#include "bsp_OLED.h"
/*������ʼģʽ*/
void light_config(void)
{		
		light = LIGHT_MEDIUM;
		//light_GPIO_Init();
	//ADC_ConvertedValue_Light < LIGHT_OFF && GPIO_ReadInputDataBit(PEOPEL_PORT,PEOPEL) == 0
		
		LED_VALUE = 1;
		/*�򿪰׹�*/
		//GPIO_SetBits(LED_PORT,LED_ON);
		GPIO_SetBits(LED_PORT,LED_ON_2);
		/*��������������*/
		PWM_Config(light);
	
}

/*����ģʽ*/
void light_config_zhaoming(void)
{
		light = LIGHT_MAX;
		LED_VALUE = 6;
		GPIO_SetBits(LED_PORT,LED_ON);
		GPIO_SetBits(LED_PORT,LED_ON_2);	
	/*�����յ�������*/
		PWM_Config(light);
	
}


/*����ģʽ*/
void light_config_huyan(void)
{
	light = LIGHT_MIN;
	LED_VALUE = 1;
	GPIO_ResetBits(LED_PORT,LED_ON_2);
	GPIO_SetBits(LED_PORT,LED_ON);
	PWM_Config(light);
}


/*�ص�*/
void light_off(void)
{
	LED_VALUE = 0;
	light = 0;
	
	GPIO_ResetBits(LED_PORT,LED_ON_2);
	//GPIO_ResetBits(LED_PORT,LED_ON);
	PWM_Config(light);	
}

/*��������*/
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
