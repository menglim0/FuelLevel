#ifndef __USART_H
#define __USART_H

//头文件包含
#include "stdio.h"	 
#include <string.h>

//函数声明
void USART2_Send_Byte(u16 dat);
uint8_t USART2_Receive_Byte(void);
void Init_Usart(void);
void Usart_Configuration(uint32_t BaudRate); 

#endif
