/****************************************Copyright (c)****************************************************
** 
**                                      ���ڸ�����Ƽ�
**
**                                   http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			HardwareInit.c
** modified Date:  		2011-4-15
** Last Version:		V1.00
** Descriptions:		GPIO��������
**	
*********************************************************************************************************/
#include "stm32f10x.h"
#include "stm32lib.h"

/********************************************************************************************************
**������Ϣ ��void GPIOInit(void)                         // WAN.CG // 2010.12.8 
**�������� ��GPIO��ʼ����������ʼ��ʵ���õ�������GPIO��
**������� ����
**������� ����
**������ʾ ��RCC_APB2PeriphClockCmd������GPIO_Init����
********************************************************************************************************/
void GPIOInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //����GPIOA�˿�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //����GPIOB�˿�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  //����GPIOC�˿�
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);  //����GPIOD�˿�

	//PB5����Ϊ���;BUZZER
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;			   //ѡ��ڶ˿�5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	   //2Mʱ���ٶ�
	GPIO_Init(GPIOB, &GPIO_InitStructure);				   //GPIO���ú���

	//PC8-PC12����Ϊ����;KEY1-KEY5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	   //��©���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //50Mʱ���ٶ�
	GPIO_Init(GPIOA, &GPIO_InitStructure);			       //GPIO���ú���  

	//PC13����Ϊ���;LED3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;			   //ѡ���13�˿�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	   //��©���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //50Mʱ���ٶ�
	GPIO_Init(GPIOC, &GPIO_InitStructure);				   //GPIO���ú���

	//PA5,PA6,PA7����ΪSPI����
    //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
   // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   // GPIO_Init(GPIOA, &GPIO_InitStructure);

    //PA4����ΪSPIƬѡ����
	//GPIO_SetBits(GPIOA, GPIO_Pin_4);                       //Ԥ��Ϊ��
  //  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;			   //ѡ��ڶ˿�3
   // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //50Mʱ���ٶ�
   // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //�������
    //GPIO_Init(GPIOA, &GPIO_InitStructure);				   //GPIO���ú���

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
	//GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);         //�˿���ӳ�䵽PD0,PD1

	// PB6,PB7 ����ΪI2C
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}



