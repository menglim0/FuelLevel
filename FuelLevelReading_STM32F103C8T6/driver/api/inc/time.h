/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
**
**                                   http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			time.h
** modified Date:  		2010-12-12
** Last Version:		V1.00
** Descriptions:		time驱动程序头文件
**	
*********************************************************************************************************/

#ifndef __time__h
#define __time__h

extern  int16u  Buzzer_Time;	           //蜂鸣器鸣响时间
extern  int32u  SysTick_Dly;


void  TIM2Init(void);
void  SysTickInit(void);

#endif
