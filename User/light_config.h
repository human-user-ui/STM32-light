#ifndef _LIGHT_CONFIG_H_
#define _LIGHT_CONFIG_H_

#include "bsp_AD_Light.h"
#include "bsp_PWM.h"
void light_GPIO_Init(void);
void light_config(void);
void light_up(void);
void light_down(void);
void light_off(void);
void light_config_huyan(void);
void light_config_zhaoming(void);
#endif
