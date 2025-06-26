#include "LD3320.h"

#define GPIO_PORT_RCC RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOC
#define GPIO_PORT GPIOE|GPIOB|GPIOC

#define INTB GPIO_Pin_4
#define RST GPIO_Pin_5
#define MD GPIO_Pin_1
#define RDB GPIO_Pin_8
#define A0 GPIO_Pin_9
#define SPI_CS GPIO_Pin_10
#define SPI_EN GPIO_Pin_11
#define SPI_IN GPIO_Pin_12
#define SPI_OUT GPIO_Pin_13
#define SPI_CLK GPIO_Pin_14

#define GPIO_OUT_PP_PIN SPI_CLK|SPI_OUT|SPI_IN|SPI_EN|SPI_CS|A0|RDB

/*Ƭѡ�ź�*/
#define SPI_CS_0 GPIO_ResetBits(GPIOE,SPI_CS)
#define SPI_CS_1 GPIO_SetBits(GPIOE,SPI_CS)

/*SPI_CLK*/
#define SPI_CLK_0 GPIO_ResetBits(GPIOE,SPI_CLK)
#define SPI_CLK_1 GPIO_SetBits(GPIOE,SPI_CLK)

/*A0*/
#define A0_0 GPIO_ResetBits(GPIOE,A0)
#define A0_1 GPIO_SetBits(GPIOE,A0)

/*SPI_OUT*/
#define SPI_OUT_0 GPIO_ResetBits(GPIOE,SPI_OUT)
#define SPI_OUT_1 GPIO_SetBits(GPIOE,SPI_OUT)
#define SPI_OUT_Read GPIO_ReadInputDataBit(GPIOE,SPI_OUT)
 
/*SPI_IN*/
#define SPI_IN_0 GPIO_ResetBits(GPIOE,SPI_IN)
#define SPI_IN_1 GPIO_SetBits(GPIOE,SPI_IN)

/*RDB*/
#define RDB_0 GPIO_ResetBits(GPIOE,RDB)
#define RDB_1 GPIO_SetBits(GPIOE,RDB)

/*RST*/
#define RST_0 GPIO_ResetBits(GPIOC,RST)
#define RST_1 GPIO_SetBits(GPIOC,RST)

/*SPI_EN*/
#define SPI_EN_0 GPIO_ResetBits(GPIOE,SPI_EN)
#define SPI_EN_1 GPIO_SetBits(GPIOE,SPI_EN)

#define CLK_IN   		32.768	
#define LD_PLL_11			(uint8_t)((CLK_IN/2.0)-1)
#define LD_PLL_MP3_19		0x0f
#define LD_PLL_MP3_1B		0x18
#define LD_PLL_MP3_1D   	(uint8_t)(((90.0*((LD_PLL_11)+1))/(CLK_IN))-1)

#define LD_PLL_ASR_19 		(uint8_t)(CLK_IN*32.0/(LD_PLL_11+1) - 0.51)
#define LD_PLL_ASR_1B 		0x48
#define LD_PLL_ASR_1D 		0x1f
/*LD3320ϵͳ�Ĺ���״̬*/
enum AsrStatus{
	LD_ASR_NONE, 	         //	LD_ASR_NONE:		��ʾû������ASRʶ��
	LD_ASR_RUNING,				 //	LD_ASR_RUNING��		��ʾLD3320������ASRʶ����
	LD_ASR_FOUNDOK,        //	LD_ASR_FOUNDOK:		��ʾһ��ʶ�����̽�������һ��ʶ����
	LD_ASR_FOUNDZERO,			 //	LD_ASR_FOUNDZERO:	��ʾһ��ʶ�����̽�����û��ʶ����
	LD_ASR_ERROR           //	LD_ASR_ERROR:		��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬
};
/*��ǰLD332�Ĺ���״̬*/
enum LD_Mode{
	LD_MODE_MP3,
	LD_MODE_ASR_RUN
};
enum AsrStatus nAsrStatus = LD_ASR_NONE;
enum LD_Mode nLD_Mode;

