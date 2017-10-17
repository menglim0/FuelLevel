/****************************************Copyright (c)****************************************************
** 
**                                      ���ڸ�����Ƽ�
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
**������Ϣ ��void I2CInit(void)                         // WAN.CG // 2010.12.18
**�������� ��I2CInit��ʼ������
**������� ��I2CSpeed��ѡ��ͨ���ٶ�
**������� ����
**������ʾ ��
********************************************************************************************************/
void I2C1Init(u32 I2CSpeed)
{
	I2C_InitTypeDef   I2C_InitStructure; 
	GPIO_InitTypeDef  GPIO_InitStructure;

	// PB6,PB7 ����ΪI2C
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
    I2C_InitStructure.I2C_ClockSpeed = I2CSpeed; //ͨ���ٶ�
	I2C_Init(I2C1, &I2C_InitStructure);
    
	I2C_Cmd(I2C1, ENABLE);	                     //ʹ��I2C	
	
	I2C_AcknowledgeConfig(I2C1, ENABLE);         //�����ֽ�Ӧ��ģʽ 
}
/********************************************************************************************************
**������Ϣ ��void I2C1_ReadNByte(u8 addr ,u8* Buffer,u16 num)            // WAN.CG // 2010.12.18 
**�������� ����ȡEEPROM����ֽ� (24C08)  
**������� ��addr:��ȡ���ݵ���ʼ��ַ
**           Buffer����ȡ���ݵĴ洢��ַ
**           num����ȡ���ݵĸ���
**������� ��
**������ʾ ��
********************************************************************************************************/
void I2C1_ReadNByte(u8 addr ,u8* Buffer,u16 num)
{
    if(num==0)
		return;
	
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); 		
	
	I2C_AcknowledgeConfig(I2C1, ENABLE);	//����1�ֽ�1Ӧ��ģʽ 	
    I2C_GenerateSTART(I2C1, ENABLE);	    // ������ʼλ

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));//EV5,��ģʽ	 
    
    I2C_Send7bitAddress(I2C1,  EEPROM_ADDR, I2C_Direction_Transmitter);	       //����������ַ(д)
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); //���ݷ������

	I2C_SendData(I2C1, addr);											 	   //����д���ַ
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));          //���ݷ������ 		
	
	I2C_GenerateSTART(I2C1, ENABLE);										   //��ʼλ
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));						  	
	
	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Receiver);		       //������
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));	 
	
    while (num)
    {
		if(num==1)
		{
     		I2C_AcknowledgeConfig(I2C1, DISABLE);	//���һλ��Ҫ�ر�Ӧ���
    		I2C_GenerateSTOP(I2C1, ENABLE);			//����ֹͣλ
		}
	    
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)); //EV7
	    *Buffer = I2C_ReceiveData(I2C1);
	    Buffer++; 	    
	    num--;
    }  	
	I2C_AcknowledgeConfig(I2C1, ENABLE);            //�ٴ�����Ӧ��ģʽ
}

/********************************************************************************************************
**������Ϣ ��void I2C_BusyTest(void)                // WAN.CG // 2010.12.18 
**�������� ���ж��Ƿ�������  
**������� ��
**������� ��
**������ʾ ��
********************************************************************************************************/
void I2C1_BusyTest(void)      
{
	vu16 SR1_Tmp;
  	do
  	{	
    	I2C_GenerateSTART(I2C1, ENABLE);	    //��ʼλ     
    	SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);	                //��SR1 
		I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);	//������ַ(д)	
  	}
 	 while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));

  	I2C_ClearFlag(I2C1, I2C_FLAG_AF);	       
  	I2C_GenerateSTOP(I2C1, ENABLE);            //ֹͣλ
}  

/********************************************************************************************************
**������Ϣ ��void I2C_WriteByte(u8 addr,u8 dat)              // WAN.CG // 2010.12.18 
**�������� ����ȡEEPROM����ֽ� (24C08)  
**������� ��addr:����д�������ĵ�ַ
**           dat��Ҫд�������
**������� ��
**������ʾ ��
********************************************************************************************************/
void I2C1_WriteByte(u8 addr,u8 dat)
{
  	I2C_GenerateSTART(I2C1, ENABLE);		  //��ʼλ 
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));  
  
  	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);		 //����������ַ(д)
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));   
  	
  	I2C_SendData(I2C1, addr);				  //���͵�ַ
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); 
	
  	I2C_SendData(I2C1, dat); 			      //дһ���ֽ�
  	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); 	
  	
  	I2C_GenerateSTOP(I2C1, ENABLE);			  //ֹͣλ   
  	I2C1_BusyTest();							  //�ж��Ƿ�������
}

/********************************************************************************************************
**������Ϣ ��void I2C_WritePage(u8 addr,u8* Buffer, u8 num)         // WAN.CG // 2010.12.18 
**�������� ��д��һҳ���ݣ���24C08ÿҳ16�ֽ� 
**������� ��addr:   д�����ݵ���ʼ��ַ
**           Buffer��д�����ݵĴ洢��ַ
**           num��   д�����ݵĸ���
**������� ��
**������ʾ ��
********************************************************************************************************/
void I2C1_WritePage(u8 addr,u8* Buffer, u8 num)
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

	I2C_GenerateSTART(I2C1, ENABLE);	 //��ʼλ
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));	 

	I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter); 	//������ַ(д)
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));  	

	I2C_SendData(I2C1, addr);				//д��ֵַ
	while(! I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));  

	while(num--)  
	{
	 	I2C_SendData(I2C1, *Buffer); 
	  	Buffer++; 
	  	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}	  
	I2C_GenerateSTOP(I2C1, ENABLE);	       //ֹͣλ
}

/********************************************************************************************************
**������Ϣ ��void I2C1_WriteNByte(u8 addr,u8* Buffer, u16 num)            // WAN.CG // 2010.12.18 
**�������� ����eepromд�������ֽڵ����� 
**������� ��addr:   д�����ݵ���ʼ��ַ
**           Buffer��д�����ݵĴ洢��ַ
**           num��   д�����ݵĸ���
**������� ��
**������ʾ ��
********************************************************************************************************/
void I2C1_WriteNByte(u8 addr,u8* Buffer, u16 num)
{
	u8 temp;
			
	temp=addr % I2C_PAGESIZE;	  //�Ȱ�ҳ������Ĳ���д��
	if(temp)
	{
		temp=I2C_PAGESIZE-temp;
		I2C1_WritePage(addr, Buffer, temp);
		num-=temp;
		addr+=temp;
		Buffer+=temp;
		I2C1_BusyTest();	         //д���
	}	 
	while(num)				 	 //��ҳ���뿪ʼд
	{
		if(num>=I2C_PAGESIZE)
		{
			I2C1_WritePage(addr, Buffer, I2C_PAGESIZE);
			num-=I2C_PAGESIZE;
			addr+=I2C_PAGESIZE;
			Buffer+=I2C_PAGESIZE;
			I2C1_BusyTest();	         //д���
		}
		else
		{
			I2C1_WritePage(addr, Buffer, num);
			num=0;
			I2C1_BusyTest();	         //д���
		}
	}
}

