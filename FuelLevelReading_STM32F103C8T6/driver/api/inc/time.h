/****************************************Copyright (c)****************************************************
** 
**                                      ���ڸ�����Ƽ�
**
**                                   http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			time.h
** modified Date:  		2010-12-12
** Last Version:		V1.00
** Descriptions:		time��������ͷ�ļ�
**	
*********************************************************************************************************/

#ifndef __time__h
#define __time__h

extern  int16u  Buzzer_Time;	           //����������ʱ��
extern  int32u  SysTick_Dly;


void  TIM2Init(void);
void  SysTickInit(void);

#endif