/**
*@brief ͨѶʱ�������ʱ���� �ڲ����ܺ���
*@param ms ��ʱʱ��
*@retval void
*/
static void  delay(unsigned int ms)
{
	unsigned int j  =  0;
	unsigned int g  =  0;
	for (j=0;j<ms;j++)
	{
		for (g=0;g<12;g++)
		{
		}
	}
}

void  delay_2(unsigned long uldata)
{
	unsigned int j  =  0;
	unsigned int g  =  0;
	for (j=0;j<5;j++)
	{
		for (g=0;g<uldata;g++)
		{
			__nop();
			__nop();
			__nop();
		}
	}
}
/**
*@brief �Թ������Ž��г�ʼ�� �ڲ�����
*@param null
*@retval void
*/
static void LD3320_Gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(GPIO_PORT_RCC, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_OUT_PP_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = RST;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = MD;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = INTB;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
}

static void LD3320_EXIT_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
		
	GPIO_InitStruct.GPIO_Pin = INTB;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,INTB);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line14;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);

}
/**
*@brief ��LD3320���и�λ
*@param null
*@retval void
*/
void LD_reset()
{
	LD3320_Gpio_Init();
	
	RST_1;
	delay(1);
	RST_0;
	delay(1);
	RST_1;
	
	delay(1);
	SPI_CS_0;
	delay(1);
	SPI_CS_1;
	delay(1);
	
	/*������ģʽ����Ϊ���й���ģʽ��*/
	GPIO_SetBits(GPIOB,MD);
}

/**
*@brief LD3320 д��ָ���
*@param ָ��
*@retval void
*/
static void LD3320_Write_Command(uint8_t Command)
{
	uint16_t i;
	delay(1);
	for(i = 0; i < 8; i++)
	{
		SPI_CLK_1;
		if((Command & 0x80) == 0x80)
		{
			SPI_IN_1;
		}else
		{
			SPI_IN_0;
		}
		Command = Command<<1;
		delay(1);
		SPI_CLK_0;
		delay(1);
	}
}
/**
*@brief LD3320��ַд�뺯��
*@param addr �Ĵ�����ַ
*@retval void
*/
static void Ld3320_Write_Addr(uint8_t addr)
{
	uint16_t i;
	A0_1;
	delay(1);
	for(i = 0; i < 8; i++)
	{
		SPI_CLK_1;
		if((addr & 0x80) == 0x80)
		{
			SPI_IN_1;
		}else
		{
			SPI_IN_0;
		}
		addr = addr << 1;
		delay(1);
		SPI_CLK_0;
		delay(1);
	}
}
/**
*@brief LD3320д������
*@param data һ�ֽ�����
*@retval void
*/
static void LD3320_Write_Data(uint8_t data)
{
	uint16_t i;
	A0_0;
	delay(1);
	for(i = 0; i < 8; i++)
	{
		SPI_CLK_1;
		if((data & 0x80) == 0x80)
		{
			SPI_IN_1;
		}else
		{
			SPI_IN_0;
		}
		data = data<<1;
		delay(1);
		SPI_CLK_0;
		delay(1);
	}
}

void LD3320_Write_byte(uint8_t addr, uint8_t data)
{
	SPI_EN_0;
	SPI_CS_0;
	
	delay(1);
	LD3320_Write_Command(0x04);
	Ld3320_Write_Addr(addr);
	LD3320_Write_Data(data);
	
	SPI_EN_1;
	SPI_CS_1;
}

uint8_t LD3320_Read_byte(uint8_t addr)
{
	uint8_t temp = 0, i, data = 0;
	SPI_EN_0;
	SPI_CS_0;
	delay(1);
	LD3320_Write_Command(0x05);
	Ld3320_Write_Addr(addr);
	SPI_EN_1;
	RDB_0;
	
	for(i = 0; i < 8; i++)
	{
		data = (data << 1);
		temp = SPI_OUT_Read;
		SPI_CLK_0;
		if(temp == 1)
			data |= 0x01;
		delay(1);
		SPI_CLK_1;
	}
	
	delay(1);
	SPI_CS_1;
	return data;
}


