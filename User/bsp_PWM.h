#ifndef _BSP_PWM_H
#define _BSP_PWM_H
#include <stm32f10x.h>



void PWM_EN(void);
void PWM_Config(uint16_t pulse);
void TIM3_NVIC_Config(void);
#endif
