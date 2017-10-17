/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
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

int8u   UART1_TXEN_flag = 0;	   //发送中断标志位关闭

/********************************************************************************************************
**函数信息 ：Uart1WriteFull                       // WAN.CG // 2014.4.16
**功能描述 ：UART1数据队列写满处理程序 
**输入参数 ：Buf:指向数据队列
**           Data:将要入队的数据
**输出参数 ：TRUE  :成功
**           FALSE:失败
**调用提示 ：
********************************************************************************************************/
int8u Uart1WriteFull(DataQueue *Buf, QUEUE_DATA_TYPE Data)
{
    QUEUE_DATA_TYPE temp;
    
    QueueRead(&temp,(void *)Buf);                   /*数据队列满，将最先接收的数据覆盖*/
    
    return QueueWrite((void *)Buf, Data);           /* 数据重新入队 */
}
/*********************************************************************************************************
**函数信息 ：QueueWriteStr	                 // WAN.CG // 2014.4.16
** 功能描述: 向指定的数据队列写入字符串
** 输  　入: Data:要写入的数据的指针
** 输　  出: 无
** 全局变量: 无
** 调用模块: QueueWrite
********************************************************************************************************/
void QueueWriteStr(void *buf, char *Data)
{  
   while(*Data!='\0')	
   {	
      	 QueueWrite(buf, *Data);      /* 数据入队 */		
		 Data++;
   }
   
}

/********************************************************************************************************
**函数信息 ：void UART1Init(void)                         // WAN.CG // 2011.4.16
**功能描述 ：UART1初始化函数
**输入参数 ：u32 bps:串口波特率
**输出参数 ：无
**调用提示 ：
********************************************************************************************************/
void UART1Init(int32u bps)
{
    GPIO_InitTypeDef         GPIO_InitStructure;
	NVIC_InitTypeDef         NVIC_InitStructure;
    USART_InitTypeDef        USART_InitStructure;
	USART_ClockInitTypeDef   USART_ClockInitStructure;
	
	//使能串口1，PA和AFIO总线
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1, ENABLE);

	//PA9,PA10配置为UART1
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		   //推挽输出,UART1-TX
	GPIO_Init(GPIOA , &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //浮空输入,UART1-RX
	GPIO_Init(GPIOA, &GPIO_InitStructure);	 

    USART_InitStructure.USART_BaudRate = bps;			 //设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	   //串口1配置初始化
	
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;	  
	USART_ClockInit(USART1, &USART_ClockInitStructure);		  //串口1时钟初始化

    USART_Init(USART1, &USART_InitStructure);
  
    USART_Cmd(USART1, ENABLE);							   //使能串口1

	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);		   //接收中断使能
	//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);	 

	//UART1中断配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//创建UART1发送数据队列
	QueueCreate((void *)UART1SendBuf, sizeof(UART1SendBuf), NULL, (int8u (*)())Uart1WriteFull);
 	//创建UART1接收数据队列
    QueueCreate((void *)UART1RecvBuf, sizeof(UART1RecvBuf), NULL, (int8u (*)())Uart1WriteFull);
} 


/********************************************************************************************************
**函数信息 ：void USART1_IRQHandler(void)                         // WAN.CG // 2011.4.16 
**功能描述 ：USART1中断服务函数
**输入参数 ：无
**输出参数 ：无
**调用提示 ：
********************************************************************************************************/
void USART1_IRQHandler(void)
{
	int8u queue_read_buf;

	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)   	//接收中断
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		QueueWrite((void *)UART1RecvBuf, USART_ReceiveData(USART1));	//串口1数据进入接收数据队列
	}
	
	if(USART_GetITStatus(USART1,USART_IT_TXE)==SET)		//发送空中断
	{  
		if(QueueRead(&queue_read_buf, UART1SendBuf) == QUEUE_OK)	     //判断串口1发送数据队列是否有有效数据
		{
			USART_SendData(USART1, queue_read_buf);
		} 
		else
		{
			USART_ITConfig(USART1,USART_IT_TXE,DISABLE);		        //发送队列空，关发送中断
			UART1_TXEN_flag = 0;
		}
	}
	
	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE)==SET)	  	//溢出处理
	{
		USART_ClearFlag(USART1,USART_FLAG_ORE);	            //清除ORE
		USART_ReceiveData(USART1);				            //读DR
	}	
}
