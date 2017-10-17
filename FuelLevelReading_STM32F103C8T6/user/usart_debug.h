#ifndef __usart_debug_H
#define    __usart_debug_H
 
#include "stm32f10x.h"
#include <stdio.h>
 
void usart_debug_config(void);
int fputc(int ch, FILE *f);
 
 
#endif // __USART1_H
