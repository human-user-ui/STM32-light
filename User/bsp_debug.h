#ifndef _BSP_DEBUG_H_
#define _BSP_DEBUG_H_

#include <stm32f10x.h>
#include <stdio.h>
// ���� 1-USART1 #define DEBUG_USARTx 							USART1
#define DEBUG_USARTx								USART1
#define DEBUG_USART_CLK 						RCC_APB2Periph_USART1
#define DEBUG_USART_APBxClkCmd			RCC_APB2PeriphClockCmd
#define DEBUG_USART_BAUDRATE 				115200
// USART GPIO ���ź궨��
#define DEBUG_USART_GPIO_CLK				(RCC_APB2Periph_GPIOA)
#define DEBUG_USART_GPIO_APBxClkCmd	RCC_APB2PeriphClockCmd
#define DEBUG_USART_TX_GPIO_PORT		GPIOA
#define DEBUG_USART_TX_GPIO_PIN			GPIO_Pin_9
#define DEBUG_USART_RX_GPIO_PORT		GPIOA
#define DEBUG_USART_RX_GPIO_PIN			GPIO_Pin_10
#define DEBUG_USART_IRQ							USART1_IRQn
#define DEBUG_USART_IRQHandler			USART1_IRQHandler
 
 
void Usart1Init(void); 
void Usart_SendByte(USART_TypeDef * pUSARTx,uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendInt( USART_TypeDef * pUSARTx, int num);
#endif
