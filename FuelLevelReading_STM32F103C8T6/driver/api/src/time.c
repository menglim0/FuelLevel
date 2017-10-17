/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
**
**                                 http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			HardwareInit.c
** modified Date:  		2010-12-12
** Last Version:		V1.00
** Descriptions:		硬件初始化部分
**	
*********************************************************************************************************/
#include "stm32f10x.h"
#include "stm32lib.h"
#include "api.h"

int16u  Buzzer_Time;	//蜂鸣器鸣响时间设置，全局变量
int32u  SysTick_Dly;	//延时时间设置，全局变量 

/********************************************************************************************************
**函数信息 ：void TIME2_NVIC_Init(void)                         // WAN.CG // 2010.12.12 
**功能描述 ：中断配置初始化函数
**输入参数 ：无
**输出参数 ：无
**调用提示 ：
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
**函数信息 ：void TIM2Init(void)                         // WAN.CG // 2010.12.12
**功能描述 ：TIM2Init初始化函数，设置为50毫秒中断一次
**输入参数 ：无
**输出参数 ：无
**调用提示 ：RCC_APB1PeriphClockCmd（）
********************************************************************************************************/
void TIM2Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  		//开启TIM2时钟
    TIM_DeInit(TIM2);						 			  		//复位TIM2，可以设置数据
	TIM_TimeBaseStructure.TIM_Period=500;                		//计数值，计数等于该数则发生中断
	TIM_TimeBaseStructure.TIM_Prescaler= (7200 - 1);     		//分频数,即每72M/7200计数一次
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		//采样分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);                 		//清除溢出中断标志
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM2, ENABLE);                                		//开启定时器

    TIME2_NVIC_Init();                                          //配置TIME中断
}
/********************************************************************************************************
**函数信息 ：void TIM2_IRQHandler(void)                         // WAN.CG // 2010.12.12 
**功能描述 ：TIM2中断服务函数，设置为50毫秒中断一次
**输入参数 ：无
**输出参数 ：无
**调用提示 ：
********************************************************************************************************/
void TIM2_IRQHandler(void)
{
	if ( TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET )
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);	  //清除中断标志

	if(GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_2)==Bit_SET)//判断PD2是否为高电平 
		GPIO_ResetBits(GPIOD, GPIO_Pin_2);	              //PD2输出低电平，点亮ARMLED
	else
		GPIO_SetBits(GPIOD, GPIO_Pin_2);	              //PD2输出高电平，熄灭ARMLED

	if(Buzzer_Time)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_5);	//PB5输出高电平，蜂鸣器鸣响
		Buzzer_Time--;
	}
	else 
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);	//PB5输出低电平，蜂鸣器不鸣响

	if((!UART1_TXEN_flag)&&(QueueNData(UART1SendBuf)))//如果，发送中断关闭，同时发送区有数据，开UART1发送中断
		{  	
			UART1_TXEN_flag = 1;
			USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	
		}
		
}
/********************************************************************************************************
**函数信息 ：void SysTick_Handler(void)                         // WAN.CG // 2011.4.18 
**功能描述 ：SysTick中断服务函数,设置为每1毫秒中断一次，主要用于延时，有系统时用作系统时钟
**输入参数 ：无
**输出参数 ：无
**调用提示 ：
********************************************************************************************************/
void SysTick_Handler(void)
{
	if(SysTick_Dly)
		SysTick_Dly--;
}
/********************************************************************************************************
**函数信息 ：void SysTickInit(void)                         // WAN.CG // 2011.4.18 
**功能描述 ：SysTick系统时钟设置,每1毫秒中断一次,72M的主时钟
**输入参数 ：无
**输出参数 ：无
**调用提示 ：
********************************************************************************************************/
void SysTickInit(void)  
{ 
	RCC_ClocksTypeDef RCC_ClockFreq;

	RCC_GetClocksFreq(&RCC_ClockFreq); 	  //获取RCC的信息
	
	if (SysTick_Config(72000000 / 1000))  //SYSTICK分频--1ms的系统时钟中断
  	{  
    	while (1);
  	}
}
