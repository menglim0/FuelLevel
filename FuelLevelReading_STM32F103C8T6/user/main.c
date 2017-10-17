/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
**
**                                   http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			main.c
** modified Date:  		2011-1-15
** Last Version:		V1.00
** Descriptions:		CAN测试程序
**	
*********************************************************************************************************/
#include "stm32f10x.h"
#include "stm32lib.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "api.h" 
#include "usually.h"
#include "usart.h"

#include "interpol.h"

//变量定义
const char menu[] =
   "\n\r"
   "+********************* STM32开发板  ********************+\n\r"	   //"\n"：在超级终端的作用是换行
   "|                                                       |\n\r"	   //"\r" :在超级终端的作用是置顶
   "|                  USART 异步通信试验                   |\n\r"
   "|                   www.scdbit.com                      |\n\r"
   "|                   成都贝一特科技                      |\n\r"
   "|                      2012.5.15                        |\n\r"
   "|                技术支持QQ：406795003                  |\n\r"
   "+-------------------------------------------------------+\n\r";

//函数申明
void Init_LED(void);	
void Init_NVIC(void);
void Delay_Ms(uint16_t time);  
void Delay_Us(uint16_t time);

int CAN_tx_data(); 

void Delay(u32 dly);


u16 ADCData=3000;
u16 dec, sig,hun;
u16 FuelLevel_Percent;

	//unsigned char offset=0x28;  //Oil temp offset
	unsigned char offset=0x2c; //Torq offset
	
	unsigned char DelayCount=0x00; //Torq offset
	
#define ADC1_DR_Address    ((u32)0x4001244C)

unsigned int AD_value;
unsigned int AD_value_group[2];
u16 ADC_SimpleConvertValue[4];
u8 Fuel_Percent;

u16 FuelLevel_Reading_AD_InputValue[8];
u16 FuelLevel_Reading_Percent_OutputValue[8];



#define  N   50      //The sample freq is 50 
#define  M  12      //12 adc channel

/* Private variables ---------------------------------------------------------*/


vu16 ADC_ConvertedValue[4];
ErrorStatus HSEStartUpStatus;	
	
	/* Private function prototypes -----------------------------------------------*/
void ADC_Configuration(void);
//void RCC_Configuration(void);
//void GPIO_Configuration(void);
//void NVIC_Configuration(void);
void DMA_Configuration(void);

void AD_ConvertFunction(void);
u16 Get_Adc(u8 ch); 
/********************************************************************************************************
**函数信息 ：void CANInit(void)                         // WAN.CG // 2011.4.14
**功能描述 ：CAN初始化函数
**输入参数 ：无
**输出参数 ：无
**调用提示 ：
********************************************************************************************************/
void CANInit(void)
{
	GPIO_InitTypeDef       GPIO_InitStructure;
    CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;	
	//NVIC_InitTypeDef       NVIC_InitStructure;

	//PB8,PB9配置为CAN BUS
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 	
	//PB8-CAN RX 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 	
	//PB9-CAN TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	//GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);         //端口重映射到PB8,PB9
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //??????
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM=DISABLE;		//时间触发
	CAN_InitStructure.CAN_ABOM=ENABLE;		//自动离线管理
	CAN_InitStructure.CAN_AWUM=ENABLE;		//自动唤醒
	CAN_InitStructure.CAN_NART=ENABLE;		//ENABLE:错误不自动重传 DISABLE:重传
	CAN_InitStructure.CAN_RFLM=DISABLE;
	CAN_InitStructure.CAN_TXFP=DISABLE;
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;		//正常传输模式
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;			//1-4
	CAN_InitStructure.CAN_BS1=CAN_BS1_5tq;			//1-16
	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;			//1-8
	CAN_InitStructure.CAN_Prescaler=9;				//波特率为 36/(9*(1+12+7))=200k
	CAN_Init(CAN1,&CAN_InitStructure);
	
	/* CAN 过滤器设置 */
	CAN_FilterInitStructure.CAN_FilterNumber=0;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	/* 允许FMP0中断*/ 
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);


}
/********************************************************************************************************
**函数信息 ：void USB_LP_CAN1_RX0_IRQHandler(void)                    // WAN.CG // 2011.4.14 
**功能描述 ：his function handles USB Low Priority or CAN RX0 interrupts
**输入参数 ：无
**输出参数 ：无
**调用提示 ：
********************************************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{  
	CanRxMsg RxMessage;
	RxMessage.StdId=0;
	CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);

		
	//if(RxMessage.StdId==0x4C9)//油温ID
		if(RxMessage.StdId==0x554)//IBSG ID
	{
		DelayCount++;
		//ADCData = RxMessage.Data[1];
		//ADCData = RxMessage.Data[2];
		
		if(DelayCount==10)
		{

		//USART_SendData(USART1,ADCData);
		Delay(1);
		//ADCData = RxMessage.Data[1];
		//USART_SendData(USART1,ADCData);
		}
		
	}	
}	   
/*********************************************************************************************************/

