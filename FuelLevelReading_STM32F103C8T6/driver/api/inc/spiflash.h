/****************************************Copyright (c)****************************************************
** 
**                                      ���ڸ�����Ƽ�
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

#define ADDR_MAX		0x1FFFFF	// ����оƬ�ڲ�����ַ 
#define	SEC_MAX     	511			// �������������
#define SEC_SIZE		0x1000      // ������С

#define SEC_ASCII		16      	// ASCII�ֿ�洢��������
#define SEC_FONT16		17      	// ����GB2312��16*16�ֿ�洢����ʼ������
#define SEC_FONT24		70      	// ����GB2312, 24*24�ֿ�洢����ʼ������

#define SEC_TOUCH		15      	// �����������洢��������
#define SEC_PIC 		190      	// ��ʾͼƬ�洢����ʼ������

typedef enum ERTYPE{Sec1,Sec8,Sec16,Chip} ErType;  
typedef enum IDTYPE{Manu_ID,Dev_ID,Jedec_ID} idtype;
void  SPI1Init(void);
int8u spiFLASH_RD(int32u Dst,int8u* RcvBufPt, int32u NByte);
int8u spiFLASH_RdID(idtype IDType, int32u* RcvbufPt);
int8u spiFLASH_WR(int32u Dst,int8u* SndbufPt,int32u NByte);
int8u spiFLASH_Erase(int32u sec1, int32u sec2);	 

#endif
