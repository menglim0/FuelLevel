/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
**
**                                   http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			uart.h
** modified Date:  		2011-4-16
** Last Version:		V1.00
** Descriptions:		
**	                    
*********************************************************************************************************/
#ifndef __UART_H__
#define __UART_H__

#define UART1_SEND_QUEUE_LENGTH      128     /* 给UART1发送数据队列分配的空间大小 */
#define UART1_RECV_QUEUE_LENGTH      128     /* 给UART1接收数据队列分配的空间大小 */

extern int8u   UART1_TXEN_flag;	        //发送中断标志位关闭

extern int32s  UART1SendBuf[UART1_SEND_QUEUE_LENGTH/sizeof(int32s)];
extern int32s  UART1RecvBuf[UART1_RECV_QUEUE_LENGTH/sizeof(int32s)];

void    UART1Init(int32u bps);
void    QueueWriteStr(void *buf, char *Data);	

#endif
