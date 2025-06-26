#include "bsp_AD_Light.h"
#define ADC_GPIO_Pin GPIO_Pin_0

/**
*@brief adc引脚初始化内部功能函数
*@param	void
*@retval void
*/
static void adc_Gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = ADC_GPIO_Pin;
	
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
*@brief adc功能初始化内部功能函数
*@param void
*@retval void
*/
static void adc_Mode_Init()
{
	/*ADC初始化结构体*/
	ADC_InitTypeDef ADC_InitStruct;
	/*打开ADC时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	/*ADC模式选择为独立通道*/
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	/*ADC 禁止扫描模式，单通道不需要扫描，多通道需要*/
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	/*ADC连续转换模式打开*/
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	/*ADC 出发方式选择不用外部出发*/
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	/*ADC 转换结果对齐选择右对齐*/
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	/*ADC 转换通道选择一个*/
	ADC_InitStruct.ADC_NbrOfChannel = 1;
	/*ADC初始化函数*/
	ADC_Init(ADC1,&ADC_InitStruct);
	/*设置分频系数*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	/*设置ADC1的转换通道 以及转换顺序为 1 ，第一个转换，采样时间为55.5个时钟周期*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10,1,ADC_SampleTime_55Cycles5);
	/*ADC转换结束后产生中断请求*/
	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
	/*开启ADC，并开始转换*/
	ADC_Cmd(ADC1,ENABLE);
	/*初始化ADC 校准寄存器*/
	ADC_ResetCalibration(ADC1);
	/*等待ADC校准寄存器初始化完成*/
	while(ADC_GetResetCalibrationStatus(ADC1));
	/*ADC开始校准*/
	ADC_StartCalibration(ADC1);
	/*等待校准完成*/
	while(ADC_GetCalibrationStatus(ADC1) != SET);
	/*开启软件触发*/
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
*@brief adc中断配置功能函数
*@param void
*@retval void
*/
static void adc_Nvic_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

/**
*@brief 为外部提供的接口函数，用于功能初始化吗
*@param void
*@retval void
*/
void ADC_EN(void)
{
	adc_Gpio_Init();
	adc_Mode_Init();
	adc_Nvic_Config();
}


