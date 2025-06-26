#include "bsp_OLED.h"
#include "bsp_OLED_Data.h"

#define OLED_SLAVE_ADDRESS 0x78
#define OWN_SLAVE_ADDRESS 0XAA
#define COMMAND 0X00
#define DATA 0X40


uint8_t GRAM[4][128];

void GRAM_0()
{
	int i,j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 128; j++)
		{
			GRAM[i][j] = 0x00;
		}
	}
}


/**
*@brief 延时函数 单位MS
*@param ms 延时时间
*@retval void
*/
static void oled_Delayms(uint16_t ms)
{
	uint16_t i, j;
	for(i = 0; i < ms; i++)
	{
		for(j = 0; j < 123; j++);
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

/**
*@brief 初始化I2C外设所用GPIO引脚
*@param null
*@retval void
*/
static void oled_I2c_Gpio_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_I2C1EN,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}


/**
*@brief 初始化硬件I2C外设
*@param null
*@retval void
*/
static void oled_I2c_Init(void)
{
	I2C_InitTypeDef I2C_InitStruct;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_ClockSpeed = 300000 ;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = OWN_SLAVE_ADDRESS;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStruct.I2C_Ack = ENABLE;
	I2C_Init(I2C1,&I2C_InitStruct);
	I2C_Cmd(I2C1,ENABLE);
}

/**
*@brief 暂定为I2C超时处理函数，可做串口输出调试I2C使用
*@param null
*@retval void
*/
//static void i2c_TIMEOUT_UseCallback(uint8_t errorCode)
//{
//}	

/**
*@brief 写一个字节进入OLED中
*@param byte: 写入字节数据
*@param mode: 写入命令还是数据
*@retval void
*/
void I2c_Write_byte(uint8_t byte, uint8_t mode)
{
	/*发送起始位*/
	I2C_GenerateSTART(I2C1, ENABLE);
	/*检查事件5是否发生 并将标志位SB置1*/
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
	{
	
	}
	/*发送7位设备地址，并确定方向读或者写*/
	I2C_Send7bitAddress(I2C1,OLED_SLAVE_ADDRESS,I2C_Direction_Transmitter);
	/*检查事件6是否发生 并清除标志位ADDR*/	
	while(!I2C_CheckEvent(I2C1,
				I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
		
	}
	/*发送命令*/
	I2C_SendData(I2C1,mode);
	/*检查事件8是否发生 并清除标志位*/
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	}
	/**/
	I2C_SendData(I2C1,byte);
	/**/
	while(! I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	}
	/**/
	I2C_GenerateSTOP(I2C1,ENABLE);
}


/**
*@brief 初始化OLED屏幕
*@param null
*@retval void
*/
static void oled_Init(void)
{
	oled_Delayms(100);
	// 初始化序列
    I2c_Write_byte(0xAE,COMMAND); // 关闭显示
    
    I2c_Write_byte(0xD5,COMMAND); // 设置时钟分频
    I2c_Write_byte(0x80,COMMAND);
    
    I2c_Write_byte(0xA8,COMMAND); // 设置多路复用率
    I2c_Write_byte(0x1F,COMMAND);
    
    I2c_Write_byte(0xD3,COMMAND); // 设置显示偏移
    I2c_Write_byte(0x00,COMMAND);
    
    I2c_Write_byte(0x40,COMMAND); // 设置显示起始行
    
    I2c_Write_byte(0x8D,COMMAND); // 电荷泵设置
    I2c_Write_byte(0x14,COMMAND);
    
    I2c_Write_byte(0x20,COMMAND); // 内存地址模式
    I2c_Write_byte(0x00,COMMAND);
    
    I2c_Write_byte(0xA1,COMMAND); // 段重映射
    I2c_Write_byte(0xC8,COMMAND); // 扫描方向
    
    I2c_Write_byte(0xDA,COMMAND); // COM引脚配置
    I2c_Write_byte(0x02,COMMAND);
    
    I2c_Write_byte(0x81,COMMAND); // 对比度设置
    I2c_Write_byte(0xCF,COMMAND);
    
    I2c_Write_byte(0xD9,COMMAND); // 预充电周期
    I2c_Write_byte(0xF1,COMMAND);
    
    I2c_Write_byte(0xDB,COMMAND); // VCOMH电平
    I2c_Write_byte(0x40,COMMAND);
    
    I2c_Write_byte(0xA4,COMMAND); // 正常显示
    I2c_Write_byte(0xA6,COMMAND); // 非反转显示
    
    I2c_Write_byte(0xAF,COMMAND); // 开启显示
		//将缓存数据初始化
		GRAM_0();
	
	/*打开显示*/
	I2c_Write_byte(0xAF,COMMAND);
	
}

/**
*@brief OLED总初始化
*@param null
*@retval void
*/
void OLED_EN(void)
{
	oled_I2c_Gpio_Init();
	oled_I2c_Init();
	oled_Init();
	oled_clean();
}


/**
*@brief OLED清屏函数
*@param null
*@retval void
*/
void oled_clean(void)
{
	
	unsigned char m, n;
	oled_Delayms(3000);
	for(m = 0; m <= 8; m++)
	{
		/*设置显示页*/
		I2c_Write_byte(0xB0+m,COMMAND);
		/*设置显示高四位和低四位*/
		I2c_Write_byte(0x00,COMMAND);
		I2c_Write_byte(0x10,COMMAND);
		/**/
		for(n = 0; n < 128; n++)
		{
			I2c_Write_byte(0x00,DATA);
		}
	}
}


/**
*@brief 设置光标位置 
*@param page 光标所在页
*@param x 光标所在列
*@retval void
*/
#define OLED_WIDTH 127
#define OLED_HEIGHT 128
void OLED_SetCursor(uint8_t y, uint8_t x)
{
	/*-------- 边界检查 --------*/
    // X坐标限制在0-127范围
    x = (x >= OLED_WIDTH) ? (OLED_WIDTH - 1) : x;
    
    // Y坐标限制在0-3范围（32像素高，4页）
    y = (y >= 3) ? 3 : y;

    /*-------- 设置地址 --------*/
    // 1. 设置页地址（0xB0 - 0xB7）
    I2c_Write_byte(0xB0 | y,COMMAND);
    
    // 2. 设置列地址低半字节（0x00-0x0F）
    I2c_Write_byte(0x00 | (x & 0x0F),COMMAND);
    
    // 3. 设置列地址高半字节（0x10-0x1F）
    I2c_Write_byte(0x10 | ((x >> 4) & 0x0F),COMMAND);
	/*
	I2c_Write_byte(0xB0+Page,COMMAND);
	I2c_Write_byte(((X&0xf0)>>4)|0x10,COMMAND);
	I2c_Write_byte((X&0x0f)|0x10,COMMAND);
	设置显示页*/
}

/**
*@brief 显示一个ASCII字符
*@param page 字符显示页
*@param x 字符显示列
*@param ch 显示字符内容
*@retval void 
*/
void oled_Write_Char(int page, int x, char ch)
{
	uint8_t i;
//	OLED_SetCursor(page, x);
	for(i = 0; i < 6; i++)
	{
		GRAM[page][x+i] = OLED_F8x16[ch - ' '][i];
		oled_Delayms(1);
	}
	
	//refresh();
}
/**
*@brief 显示字符串
*@param X 显示列
*@param Y 显示行
*@param String 显示字符串
*@retval void 
*/
void oled_Write_String(int X, int Y, char *String)
{
	while(*String != '\0')
	{
		oled_Write_Char(X, Y, *String);
		//oled_Delayms(5);
		String++;
		Y += 6;
	}
	//refresh();
}
/**
*@brief 显示数字
*@param page
*@param x
*@param num 0~65536
*@retval void
*/
void oled_Write_Num(int page, int x, uint16_t num, uint16_t length)
{
	uint16_t i, j;
	int nu;
	for(i = 0; i < length; i++)
	{
		nu = num/num_pow(10,length-i-1);
		num = num%num_pow(10,length-i-1);
		
		for(j = 0; j < 6; j++)
		{
			int a = nu+ 16;
			oled_Delayms(1);
			GRAM[page][x+j] = OLED_F8x16[a][j];
		}
		x+=6;
		
	}
	
	//refresh();
}

void refresh()
{
	int i,j;
	OLED_SetCursor(0,0);
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 128; j++)
		{
			I2c_Write_byte(GRAM[i][j],DATA);
			__nop();
			__nop();
		}
	}
}
