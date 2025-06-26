#ifndef _LD3320_H_
#define _LD3320_H_

#include <stm32f10x.h>
/***************内部功能函数区*****************/
static void  delay(unsigned int ms);
static void LD3320_Gpio_Init(void);
static void LD3320_EXIT_Init(void);
static void LD3320_Write_Command(uint8_t Command);
static void Ld3320_Write_Addr(uint8_t addr);
static void LD3320_Write_Data(uint8_t data);
void LD_Init_Common(void);
static void LD_AsrStart(void);
static uint8_t LD_Check_ASRBusyFlag_b2(void);
static uint8_t LD_AsrAddFixed(void);
/****************外部接口函数*******************/
void LD_reset(void);
void LD3320_Write_byte(uint8_t addr, uint8_t data);
uint8_t LD3320_Read_byte(uint8_t addr);
void LD_Init_Common(void);
uint8_t RunASR(void);
void ProcessInt0(void);
void LD_ReloadMp3Data_2(void);
void ld3320_work(void);
#endif
