/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
**
**                                 http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			HardwareInit.h
** modified Date:  		2010-12-12
** Last Version:		V1.00
** Descriptions:		硬件初始化头文件
**	
*********************************************************************************************************/
#ifndef _gpio_h_
#define _gpio_h_


#define KEY1 (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8))
#define KEY2 (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9))
#define KEY3 (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_10))
#define KEY4 (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11))
#define KEY5 (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_12))

void GPIOInit(void); 

#endif
