#include "bsp_AD_Light.h"
#define ADC_GPIO_Pin GPIO_Pin_0

/**
*@brief adc���ų�ʼ���ڲ����ܺ���
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
*@brief adc���ܳ�ʼ���ڲ����ܺ���
*@param void
*@retval void
*/
static void adc_Mode_Init()
{
	/*ADC��ʼ���ṹ��*/
	ADC_InitTypeDef ADC_InitStruct;
	/*��ADCʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	/*ADCģʽѡ��Ϊ����ͨ��*/
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	/*ADC ��ֹɨ��ģʽ����ͨ������Ҫɨ�裬��ͨ����Ҫ*/
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	/*ADC����ת��ģʽ��*/
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	/*ADC ������ʽѡ�����ⲿ����*/
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	/*ADC ת���������ѡ���Ҷ���*/
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	/*ADC ת��ͨ��ѡ��һ��*/
	ADC_InitStruct.ADC_NbrOfChannel = 1;
	/*ADC��ʼ������*/
	ADC_Init(ADC1,&ADC_InitStruct);
	/*���÷�Ƶϵ��*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	/*����ADC1��ת��ͨ�� �Լ�ת��˳��Ϊ 1 ����һ��ת��������ʱ��Ϊ55.5��ʱ������*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10,1,ADC_SampleTime_55Cycles5);
	/*ADCת������������ж�����*/
	ADC_ITConfig(ADC1,ADC_IT_EOC,ENABLE);
	/*����ADC������ʼת��*/
	ADC_Cmd(ADC1,ENABLE);
	/*��ʼ��ADC У׼�Ĵ���*/
	ADC_ResetCalibration(ADC1);
	/*�ȴ�ADCУ׼�Ĵ�����ʼ�����*/
	while(ADC_GetResetCalibrationStatus(ADC1));
	/*ADC��ʼУ׼*/
	ADC_StartCalibration(ADC1);
	/*�ȴ�У׼���*/
	while(ADC_GetCalibrationStatus(ADC1) != SET);
	/*�����������*/
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/**
*@brief adc�ж����ù��ܺ���
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
*@brief Ϊ�ⲿ�ṩ�Ľӿں��������ڹ��ܳ�ʼ����
*@param void
*@retval void
*/
void ADC_EN(void)
{
	adc_Gpio_Init();
	adc_Mode_Init();
	adc_Nvic_Config();
}


