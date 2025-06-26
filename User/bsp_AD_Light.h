#ifndef _BSP_AD_LIGHT
#define _BSP_AD_LIGHT
#include <stm32f10x.h>
#include <stm32f10x_adc.h>

#include "bsp_PWM.h"

void ADC_EN(void);
void light_config(void);
void light_config_huyan(void);
void light_config_zhaoming(void);
void light_off(void);
void zidong_config(void);
#endif

