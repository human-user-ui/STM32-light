#include "bsp_PWM.h"
/**
*@brief ��ʼ��PWM����
*@param null
*@retval void
*/
 void PWM_Gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
}

void Tim3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	//NVIC_InitTypeDef NVIC_InitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//ʹ�ܶ�ʱ��ʱ��
	
	/*����ʱ��*/
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; // 72MHZ
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; //����ģʽΪ���ϼ���
	TIM_TimeBaseInitStruct.TIM_Period = arr; //������װֵ ARR 0~65535
	TIM_TimeBaseInitStruct.TIM_Prescaler = psc; //���÷�Ƶϵ��������CK_CNT
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);//��ʼ����ʱ��
	
	/*���ö�ʱ�����*/
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;		//��ʱ�����ģʽΪ�����ȵ���1
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable; //
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High; //��ʱ���������
	TIM_OC3Init(TIM3,&TIM_OCInitStruct);
	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	
	TIM_Cmd(TIM3,ENABLE);//ʹ�ܶ�ʱ��
}


/**
*@brief 
*@param pulse ����ռ�ձ� 0~1000
*@retval void
*/

extern int time;

void PWM_Config(uint16_t pulse)
{
	TIM_SetCompare3(TIM3,pulse);
}
/**
*@brief �����ʼ���ӿ�
*@param null
*@retval void
*/
void PWM_EN(void)
{
	PWM_Gpio_Init();
	Tim3_Int_Init(1000,71);

}