/**
*@brief LDͨ�ó�ʼ��
*@param null
*@retval void
*/
static void LD_Init_Common()
{
	LD3320_Read_byte(0x06);
	/*��LD3320������λ*/
	LD3320_Write_byte(0x17,0x35);
	delay(10);
	/*��ȡFIFO�Ĵ�����״̬*/
	LD3320_Read_byte(0x06);
	/*ģ���·���� ��ʼ��*/
	LD3320_Write_byte(0x89,0x03);
	delay(5);
	LD3320_Write_byte(0xCF, 0x43);   
	delay(5);
	LD3320_Write_byte(0xCB, 0x02);
	
	/*�ڲ�ʱ������*/
	LD3320_Write_byte(0x11, LD_PLL_11);       
	if (nLD_Mode == LD_MODE_MP3)
	{
		LD3320_Write_byte(0x1E, 0x00); 
		LD3320_Write_byte(0x19, LD_PLL_MP3_19);   
		LD3320_Write_byte(0x1B, LD_PLL_MP3_1B);   
		LD3320_Write_byte(0x1D, LD_PLL_MP3_1D);
	}
	else
	{
		LD3320_Write_byte(0x1E,0x00);
		LD3320_Write_byte(0x19, LD_PLL_ASR_19); 
		LD3320_Write_byte(0x1B, LD_PLL_ASR_1B);		
	  LD3320_Write_byte(0x1D, LD_PLL_ASR_1D);
	}
	delay(10);
	
	LD3320_Write_byte(0xCD, 0x04);
	LD3320_Write_byte(0x17, 0x4c); 
	delay(5);
	LD3320_Write_byte(0xB9, 0x00);
	LD3320_Write_byte(0xCF, 0x4F); 
	LD3320_Write_byte(0x6F, 0xFF); 
}

void LD_Init_ASR()
{
	nLD_Mode=LD_MODE_ASR_RUN;
	LD_Init_Common();

	LD3320_Write_byte(0xBD, 0x00);
	LD3320_Write_byte(0x17, 0x48);
	delay( 10 );

	LD3320_Write_byte(0x3C, 0x80);    
	LD3320_Write_byte(0x3E, 0x07);
	LD3320_Write_byte(0x38, 0xff);    
	LD3320_Write_byte(0x3A, 0x07);
	
	LD3320_Write_byte(0x40, 0);          
	LD3320_Write_byte(0x42, 8);
	LD3320_Write_byte(0x44, 0);    
	LD3320_Write_byte(0x46, 8); 
	delay( 1 );
}

void LD_Init_MP3()
{
	nLD_Mode = LD_MODE_MP3;
	LD_Init_Common();

	LD3320_Write_byte(0xBD,0x02);
	LD3320_Write_byte(0x17, 0x48);
	delay(10);

	LD3320_Write_byte(0x85, 0x52); 
	LD3320_Write_byte(0x8F, 0x00);  
	LD3320_Write_byte(0x81, 0x00);
	LD3320_Write_byte(0x83, 0x00);
	LD3320_Write_byte(0x8E, 0xff);
	LD3320_Write_byte(0x8D, 0xff);
    delay(1);
	LD3320_Write_byte(0x87, 0xff);
	LD3320_Write_byte(0x89, 0xff);
	delay(1);
	LD3320_Write_byte(0x22, 0x00);    
	LD3320_Write_byte(0x23, 0x00);
	LD3320_Write_byte(0x20, 0xef);    
	LD3320_Write_byte(0x21, 0x07);
	LD3320_Write_byte(0x24, 0x77);          
  LD3320_Write_byte(0x25, 0x03);
  LD3320_Write_byte(0x26, 0xbb);    
  LD3320_Write_byte(0x27, 0x01); 
}
static void LD_AsrStart()
{
	LD_Init_ASR();
}

uint8_t ucSPVol=15; // MAX=15 MIN=0		//	Speaker�������������

void LD_AdjustMIX2SPVolume(uint8_t val)
{
	ucSPVol = val;
	val = ((15-val)&0x0f) << 2;
	LD3320_Write_byte(0x8E, val | 0xc3); 
	LD3320_Write_byte(0x87, 0x78); 
}

