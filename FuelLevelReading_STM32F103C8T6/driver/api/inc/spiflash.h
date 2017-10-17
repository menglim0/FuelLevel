/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
**
**                                 http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			spiFLASH.h
** modified Date:  		2010-05-19
** Last Version:		V1.00
** Descriptions:		spi flash API
**	
*********************************************************************************************************/

#ifndef __SPI_FLASH_H_ 
#define __SPI_FLASH_H_

#include "api.h"

#define ADDR_MAX		0x1FFFFF	// 定义芯片内部最大地址 
#define	SEC_MAX     	511			// 定义最大扇区号
#define SEC_SIZE		0x1000      // 扇区大小

#define SEC_ASCII		16      	// ASCII字库存储的扇区号
#define SEC_FONT16		17      	// 国标GB2312，16*16字库存储的起始扇区号
#define SEC_FONT24		70      	// 国标GB2312, 24*24字库存储的起始扇区号

#define SEC_TOUCH		15      	// 触摸屏参数存储的扇区号
#define SEC_PIC 		190      	// 演示图片存储的起始扇区号

typedef enum ERTYPE{Sec1,Sec8,Sec16,Chip} ErType;  
typedef enum IDTYPE{Manu_ID,Dev_ID,Jedec_ID} idtype;
void  SPI1Init(void);
int8u spiFLASH_RD(int32u Dst,int8u* RcvBufPt, int32u NByte);
int8u spiFLASH_RdID(idtype IDType, int32u* RcvbufPt);
int8u spiFLASH_WR(int32u Dst,int8u* SndbufPt,int32u NByte);
int8u spiFLASH_Erase(int32u sec1, int32u sec2);	 

#endif
