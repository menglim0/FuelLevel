/****************************************Copyright (c)****************************************************
** 
**                                      ���ڸ�����Ƽ�
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

#define UART1_SEND_QUEUE_LENGTH      128     /* ��UART1�������ݶ��з���Ŀռ��С */
#define UART1_RECV_QUEUE_LENGTH      128     /* ��UART1�������ݶ��з���Ŀռ��С */

extern int8u   UART1_TXEN_flag;	        //�����жϱ�־λ�ر�

extern int32s  UART1SendBuf[UART1_SEND_QUEUE_LENGTH/sizeof(int32s)];
extern int32s  UART1RecvBuf[UART1_RECV_QUEUE_LENGTH/sizeof(int32s)];

void    UART1Init(int32u bps);
void    QueueWriteStr(void *buf, char *Data);	

#endif