/*�ж�ASR��æ��״̬*/
static uint8_t LD_Check_ASRBusyFlag_b2()
{
	uint8_t j;
	uint8_t flag = 0;
	for (j=0; j<10; j++)
	{
		if (LD3320_Read_byte(0xb2) == 0x21)
		{
			flag = 1;
			break;
		}
		delay(10);		
	}
	return flag;
}

/**
*@brief ʶ�����Ӻ���
*@param null
*@retval void
*/
#define CODE_DEFAULT	0
#define CODE_KAIDENG	1
#define CODE_GUANDENG	2
#define CODE_UP 3
#define CODE_DOWN 4
#define CODE_HUYAN	5
#define CODE_ZHAOMING	6

/**
*@brief ʶ�����Ӻ���
*@param null
*@retval uint8_t flag LD״̬���� 1������ 0 ������
*/
static uint8_t LD_AsrAddFixed()
{
	uint8_t k, flag;
	uint8_t nAsrAddLength;
	const char sRecog[6][25] = {"kai deng", "guan deng", 
		"deng guang tiao liang", "deng guang tiao an", "hu yan mo shi", "zhao ming mo shi"};
		const uint8_t pCode[6] = {CODE_KAIDENG, CODE_GUANDENG, CODE_UP, CODE_DOWN, CODE_HUYAN,CODE_ZHAOMING};
		
	flag = 1;
	for (k=0; k<6; k++)
	{
			
		if(LD_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}
		
		LD3320_Write_byte(0xc1, pCode[k]);
		LD3320_Write_byte(0xc3, 0 );
		LD3320_Write_byte(0x08, 0x04);
		delay(1);
		LD3320_Write_byte(0x08, 0x00);
		delay(1);

		for (nAsrAddLength=0; nAsrAddLength<30; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD3320_Write_byte(0x5, sRecog[k][nAsrAddLength]);
		}
		//����оƬ��ӵ��ַ�����
		LD3320_Write_byte(0xb9, nAsrAddLength);
		//DSPæ
		LD3320_Write_byte(0xb2, 0xff);
		//����оƬҪ���һ��ʶ���
		LD3320_Write_byte(0x37, 0x04);
	}
    return flag;

}
#define MIC_VOL 0x43 //0x00-0x7f��˷���������
uint8_t LD_AsrRun()
{
	//������˷�����
	LD3320_Write_byte(0x35, MIC_VOL);
	
	LD3320_Write_byte(0x1C, 0x09);
	LD3320_Write_byte(0xBD, 0x20);
	LD3320_Write_byte(0x08, 0x01);
	delay( 1 );
	LD3320_Write_byte(0x08, 0x00);
	delay( 1 );

	if(LD_Check_ASRBusyFlag_b2() == 0)
	{
		return 0;
	}

	LD3320_Write_byte(0xB2, 0xff);	
	LD3320_Write_byte(0x37, 0x06);
	delay( 5 );
	LD3320_Write_byte(0x1C, 0x0b);
	LD3320_Write_byte(0x29, 0x10);
	
	LD3320_Write_byte(0xBD, 0x00);
	
  LD3320_EXIT_Init();
	
	return 1;

}


