/****************************************Copyright (c)****************************************************
** 
**                                      ���ڸ�����Ƽ�
**
**                                 http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			API.h
** modified Date:  		2011-03-25
** Last Version:		V1.00
** Descriptions:		API
**	
*********************************************************************************************************/

#ifndef __API_H_ 
#define __API_H_

/*********************************************************************************************************
  ͨ�ú궨��
*********************************************************************************************************/
#define OK                     1 
#define ERROR                  0    
  
#define TRUE                   1
#define FALSE                  0  	                                                            

#ifndef NULL
#define NULL                   0ul
#endif 	
/*********************************************************************************************************
  �����������Ͷ���
*********************************************************************************************************/
typedef unsigned char  int8u;                    /* �޷���8λ���ͱ���  */
typedef signed   char  int8s;                    /* �з���8λ���ͱ���  */
typedef unsigned short int16u;                   /* �޷���16λ���ͱ��� */
typedef signed   short int16s;                   /* �з���16λ���ͱ��� */
typedef unsigned int   int32u;                   /* �޷���32λ���ͱ��� */
typedef signed   int   int32s;                   /* �з���32λ���ͱ��� */
typedef float          fp32;                     /*  �����ȸ�������32λ���ȣ�  */
typedef double         fp64;                     /*  ˫���ȸ�������64λ���ȣ�  */
typedef unsigned int   OS_CPU_SR;                /* Define size of CPU status register (PSR = 32 bits) */


#define  OS_CRITICAL_METHOD   3

#if      OS_CRITICAL_METHOD == 3 
#define  OS_ENTER_CRITICAL()  {cpu_sr = OS_CPU_SR_Save();}
#define  OS_EXIT_CRITICAL()   {OS_CPU_SR_Restore(cpu_sr);}
#endif
/*********************************************************************************************************
*                                              PROTOTYPES
*********************************************************************************************************/
#if OS_CRITICAL_METHOD == 3                       /* See OS_CPU_A.ASM                                  */
OS_CPU_SR  OS_CPU_SR_Save(void);
void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
#endif

/*********************************************************************************************************
�õ���API������Ҫ������ȥ  
*********************************************************************************************************/
#include "gpio.h"
//#include "time.h"
//#include "queue.h"
//#include "uart.h"
//#include "i2c.h"  
//#include "spiflash.h"



#endif	 //__API_H_
