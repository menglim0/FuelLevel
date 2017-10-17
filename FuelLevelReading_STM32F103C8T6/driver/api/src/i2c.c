/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
**
**                                 http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			i2c.c
** modified Date:  		2010-1-7
** Last Version:		V1.00
** Descriptions:		
**	
*********************************************************************************************************/
#include "stm32f10x.h"
#include "stm32lib.h"
#include "api.h"
/********************************************************************************************************
**函数信息 ：void I2CInit(void)                         // WAN.CG // 2010.12.18
**功能描述 ：I2CInit初始化函数
**输入参数 ：I2CSpeed：选择通信速度
**输出参数 ：无
**调用提示 ：
********************************************************************************************************/
void I2C1Init(u32 I2CSpeed)
{
	I2C_InitTypeDef   I2C_InitStructure; 
	GPIO_InitTypeDef  GPIO_InitStructure;

	// PB6,PB7 配置为I2C
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);	
	
    I2C_DeInit(I2C1);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x30;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = I2CSpeed; //通信速度
	I2C_Init(I2C1, &I2C_InitStructure);
    
	I2C_Cmd(I2C1, ENABLE);	                     //使能I2C	
	
	I2C_AcknowledgeConfig(I2C1, ENABLE);         //允许单字节应答模式 
}
/********************************************************************************************************
**函数信息 ：void I2C1_ReadNByte(u8 addr ,u8* Buffer,u16 num)            // WAN.CG // 2010.12.18 
**功能描述 ：读取EEPROM多个字节 (24C08)  
**输入参数 ：addr:读取数据的起始地址
**           Buffer：读取数据的存储地址
**           num：读取数据的个数
**输出参数 ：
**调用提示 ：
********************************************************************************************************/
void I2C1_ReadNByte(u8 addr ,u8* Buffer,u16 num)
{
    if(num==0)
		return;
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); 		
	
	I2C_AcknowledgeConfig(I2C1, ENABLE);	//允许1字节1应答模式 	
    I2C_GenerateSTART(I2C1, ENABLE);	    // 发送起始位

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//EV5,主模式	 
    
    I2C_Send7bitAddress(I2C1,  EEPROM_ADDR, I2C_Direction_Transmitter);	       //发送器件地址(写)
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); //数据发送完成

	I2C_SendData(I2C1, addr);											 	   //发送写入地址
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));          //数据发送完成 		
	
	I2C_GenerateSTART(I2C1, ENABLE);										   //起始位
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));						  	
	
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Receiver);		       //器件读
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));	 
	
    while (num)
    {
		if(num==1)
		{
     		I2C_AcknowledgeConfig(I2C1, DISABLE);	//最后一位后要关闭应答的
    		I2C_GenerateSTOP(I2C1, ENABLE);			//发送停止位
		}
	    
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)); //EV7
	    *Buffer = I2C_ReceiveData(I2C1);
	    Buffer++; 	    
	    num--;
    }  	
	I2C_AcknowledgeConfig(I2C1, ENABLE);            //再次允许应答模式
}

/********************************************************************************************************
**函数信息 ：void I2C_BusyTest(void)                // WAN.CG // 2010.12.18 
**功能描述 ：判断是否操作完成  
**输入参数 ：
**输出参数 ：
**调用提示 ：
********************************************************************************************************/
void I2C1_BusyTest(void)      
{
	vu16 SR1_Tmp;
  	do
  	{	
    	I2C_GenerateSTART(I2C1, ENABLE);	    //起始位     
    	SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);	                //读SR1 
		I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);	//器件地址(写)	
  	}
 	 while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));

  	I2C_ClearFlag(I2C1, I2C_FLAG_AF);	       
  	I2C_GenerateSTOP(I2C1, ENABLE);            //停止位
}  

/********************************************************************************************************
**函数信息 ：void I2C_WriteByte(u8 addr,u8 dat)              // WAN.CG // 2010.12.18 
**功能描述 ：读取EEPROM多个字节 (24C08)  
**输入参数 ：addr:数据写入器件的地址
**           dat：要写入的数据
**输出参数 ：
**调用提示 ：
********************************************************************************************************/
void I2C1_WriteByte(u8 addr,u8 dat)
{
  	I2C_GenerateSTART(I2C1, ENABLE);		  //起始位 
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));  
  
  	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);		 //发送器件地址(写)
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));   
  	
  	I2C_SendData(I2C1, addr);				  //发送地址
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); 
	
  	I2C_SendData(I2C1, dat); 			      //写一个字节
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); 	
  	
  	I2C_GenerateSTOP(I2C1, ENABLE);			  //停止位   
  	I2C1_BusyTest();							  //判断是否操作完成
}

/********************************************************************************************************
**函数信息 ：void I2C_WritePage(u8 addr,u8* Buffer, u8 num)         // WAN.CG // 2010.12.18 
**功能描述 ：写入一页数据，如24C08每页16字节 
**输入参数 ：addr:   写入数据的起始地址
**           Buffer：写入数据的存储地址
**           num：   写入数据的个数
**输出参数 ：
**调用提示 ：
********************************************************************************************************/
void I2C1_WritePage(u8 addr,u8* Buffer, u8 num)
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

	I2C_GenerateSTART(I2C1, ENABLE);	 //起始位
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));	 

	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter); 	//器件地址(写)
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  	

	I2C_SendData(I2C1, addr);				//写地址值
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));  

	while(num--)  
	{
	 	I2C_SendData(I2C1, *Buffer); 
	  	Buffer++; 
	  	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}	  
	I2C_GenerateSTOP(I2C1, ENABLE);	       //停止位
}

/********************************************************************************************************
**函数信息 ：void I2C1_WriteNByte(u8 addr,u8* Buffer, u16 num)            // WAN.CG // 2010.12.18 
**功能描述 ：向eeprom写入任意字节的数据 
**输入参数 ：addr:   写入数据的起始地址
**           Buffer：写入数据的存储地址
**           num：   写入数据的个数
**输出参数 ：
**调用提示 ：
********************************************************************************************************/
void I2C1_WriteNByte(u8 addr,u8* Buffer, u16 num)
{
	u8 temp;
			
	temp=addr % I2C_PAGESIZE;	  //先把页不对齐的部分写入
	if(temp)
	{
		temp=I2C_PAGESIZE-temp;
		I2C1_WritePage(addr, Buffer, temp);
		num-=temp;
		addr+=temp;
		Buffer+=temp;
		I2C1_BusyTest();	         //写完成
	}	 
	while(num)				 	 //从页对齐开始写
	{
		if(num>=I2C_PAGESIZE)
		{
			I2C1_WritePage(addr, Buffer, I2C_PAGESIZE);
			num-=I2C_PAGESIZE;
			addr+=I2C_PAGESIZE;
			Buffer+=I2C_PAGESIZE;
			I2C1_BusyTest();	         //写完成
		}
		else
		{
			I2C1_WritePage(addr, Buffer, num);
			num=0;
			I2C1_BusyTest();	         //写完成
		}
	}
}