uint8_t RunASR()
{
	uint8_t i = 0;
	uint8_t asrflag = 0;
	for (i=0; i<5; i++)			//	��ֹ����Ӳ��ԭ����LD3320оƬ����������������һ������5������ASRʶ������
	{
		LD_AsrStart();
		delay(100);
		if (LD_AsrAddFixed()==0)
		{
			LD_reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			delay(100);			//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}
		
		delay(10);
		if (LD_AsrRun() == 0)
		{
			LD_reset();			//	LD3320оƬ�ڲ����ֲ���������������LD3320оƬ
			delay(100);			//	���ӳ�ʼ����ʼ����ASRʶ������
			continue;
		}

		asrflag=1;
		break;					//	ASR���������ɹ����˳���ǰforѭ������ʼ�ȴ�LD3320�ͳ����ж��ź�
	}
	return asrflag;
}

#define        CAUSE_MP3_SONG_END           0x20
uint8_t bMp3Play=0;							//	������¼����MP3��״̬

uint8_t ucRegVal;
uint8_t  ucRegVal;
uint8_t ucHighInt;
uint8_t ucLowInt;
uint8_t ucStatus;
uint32_t nMp3StartPos=0;
uint32_t nMp3Size=0;
uint32_t nMp3Pos=0;
uint32_t nCurMp3Pos=0;

void ProcessInt0()
{
	uint8_t nAsrResCount=0;
	ucRegVal = LD3320_Read_byte(0x2B);
	if(nLD_Mode == LD_MODE_ASR_RUN)
	{
		// ����ʶ��������ж�
		// �����������룬����ʶ��ɹ���ʧ�ܶ����жϣ�
		LD3320_Write_byte(0x29,0) ;
		LD3320_Write_byte(0x02,0) ;
		if((ucRegVal & 0x10) &&
			LD3320_Read_byte(0xb2)==0x21 && 
			LD3320_Read_byte(0xbf)==0x35)
		{
			nAsrResCount = LD3320_Read_byte(0xba);
			if(nAsrResCount>0 && nAsrResCount<=4) 
			{
				nAsrStatus=LD_ASR_FOUNDOK;
			}
			else
		    {
				nAsrStatus=LD_ASR_FOUNDZERO;
			}	
		}
		else
		{
			nAsrStatus=LD_ASR_FOUNDZERO;
		}
			
	  	LD3320_Write_byte(0x2b, 0);
    	LD3320_Write_byte(0x1C,0);
		return;
	}
	
	// �������Ų������жϣ������֣�
	// A. ����������ȫ�������ꡣ
	// B. ���������ѷ�����ϡ�
	// C. ����������ʱ��Ҫ���꣬��Ҫ�����µ����ݡ�	
	ucHighInt = LD3320_Read_byte(0x29); 
	ucLowInt=LD3320_Read_byte(0x02); 
	LD3320_Write_byte(0x29,0) ;
	LD3320_Write_byte(0x02,0) ;
    if(LD3320_Read_byte(0xBA)&CAUSE_MP3_SONG_END)
    {
	// A. ����������ȫ�������ꡣ

		LD3320_Write_byte(0x2B,  0);
    LD3320_Write_byte(0xBA, 0);	
		LD3320_Write_byte(0xBC,0x0);	
		bMp3Play=0;					// ��������ȫ����������޸�bMp3Play�ı���
		LD3320_Write_byte(0x08,1);
		delay_2(5);
     LD3320_Write_byte(0x08,0);
		LD3320_Write_byte(0x33, 0);

		return ;
     }

	 if(nMp3Pos>=nMp3Size)
	{
// B. ���������ѷ�����ϡ�

		LD3320_Write_byte(0xBC, 0x01);
		LD3320_Write_byte(0x29, 0x10);
//		bMp3Play=0;				//	��ʱ��ֻ������MCU������MP3���ݷ��͵�LD3320оƬ�ڣ����ǻ�û�а����������ȫ���������

		return;	
	}

	// C. ����������ʱ��Ҫ���꣬��Ҫ�����µ����ݡ�	

	LD_ReloadMp3Data_2();
		
	LD3320_Write_byte(0x29,ucHighInt); 
	LD3320_Write_byte(0x02,ucLowInt) ;

	delay_2(10);


}

#include <bsp_flash.h>
#define        RESUM_OF_MUSIC               0x01
#define        CAUSE_MP3_SONG_END           0x20
#define        MASK_INT_SYNC				0x10
#define        MASK_INT_FIFO				0x04
#define    	   MASK_AFIFO_INT				0x01
#define        MASK_FIFO_STATUS_AFULL		0x08



/**
*@brief ��ȡMP3����
*/
void LD_ReloadMp3Data_2()
{
//	uint32_t nCurMp3Pos;
	uint8_t val;
//Q	uint8_t k;
	u32 adrr = nMp3StartPos;
//	nCurMp3Pos = nMp3StartPos + nMp3Pos;
	
	ucStatus = LD3320_Read_byte(0x06);
	while ( !(ucStatus&MASK_FIFO_STATUS_AFULL) && (nMp3Pos<nMp3Size) )
	{
		val=0;
		SPI_FLASH_BufferRead(&val,adrr++,1);
		LD3320_Write_byte(0x01,val);
		nMp3Pos++;

		ucStatus = LD3320_Read_byte(0x06);
	}

}
void LD_play()
{
	nMp3Pos=0;
	bMp3Play=1;

	if (nMp3Pos >=  nMp3Size)
		return ; 

	LD_ReloadMp3Data_2();

  LD3320_Write_byte(0xBA, 0x00);
	LD3320_Write_byte(0x17, 0x48);
	LD3320_Write_byte(0x33, 0x01);
	LD3320_Write_byte(0x29, 0x04);
	
	LD3320_Write_byte(0x02, 0x01); 
	LD3320_Write_byte(0x85, 0x5A);


}
#define MP3_KAIDENG_START  0x050000
#define MP3_KAIDENG_SIZE   0x05207E
#define MP3_GUANDENG_START 0x020000
#define MP3_GUANDENG_SIZE  0x022156
#define MP3_UP_START 0x060000
#define MP3_UP_SIZE  0x062036
#define MP3_DOWN_START  0x060000
#define MP3_DOWN_SIZE   0x062036
#define MP3_HUYAN_START 0x030000
#define MP3_HUYAN_SIZE  0x0326F6
#define MP3_ZHAOMING_START 0x040000
#define MP3_ZHAOMING_SIZE  0x4273E
#define MP3_NIHAO_START 0x000000
#define MP3_NIHAO_SIZE  0x00234E
void PlaySound(uint8_t nCode)
{
	switch(nCode)
	{
	case CODE_KAIDENG:
		nMp3StartPos = MP3_KAIDENG_START;
		nMp3Size = MP3_KAIDENG_SIZE;
		break;
	case CODE_GUANDENG:
		nMp3StartPos = MP3_GUANDENG_START;
		nMp3Size = MP3_GUANDENG_SIZE;
		break;
	case CODE_UP:
		nMp3StartPos = MP3_UP_START;
		nMp3Size = MP3_UP_SIZE;
		break;
	case CODE_DOWN:
		nMp3StartPos = MP3_DOWN_START;
		nMp3Size = MP3_DOWN_SIZE;
		break;
	case CODE_HUYAN:
		nMp3StartPos = MP3_HUYAN_START;
		nMp3Size = MP3_HUYAN_SIZE;
		break;
	case CODE_ZHAOMING:
		nMp3StartPos = MP3_ZHAOMING_START;
		nMp3Size = MP3_ZHAOMING_SIZE;
		break;
	default:
		nMp3StartPos = MP3_NIHAO_START;
		nMp3Size = MP3_NIHAO_SIZE;
		break;		
	}

	LD_Init_MP3();
	LD_AdjustMIX2SPVolume(15);
	LD_play();
}

uint8_t LD_GetResult()
{
	return LD3320_Read_byte(0xc5 );
}


void ld3320_work(void)
{
		uint8_t  nAsrRes;

	
		if (bMp3Play != 0)			//	������ڲ���MP3�У���ȴ���ֱ��MP3������� ��bMp3Play==0
							//	bMp3Play �Ƕ����һ��ȫ�ֱ���������¼MP3���ŵ�״̬������LD3320оƬ�ڲ��ļĴ���

		switch(nAsrStatus)
		{
			case LD_ASR_RUNING:
			case LD_ASR_ERROR:		
				break;
			case LD_ASR_NONE:
			{
				
				nAsrStatus=LD_ASR_RUNING;
				if (RunASR()==0)	//	����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����
				{
					nAsrStatus = LD_ASR_ERROR;
		
				}
				break;
			}
			case LD_ASR_FOUNDOK:
			{
				
				nAsrRes = LD_GetResult();	//	һ��ASRʶ�����̽�����ȥȡASRʶ����
				
				PlaySound(nAsrRes);
				nAsrStatus = LD_ASR_NONE;
				break;
			}
			case LD_ASR_FOUNDZERO:
			default:
			{
				PlaySound(CODE_DEFAULT);
				nAsrStatus = LD_ASR_NONE;
				break;
			}
		}// switch
	// while

}
