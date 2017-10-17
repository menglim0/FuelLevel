/****************************************Copyright (c)****************************************************
** 
**                                      ���ڸ�����Ƽ�
**
**                                 http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			HardwareInit.c
** modified Date:  		2010-12-12
** Last Version:		V1.00
** Descriptions:		Ӳ����ʼ������
**	
*********************************************************************************************************/
#include "stm32f10x.h"
#include "stm32lib.h"
#include "api.h"

int16u  Buzzer_Time;	//����������ʱ�����ã�ȫ�ֱ���
int32u  SysTick_Dly;	//��ʱʱ�����ã�ȫ�ֱ��� 

/********************************************************************************************************
**������Ϣ ��void TIME2_NVIC_Init(void)                         // WAN.CG // 2010.12.12 
**�������� ���ж����ó�ʼ������
**������� ����
**������� ����
**������ʾ ��
********************************************************************************************************/
void TIME2_NVIC_Init(void)
{
	NVIC_InitTypeDef  NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}
/********************************************************************************************************
**������Ϣ ��void TIM2Init(void)                         // WAN.CG // 2010.12.12
**�������� ��TIM2Init��ʼ������������Ϊ50�����ж�һ��
**������� ����
**������� ����
**������ʾ ��RCC_APB1PeriphClockCmd����
********************************************************************************************************/
void TIM2Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  		//����TIM2ʱ��
    TIM_DeInit(TIM2);						 			  		//��λTIM2��������������
	TIM_TimeBaseStructure.TIM_Period=500;                		//����ֵ���������ڸ��������ж�
	TIM_TimeBaseStructure.TIM_Prescaler= (7200 - 1);     		//��Ƶ��,��ÿ72M/7200����һ��
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		//������Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//���ϼ���
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);                 		//�������жϱ�־
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2, ENABLE);                                		//������ʱ��

    TIME2_NVIC_Init();                                          //����TIME�ж�
}
/********************************************************************************************************
**������Ϣ ��void TIM2_IRQHandler(void)                         // WAN.CG // 2010.12.12 
**�������� ��TIM2�жϷ�����������Ϊ50�����ж�һ��
**������� ����
**������� ����
**������ʾ ��
********************************************************************************************************/
void TIM2_IRQHandler(void)
{
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET )
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);	  //����жϱ�־

	if(GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_2)==Bit_SET)//�ж�PD2�Ƿ�Ϊ�ߵ�ƽ 
		GPIO_ResetBits(GPIOD, GPIO_Pin_2);	              //PD2����͵�ƽ������ARMLED
	else
		GPIO_SetBits(GPIOD, GPIO_Pin_2);	              //PD2����ߵ�ƽ��Ϩ��ARMLED

	if(Buzzer_Time)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_5);	//PB5����ߵ�ƽ������������
		Buzzer_Time--;
	}
	else 
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);	//PB5����͵�ƽ��������������

	if((!UART1_TXEN_flag)&&(QueueNData(UART1SendBuf)))//����������жϹرգ�ͬʱ�����������ݣ���UART1�����ж�
		{  	
			UART1_TXEN_flag = 1;
			USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	
		}
		
}
/********************************************************************************************************
**������Ϣ ��void SysTick_Handler(void)                         // WAN.CG // 2011.4.18 
**�������� ��SysTick�жϷ�����,����Ϊÿ1�����ж�һ�Σ���Ҫ������ʱ����ϵͳʱ����ϵͳʱ��
**������� ����
**������� ����
**������ʾ ��
********************************************************************************************************/
void SysTick_Handler(void)
{
	if(SysTick_Dly)
		SysTick_Dly--;
}
/********************************************************************************************************
**������Ϣ ��void SysTickInit(void)                         // WAN.CG // 2011.4.18 
**�������� ��SysTickϵͳʱ������,ÿ1�����ж�һ��,72M����ʱ��
**������� ����
**������� ����
**������ʾ ��
********************************************************************************************************/
void SysTickInit(void)  
{ 
	RCC_ClocksTypeDef RCC_ClockFreq;

	RCC_GetClocksFreq(&RCC_ClockFreq); 	  //��ȡRCC����Ϣ
	
	if (SysTick_Config(72000000 / 1000))  //SYSTICK��Ƶ--1ms��ϵͳʱ���ж�
  	{  
    	while (1);
  	}
}