/********************************************************************************************************
**函数信息 ：int main (void)                           // WAN.CG // 2011.4.15 
**功能描述 ：开机后，ARMLED闪动,CAN BUS开始接收数据（在CAN中断中接收），并以接收的数据作为LED灯闪烁频率的依据
**输入参数 ：
**输出参数 ：
**调用提示 ：
********************************************************************************************************/
int main(void)
{
		//char dat=0x01;
	SystemInit();  
	CANInit();	     //系统初始化,初始化系统时钟
	Init_NVIC();
	Init_Usart();
	GPIOInit();   	//GPIO初始化，凡是实验用到的都要初始化	 	
	Usart_Configuration(115200);	//串口配置 设置波特率为115200
	printf(menu);					//输出字符串
  ADC_Configuration();
	DMA_Configuration();

	while(1)
	{  
		//GPIO_ResetBits(GPIOC, GPIO_Pin_13);	//PC13输出低电平，点亮LED3
		//Delay(ADCData);
	//	printf(dat);	
		//GPIO_SetBits(GPIOC, GPIO_Pin_13);	//PC13输出高电平，熄灭LED3
		Delay(798);
		
				//AD_ConvertFunction();
		ADC_SimpleConvertValue[0]=Get_Adc(ADC_Channel_6);
		ADC_SimpleConvertValue[1]=Get_Adc(ADC_Channel_7);
		
		Fuel_Percent=Interpol_Calculate(1,1,4,20,3);
		FuelLevel_Percent =ADC_SimpleConvertValue[0];
		CAN_tx_data();
		
	}
    
}
/********************************************************************************************************
**函数信息 ：void Delay(u16 dly)                          // WAN.CG // 2010.12.8 
**功能描述 ：延时函数，大致为0.01毫秒
**输入参数 ：u32 dly：延时时间
**输出参数 ：无
**调用提示 ：无
********************************************************************************************************/
void Delay(u32 dly)
{
	u16  i;	
	for ( ; dly>0; dly--)
		for (i=0; i<1000; i++);
}


void Init_NVIC(void)
{ 	
  	NVIC_InitTypeDef NVIC_InitStructure;			//定义一个NVIC向量表结构体变量

	#ifdef  VECT_TAB_RAM  							//向量表基地址选择

	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  	//将0x20000000地址作为向量表基地址(RAM)
	#else  

	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); //将0x08000000地址作为向量表基地址(FLASH)  
	#endif
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel=USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
   
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置中断组 为2 

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//配置串口1为中断源
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	//设置占先优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  	//设置副优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  	//使能串口1中断
	NVIC_Init(&NVIC_InitStructure);							  	//根据参数初始化中断寄存器
}

int CAN_tx_data()
{
    CanTxMsg TxMessage;
    uint8_t TransmitMailbox = 0;
		u32 i;
   
    TxMessage.StdId=0x223;
    TxMessage.RTR=CAN_RTR_DATA;
    TxMessage.IDE=CAN_ID_STD;
    TxMessage.DLC=8;
    TxMessage.Data[0]=FuelLevel_Percent>>4;
    TxMessage.Data[1]=(FuelLevel_Percent&0x00F)<<4;
    TxMessage.Data[2]=0x00;
		TxMessage.Data[3]=0x00;
    TxMessage.Data[4]=0x00;
		TxMessage.Data[5]=0x00;
    TxMessage.Data[6]=0x00;
		TxMessage.Data[7]=0x00;
    
    TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
  i = 0;
  // 用于检查消息传输是否正常
  while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF))
  {
   i++;
  }
  i = 0;
  // 检查返回的挂号的信息数目
  while((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (i != 0xFF))
  {
   i++;
  }
	
	return 0;
}


void ADC_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
 /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					//独立模式  CR1
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;						    //扫描模式使能
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;					//连续转换模式使能
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//启动转换的外部事件--无  CR2
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//转换后的数据右对齐
  ADC_InitStructure.ADC_NbrOfChannel = 1;								//转换的通道数为1
  ADC_Init(ADC1, &ADC_InitStructure);

  //ADC_TempSensorVrefintCmd(ENABLE);
  /* ADC1 regular channel14 configuration */ 
  //ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5 );

	/* Enable ADC1 DMA */															  
  ADC_DMACmd(ADC1, DISABLE);		                          						  //允许ADC1进行DMA传送
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);														  //使能ADC1

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);													  //允许ADC1复位校准寄存器 
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));									  //检测校准寄存器是否复位完成 

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);													  //启动ADC1 校准
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));									      //检测校准是否完成 
     
  /* Start ADC1 Software Conversion */ 
  //ADC_SoftwareStartConvCmd(ADC1, ENABLE);										  //软件触发启动ADC1转换

}

void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  /* DMA channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 6;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* Enable DMA channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
}


void AD_ConvertFunction(void)
{
	AD_value=ADC_GetConversionValue(ADC1);
 	//while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
		AD_value_group[0] = ADC_ConvertedValue[0];
		AD_value_group[1] = ADC_ConvertedValue[1];
}




/******AD input Convert The par is ADC_Channel_x***********/
u16 Get_Adc(u8 ch) 
{
//Set ADC regerular channel
ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );  ////  
ADC_SoftwareStartConvCmd(ADC1, ENABLE);  //
while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//
return ADC_GetConversionValue(ADC1);  //
}

