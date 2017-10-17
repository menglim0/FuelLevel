/****************************************Copyright (c)****************************************************
** 
**                                      ���ڸ�����Ƽ�
**
**                                   http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			main.c
** modified Date:  		2011-1-15
** Last Version:		V1.00
** Descriptions:		CAN���Գ���
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

//��������
const char menu[] =
   "\n\r"
   "+********************* STM32������  ********************+\n\r"	   //"\n"���ڳ����ն˵������ǻ���
   "|                                                       |\n\r"	   //"\r" :�ڳ����ն˵��������ö�
   "|                  USART �첽ͨ������                   |\n\r"
   "|                   www.scdbit.com                      |\n\r"
   "|                   �ɶ���һ�ؿƼ�                      |\n\r"
   "|                      2012.5.15                        |\n\r"
   "|                ����֧��QQ��406795003                  |\n\r"
   "+-------------------------------------------------------+\n\r";

//��������
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
**������Ϣ ��void CANInit(void)                         // WAN.CG // 2011.4.14
**�������� ��CAN��ʼ������
**������� ����
**������� ����
**������ʾ ��
********************************************************************************************************/
void CANInit(void)
{
	GPIO_InitTypeDef       GPIO_InitStructure;
    CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;	
	//NVIC_InitTypeDef       NVIC_InitStructure;

	//PB8,PB9����ΪCAN BUS
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
	//GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);         //�˿���ӳ�䵽PB8,PB9
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; //??????
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM=DISABLE;		//ʱ�䴥��
	CAN_InitStructure.CAN_ABOM=ENABLE;		//�Զ����߹���
	CAN_InitStructure.CAN_AWUM=ENABLE;		//�Զ�����
	CAN_InitStructure.CAN_NART=ENABLE;		//ENABLE:�����Զ��ش� DISABLE:�ش�
	CAN_InitStructure.CAN_RFLM=DISABLE;
	CAN_InitStructure.CAN_TXFP=DISABLE;
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;		//��������ģʽ
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;			//1-4
	CAN_InitStructure.CAN_BS1=CAN_BS1_5tq;			//1-16
	CAN_InitStructure.CAN_BS2=CAN_BS2_2tq;			//1-8
	CAN_InitStructure.CAN_Prescaler=9;				//������Ϊ 36/(9*(1+12+7))=200k
	CAN_Init(CAN1,&CAN_InitStructure);
	
	/* CAN ���������� */
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
	
	/* ����FMP0�ж�*/ 
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);


}
/********************************************************************************************************
**������Ϣ ��void USB_LP_CAN1_RX0_IRQHandler(void)                    // WAN.CG // 2011.4.14 
**�������� ��his function handles USB Low Priority or CAN RX0 interrupts
**������� ����
**������� ����
**������ʾ ��
********************************************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{  
	CanRxMsg RxMessage;
	RxMessage.StdId=0;
	CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);

		
	//if(RxMessage.StdId==0x4C9)//����ID
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
**������Ϣ ��int main (void)                           // WAN.CG // 2011.4.15 
**�������� ��������ARMLED����,CAN BUS��ʼ�������ݣ���CAN�ж��н��գ������Խ��յ�������ΪLED����˸Ƶ�ʵ�����
**������� ��
**������� ��
**������ʾ ��
********************************************************************************************************/
int main(void)
{
		//char dat=0x01;
	SystemInit();  
	CANInit();	     //ϵͳ��ʼ��,��ʼ��ϵͳʱ��
	Init_NVIC();
	Init_Usart();
	GPIOInit();   	//GPIO��ʼ��������ʵ���õ��Ķ�Ҫ��ʼ��	 	
	Usart_Configuration(115200);	//�������� ���ò�����Ϊ115200
	printf(menu);					//����ַ���
  ADC_Configuration();
	DMA_Configuration();

	while(1)
	{  
		//GPIO_ResetBits(GPIOC, GPIO_Pin_13);	//PC13����͵�ƽ������LED3
		//Delay(ADCData);
	//	printf(dat);	
		//GPIO_SetBits(GPIOC, GPIO_Pin_13);	//PC13����ߵ�ƽ��Ϩ��LED3
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
**������Ϣ ��void Delay(u16 dly)                          // WAN.CG // 2010.12.8 
**�������� ����ʱ����������Ϊ0.01����
**������� ��u32 dly����ʱʱ��
**������� ����
**������ʾ ����
********************************************************************************************************/
void Delay(u32 dly)
{
	u16  i;	
	for ( ; dly>0; dly--)
		for (i=0; i<1000; i++);
}


void Init_NVIC(void)
{ 	
  	NVIC_InitTypeDef NVIC_InitStructure;			//����һ��NVIC������ṹ�����

	#ifdef  VECT_TAB_RAM  							//���������ַѡ��

	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);  	//��0x20000000��ַ��Ϊ���������ַ(RAM)
	#else  

	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); //��0x08000000��ַ��Ϊ���������ַ(FLASH)  
	#endif
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel=USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
   
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�����ж��� Ϊ2 

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;			//���ô���1Ϊ�ж�Դ
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	//����ռ�����ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  	//���ø����ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  	//ʹ�ܴ���1�ж�
	NVIC_Init(&NVIC_InitStructure);							  	//���ݲ�����ʼ���жϼĴ���
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
  // ���ڼ����Ϣ�����Ƿ�����
  while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF))
  {
   i++;
  }
  i = 0;
  // ��鷵�صĹҺŵ���Ϣ��Ŀ
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
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;					//����ģʽ  CR1
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;						    //ɨ��ģʽʹ��
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;					//����ת��ģʽʹ��
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//����ת�����ⲿ�¼�--��  CR2
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;				//ת����������Ҷ���
  ADC_InitStructure.ADC_NbrOfChannel = 1;								//ת����ͨ����Ϊ1
  ADC_Init(ADC1, &ADC_InitStructure);

  //ADC_TempSensorVrefintCmd(ENABLE);
  /* ADC1 regular channel14 configuration */ 
  //ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );
	//ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5 );

	/* Enable ADC1 DMA */															  
  ADC_DMACmd(ADC1, DISABLE);		                          						  //����ADC1����DMA����
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);														  //ʹ��ADC1

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);													  //����ADC1��λУ׼�Ĵ��� 
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));									  //���У׼�Ĵ����Ƿ�λ��� 

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);													  //����ADC1 У׼
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));									      //���У׼�Ƿ���� 
     
  /* Start ADC1 Software Conversion */ 
  //ADC_SoftwareStartConvCmd(ADC1, ENABLE);										  //�����������ADC1ת��

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

