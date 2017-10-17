/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
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
  通用宏定义
*********************************************************************************************************/
#define OK                     1 
#define ERROR                  0    
  
#define TRUE                   1
#define FALSE                  0  	                                                            

#ifndef NULL
#define NULL                   0ul
#endif 	
/*********************************************************************************************************
  常用数据类型定义
*********************************************************************************************************/
typedef unsigned char  int8u;                    /* 无符号8位整型变量  */
typedef signed   char  int8s;                    /* 有符号8位整型变量  */
typedef unsigned short int16u;                   /* 无符号16位整型变量 */
typedef signed   short int16s;                   /* 有符号16位整型变量 */
typedef unsigned int   int32u;                   /* 无符号32位整型变量 */
typedef signed   int   int32s;                   /* 有符号32位整型变量 */
typedef float          fp32;                     /*  单精度浮点数（32位长度）  */
typedef double         fp64;                     /*  双精度浮点数（64位长度）  */
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
用到的API驱动，要包含进去  
*********************************************************************************************************/
#include "gpio.h"
//#include "time.h"
//#include "queue.h"
//#include "uart.h"
//#include "i2c.h"  
//#include "spiflash.h"



#endif	 //__API_H_
