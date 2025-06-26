#include "bsp_debug.h"



void Usart1Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);


	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;	//PA9,USART1_TxD
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//PA10,USART1_RxD
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStruct);

	USART_InitStruct.USART_BaudRate = 9600;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStruct);

	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	
//	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);

	USART_Cmd(USART1,ENABLE);
}

void Usart_SendByte(USART_TypeDef * pUSARTx,uint8_t ch)
{
	
		USART_SendData(pUSARTx,ch);
		while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TXE) == RESET)
			;
}

void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{

		unsigned int k=0;
		do{
				Usart_SendByte(pUSARTx, *(str+ k));
				k++;
			} while(*(str+ k)!='\0');
 
		/*等待发送完成*/
		while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET){
		}
}

/**
*@brief 平方功能函数
*@param X 底数
*@param Y 指数
*@retval 平方值
*/
static uint16_t num_pow(uint16_t X, uint16_t Y)
{
	uint16_t Result = 1;
	while (Y --)			//累乘Y次
	{
		Result *= X;		//每次把X累乘到结果上
	}
	return Result;
}

void Usart_SendInt( USART_TypeDef * pUSARTx, int num)
{
	uint16_t i;
	int nu, length =4;
	for(i = 0; i < length; i++)
	{
		nu = num/num_pow(10,length-i-1);
		num = num%num_pow(10,length-i-1);
		Usart_SendByte(USART1, (char)nu + '0');
	}
}
