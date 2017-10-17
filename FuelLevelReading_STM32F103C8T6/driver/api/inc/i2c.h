/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
**
**                                 http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			i2c.c
** modified Date:  		2011-1-7
** Last Version:		V1.00
** Descriptions:		
**	
*********************************************************************************************************/
#ifndef __I2C_H__
#define __I2C_H__

#define EEPROM_ADDR		0xA0	//器件识别地址
#define I2C_PAGESIZE    16      //24C08每页16字节

void I2C1Init(int32u I2CSpeed);
void I2C1_ReadNByte(int8u addr ,int8u* Buffer, int16u num);
void I2C1_WriteNByte(int8u addr,int8u* Buffer, int16u num);

#endif
