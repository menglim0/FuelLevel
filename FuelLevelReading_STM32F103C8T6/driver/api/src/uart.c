/****************************************Copyright (c)****************************************************
** 
**                                      ���ڸ�����Ƽ�
**
**                                   http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			uart.c
** modified Date:  		2011-4-15
** Last Version:		V1.00
** Descriptions:		
**	                    
*********************************************************************************************************/
#include "stm32f10x.h"
#include "stm32lib.h"
#include "api.h"
#include "uart.h"


int32s  UART1SendBuf[UART1_SEND_QUEUE_LENGTH/sizeof(int32s)];
int32s  UART1RecvBuf[UART1_RECV_QUEUE_LENGTH/sizeof(int32s)];

int8u   UART1_TXEN_flag = 0;	   //�����жϱ�־λ�ر�

/********************************************************************************************************
**������Ϣ ��Uart1WriteFull                       // WAN.CG // 2014.4.16
**�������� ��UART1���ݶ���д��������� 
**������� ��Buf:ָ�����ݶ���
**           Data:��Ҫ��ӵ�����
**������� ��TRUE  :�ɹ�
**           FALSE:ʧ��
**������ʾ ��
********************************************************************************************************/
int8u Uart1WriteFull(DataQueue *Buf, QUEUE_DATA_TYPE Data)
{
    QUEUE_DATA_TYPE temp;
    
    QueueRead(&temp,(void *)Buf);                   /*���ݶ������������Ƚ��յ����ݸ���*/
    
    return QueueWrite((void *)Buf, Data);           /* ����������� */
}
/*********************************************************************************************************
**������Ϣ ��QueueWriteStr	                 // WAN.CG // 2014.4.16
** ��������: ��ָ�������ݶ���д���ַ���
** ��  ����: Data:Ҫд������ݵ�ָ��
** �䡡  ��: ��
** ȫ�ֱ���: ��
** ����ģ��: QueueWrite
********************************************************************************************************/
void QueueWriteStr(void *buf, char *Data)
{  
   while(*Data!='\0')	
   {	
      	 QueueWrite(buf, *Data);      /* ������� */		
		 Data++;
   }
   
}

/********************************************************************************************************
**������Ϣ ��void UART1Init(void)                         // WAN.CG // 2011.4.16
**�������� ��UART1��ʼ������
**������� ��u32 bps:���ڲ�����
**������� ����
**������ʾ ��
********************************************************************************************************/
void UART1Init(int32u bps)
{
    GPIO_InitTypeDef         GPIO_InitStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;
    USART_InitTypeDef        USART_InitStructure;
	USART_ClockInitTypeDef   USART_ClockInitStructure;
	
	//ʹ�ܴ���1��PA��AFIO����
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1, ENABLE);

	//PA9,PA10����ΪUART1
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   //�������,UART1-TX
	GPIO_Init(GPIOA , &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //��������,UART1-RX
	GPIO_Init(GPIOA, &GPIO_InitStructure);	 

    USART_InitStructure.USART_BaudRate = bps;			 //���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	   //����1���ó�ʼ��
	
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;	  
	USART_ClockInit(USART1, &USART_ClockInitStructure);		  //����1ʱ�ӳ�ʼ��

    USART_Init(USART1, &USART_InitStructure);
  
    USART_Cmd(USART1, ENABLE);							   //ʹ�ܴ���1

	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);		   //�����ж�ʹ��
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);	 

	//UART1�ж�����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//����UART1�������ݶ���
	QueueCreate((void *)UART1SendBuf, sizeof(UART1SendBuf), NULL, (int8u (*)())Uart1WriteFull);
 	//����UART1�������ݶ���
    QueueCreate((void *)UART1RecvBuf, sizeof(UART1RecvBuf), NULL, (int8u (*)())Uart1WriteFull);
} 


/********************************************************************************************************
**������Ϣ ��void USART1_IRQHandler(void)                         // WAN.CG // 2011.4.16 
**�������� ��USART1�жϷ�����
**������� ����
**������� ����
**������ʾ ��
********************************************************************************************************/
void USART1_IRQHandler(void)
{
	int8u queue_read_buf;

	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)   	//�����ж�
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		QueueWrite((void *)UART1RecvBuf, USART_ReceiveData(USART1));	//����1���ݽ���������ݶ���
	}
	
	if(USART_GetITStatus(USART1,USART_IT_TXE)==SET)		//���Ϳ��ж�
	{  
		if(QueueRead(&queue_read_buf, UART1SendBuf) == QUEUE_OK)	     //�жϴ���1�������ݶ����Ƿ�����Ч����
		{
			USART_SendData(USART1, queue_read_buf);
		} 
		else
		{
			USART_ITConfig(USART1,USART_IT_TXE,DISABLE);		        //���Ͷ��пգ��ط����ж�
			UART1_TXEN_flag = 0;
		}
	}
	
	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE)==SET)	  	//�������
	{
		USART_ClearFlag(USART1,USART_FLAG_ORE);	            //���ORE
		USART_ReceiveData(USART1);				            //��DR
	}	
}
