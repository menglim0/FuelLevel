/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
**
**                                   http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			HardwareInit.c
** modified Date:  		2011-4-15
** Last Version:		V1.00
** Descriptions:		GPIO驱动函数
**	
*********************************************************************************************************/
#include "stm32f10x.h"
#include "stm32lib.h"

/********************************************************************************************************
**函数信息 ：void GPIOInit(void)                         // WAN.CG // 2010.12.8 
**功能描述 ：GPIO初始化函数，初始化实验用到的所有GPIO口
**输入参数 ：无
**输出参数 ：无
**调用提示 ：RCC_APB2PeriphClockCmd（）；GPIO_Init（）
********************************************************************************************************/
void GPIOInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //开启GPIOA端口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //开启GPIOB端口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //开启GPIOC端口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);  //开启GPIOD端口

	//PB5配置为输出;BUZZER
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;			   //选择第端口5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	   //2M时钟速度
	GPIO_Init(GPIOB, &GPIO_InitStructure);				   //GPIO配置函数

	//PC8-PC12配置为输入;KEY1-KEY5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	   //开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //50M时钟速度
	GPIO_Init(GPIOA, &GPIO_InitStructure);			       //GPIO配置函数  

	//PC13配置为输出;LED3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;			   //选择第13端口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	   //开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //50M时钟速度
	GPIO_Init(GPIOC, &GPIO_InitStructure);				   //GPIO配置函数

	//PA5,PA6,PA7配置为SPI功能
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
   // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   // GPIO_Init(GPIOA, &GPIO_InitStructure);

    //PA4配置为SPI片选功能
	//GPIO_SetBits(GPIOA, GPIO_Pin_4);                       //预置为高
  //  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;			   //选择第端口3
   // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //50M时钟速度
   // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //推挽输出
    //GPIO_Init(GPIOA, &GPIO_InitStructure);				   //GPIO配置函数

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
	//GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);         //端口重映射到PD0,PD1

	// PB6,PB7 配置为I2C
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}



