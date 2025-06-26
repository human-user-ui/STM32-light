#include "hs_ceju.h"
#include "bsp_debug.h"

extern uint32_t pulse_duration;

static void HS_GPIO_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_PinRemapConfig(GPIO_FullRemap_TIM2 ,ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void TIM2_Config(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // ʱ�����ã�1MHz����Ƶ��
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 71;    // 72MHz/(71+1)=1MHz
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // ���벶������
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0x00;
    TIM_ICInit(TIM2, &TIM_ICInitStructure);

    // ʹ���ж�
    TIM_ITConfig(TIM2, TIM_IT_CC4 | TIM_IT_Update, ENABLE);

    // NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM2, ENABLE);
}
void HS_Init(void)
{
	HS_GPIO_Config();
	TIM2_Config();
}

int HS_read(void)
{
	GPIO_SetBits(GPIOB,GPIO_Pin_10);
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
//	Usart_SendString( USART1, "�ߵ�ƽʱ�䣺");
//	Usart_SendInt( USART1, pulse_duration);
//	Usart_SendByte( USART1, ' ');
//	Usart_SendByte( USART1, ' ');
//	Usart_SendByte( USART1, ' ');
//	Usart_SendString( USART1, "���Ծ��룺");
//	Usart_SendInt( USART1, pulse_duration*0.017 );
//	Usart_SendByte( USART1, ' ');
//	Usart_SendByte( USART1, ' ');

					//		Usart_SendInt(USART1,pulse_duration*0.01846);
				//Usart_SendString(USART1,"time");
	return pulse_duration*0.01846                                                                             ;
	
}
