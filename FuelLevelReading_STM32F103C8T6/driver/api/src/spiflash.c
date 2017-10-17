/****************************************Copyright (c)****************************************************
** 
**                                      ���ڸ�����Ƽ�
**
**                                 http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			spiFLASH.c
** modified Date:  		2010-05-19
** Last Version:		V1.00
** Descriptions:		spi flash��д�ײ���������
**	
*********************************************************************************************************/
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "stm32lib.h"
#include "spiflash.h"
#include "api.h"


/* ��ֲ�������ʱ��Ҫ�޸����µĺ������ */
#define		SSEL1_Low()		GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define     SSEL1_High()	GPIO_SetBits(GPIOA, GPIO_Pin_4)

/* һЩ����flash����Ķ��壬��Ҫ���ݾ���flash�ͺ��޸� ��������ͬ����δ�г�*/	
#define     flashBUSY		0X11   //(AT26DF161DΪ0X11    ��������Ϊ0X03)
								   //Ҫ�ر�ע��ATMEL��Serial FLASH��Status Register������������������		
								   //�ر���дʹ��λ��WEL),ÿ�β���֮�󶼻��0.
#define     StatusWEN		0X06   //(ʹ��״̬�Ĵ�����д��AT26DF161DΪ0X06    SST25VF016BΪ0X50)

/********************************************************************************************************
**������Ϣ ��void SPIInit(void)                         // WAN.CG // 2011.3.25
**�������� ��SPI��ʼ������
**������� ����
**������� ����
**������ʾ ��
********************************************************************************************************/
void SPI1Init(void)
{
    SPI_InitTypeDef  	SPI_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;

	//PA5,PA6,PA7����ΪSPI����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //PA4����ΪSPIƬѡ����
	GPIO_SetBits(GPIOA, GPIO_Pin_4);                       //Ԥ��Ϊ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;			   //ѡ��ڶ˿�3
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //50Mʱ���ٶ�
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //�������
    GPIO_Init(GPIOA, &GPIO_InitStructure);				   //GPIO���ú���

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_SPI1, ENABLE);
	SPI_Cmd(SPI1, DISABLE); 												//�����Ƚ���,���ܸı�MODE
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//����ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							//��
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;						//8λ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;								//CPOL=0 ʱ�����յ�
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;							//CPHA=0 ���ݲ����1��
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;								//���NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//2��Ƶ=36M
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;						//��λ��ǰ
    SPI_InitStructure.SPI_CRCPolynomial = 7;								//CRC7
    
	SPI_Init(SPI1, &SPI_InitStructure);
	//SPI_SSOutputCmd(SPI1, ENABLE); //ʹ��NSS�ſ���
    SPI_Cmd(SPI1, ENABLE); 	  
}


/************************************************************************
** ��������: Send_Byte													
** �������ܣ�ͨ��Ӳ��SPI����һ���ֽڵ�SPI Serial Flash					
** ��ڲ���:data															
** ���ڲ���:��																
************************************************************************/
int8u Send_Byte(int8u data)
{ 		
	while((SPI1->SR & SPI_I2S_FLAG_TXE)==RESET);	//�ȴ����ͼĴ�����*/  
	SPI1->DR = data;							    //����һ���ֽ�*/ 					   
	while((SPI1->SR & SPI_I2S_FLAG_RXNE)==RESET);	//�ȴ����ռĴ�����Ч*/	
	return(SPI1->DR);     
}

/************************************************************************
** ��������:Get_Byte														
** ��������:ͨ��Ӳ��SPI�ӿڽ���һ���ֽڵ�������						
** ��ڲ���:��																
** ���ڲ���:��																
************************************************************************/
int8u Get_Byte(void)
{ 		
	while((SPI1->SR & SPI_I2S_FLAG_TXE)==RESET);	//�ȴ����ͼĴ�����*/  
	SPI1->DR = 0xff;							    //����һ���ֽ�*/ 					   
	while((SPI1->SR & SPI_I2S_FLAG_RXNE)==RESET);	//�ȴ����ռĴ�����Ч*/	
	return(SPI1->DR);     
}
 
/* ���º�������ֲʱ�����޸� */
/************************************************************************
** ��������:spiFLASH_RD																										
** ��������:SPI Serial Flash�Ķ�����,��ѡ���ID�Ͷ����ݲ���				
** ��ڲ���:int32u Dst��Ŀ���ַ,��Χ 0x0 - MAX_ADDR��MAX_ADDR = 0x1FFFFF��				
**      	int32u NByte:	Ҫ��ȡ�������ֽ���
**			int8u* RcvBufPt:���ջ����ָ��			
** ���ڲ���:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR		
** ע	 ��:��ĳ������,ĳһ��ڲ�����Ч,���ڸ���ڲ�������Invalid���ò�����������
************************************************************************/
int8u spiFLASH_RD(int32u Dst,int8u* RcvBufPt, int32u NByte)
{
	int32u i = 0;
	if ((Dst+NByte > ADDR_MAX)||(NByte == 0))	return (ERROR);	 //	�����ڲ���
	
	SSEL1_Low();			
	Send_Byte(0x0B); 						// ���Ͷ�����
	Send_Byte(((Dst & 0xFFFFFF) >> 16));	// ���͵�ַ��Ϣ:�õ�ַ��3���ֽ����
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	Send_Byte(0xFF);						// ����һ�����ֽ��Զ�ȡ����
	for (i = 0; i < NByte; i++)		
	{
		RcvBufPt[i] = Get_Byte();		
	}
	SSEL1_High();			
	return (OK);
}

/************************************************************************
** ��������:spiFLASH_RdID																										
** ��������:SPI Serial Flash�Ķ�ID����,��ѡ���ID�Ͷ����ݲ���				
** ��ڲ���:idtype IDType:ID���͡��û�����Jedec_ID,Dev_ID,Manu_ID������ѡ��
**			int32u* RcvbufPt:�洢ID������ָ��
** ���ڲ���:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR		
** ע	 ��:������Ĳ���������Ҫ���򷵻�ERROR
************************************************************************/
int8u spiFLASH_RdID(idtype IDType, int32u* RcvbufPt)
{
	int32u temp = 0;
	if (IDType == Jedec_ID)
	{
		SSEL1_Low();			
		Send_Byte(0x9F);		 		// ���Ͷ�JEDEC ID����(9Fh)
    	temp = (temp | Get_Byte()) << 8;// �������� 
		temp = (temp | Get_Byte()) << 8;
		temp = (temp | Get_Byte()) << 8;	
		temp = (temp | Get_Byte()); 	// �ڱ�����,temp��ֵӦΪ0x1F460100
		SSEL1_High();			
		*RcvbufPt = temp;
		return (OK);
	}
	
	if ((IDType == Manu_ID) || (IDType == Dev_ID) )
	{
		SSEL1_Low();			
		Send_Byte(0x90);				// ���Ͷ�ID���� (90h or ABh)
    	Send_Byte(0x00);				// ���͵�ַ
		Send_Byte(0x00);				// ���͵�ַ
		Send_Byte(IDType);				// ���͵�ַ - ����00H����01H
		temp = Get_Byte();				// ���ջ�ȡ�������ֽ�
		SSEL1_High();			
		*RcvbufPt = temp;
		return (OK);
	}
	else
	{
		return (ERROR);	
	}
}

/************************************************************************
** ��������:spiFLASH_WR											
** ��������:SPI Serial Flash��д��������д1���Ͷ�����ݵ�ָ����ַ									
** ��ڲ���:int32u Dst��Ŀ���ַ,��Χ 0x0 - MAX_ADDR��MAX_ADDR = 0x1FFFFF��				
**			int8u* SndbufPt:���ͻ�����ָ��
**      	int32u NByte:Ҫд�������ֽ���
** ���ڲ���:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR		
** ע	 ��:��ĳ������,ĳһ��ڲ�����Ч,���ڸ���ڲ�������Invalid���ò�����������
************************************************************************/
int8u spiFLASH_WR(int32u Dst, int8u* SndbufPt, int32u NByte)
{
	int32u temp = 0,i = 0,StatRgVal = 0;
	if (( (Dst+NByte-1 > ADDR_MAX)||(NByte == 0) ))
	{
		return (ERROR);	 //	�����ڲ���
	}
	
   	SSEL1_Low();			 
	Send_Byte(0x05);							// ���Ͷ�״̬�Ĵ�������
	temp = Get_Byte();							// ������õ�״̬�Ĵ���ֵ
	SSEL1_High(); 							

	SSEL1_Low();			
	Send_Byte(StatusWEN);						// ʹ״̬�Ĵ�����д
	SSEL1_High();			
	SSEL1_Low();			
	Send_Byte(0x01);							// ����д״̬�Ĵ���ָ��
	Send_Byte(0);								// ��0BPxλ��ʹFlashоƬȫ����д 
	SSEL1_High();			

		
	for(i = 0; i < NByte; i++)
	{
		SSEL1_Low();			
		Send_Byte(0x06);						// ����дʹ������
		SSEL1_High();			

		SSEL1_Low();			
		Send_Byte(0x02); 						// �����ֽ�������д����
		Send_Byte((((Dst+i) & 0xFFFFFF) >> 16));// ����3���ֽڵĵ�ַ��Ϣ 
		Send_Byte((((Dst+i) & 0xFFFF) >> 8));
		Send_Byte((Dst+i) & 0xFF);
		Send_Byte(SndbufPt[i]);					// ���ͱ���д������
		SSEL1_High();			

		do
		{
		  	SSEL1_Low();			 
			Send_Byte(0x05);					// ���Ͷ�״̬�Ĵ�������
			StatRgVal = Get_Byte();				// ������õ�״̬�Ĵ���ֵ
			SSEL1_High();								
  		}
		while (StatRgVal == flashBUSY);			// һֱ�ȴ���ֱ��оƬ����
	}

	SSEL1_Low();			
	Send_Byte(StatusWEN);						// ʹ״̬�Ĵ�����д
	SSEL1_High();			
	SSEL1_Low();			
	Send_Byte(0x01);							// ����д״̬�Ĵ���ָ��
	Send_Byte(temp);							// �ָ�״̬�Ĵ���������Ϣ 
	SSEL1_High();

	return (OK);		
}

/************************************************************************
** ��������:spiFLASH_Erase												
** ��������:����ָ����������ѡȡ���Ч���㷨����								
** ��ڲ���:int32u sec1����ʼ������,��Χ(0~511)
**			int32u sec2����ֹ������,��Χ(0~511)
** ���ڲ���:�����ɹ��򷵻�OK,ʧ���򷵻�ERROR		
************************************************************************/
int8u spiFLASH_Erase(int32u sec1, int32u sec2)
{
	int8u  temp1 = 0,temp2 = 0,StatRgVal = 0;
    int32u SecnHdAddr = 0;	  			
	int32u no_SecsToEr = 0;				   			// Ҫ������������Ŀ
	int32u CurSecToEr = 0;	  						// ��ǰҪ������������
	
	/*  �����ڲ��� */
	if ((sec1 > SEC_MAX)||(sec2 > SEC_MAX))	
	{
		return (ERROR);	
	}
   	
   	SSEL1_Low();			 
	Send_Byte(0x05);								// ���Ͷ�״̬�Ĵ�������
	temp1 = Get_Byte();								// ������õ�״̬�Ĵ���ֵ
	SSEL1_High();	
								
	SSEL1_Low();			
	Send_Byte(StatusWEN);							// ʹ״̬�Ĵ�����д
	SSEL1_High();			
	SSEL1_Low();								  	
	Send_Byte(0x01);								// ����д״̬�Ĵ���ָ��
	Send_Byte(0);									// ��0BPxλ��ʹFlashоƬȫ����д 
	SSEL1_High(); 
	
	SSEL1_Low();			
	Send_Byte(0x06);						   		 // ����дʹ������
	SSEL1_High();

	/* ����û��������ʼ�����Ŵ�����ֹ�����ţ������ڲ��������� */
	if (sec1 > sec2)
	{
	   temp2 = sec1;
	   sec1  = sec2;
	   sec2  = temp2;
	} 
						
	
	if (sec2 - sec1 == SEC_MAX)					    //�Ƿ���Ƭ����
	{
		SSEL1_Low();			
		Send_Byte(0x60);							// ����оƬ����ָ��(60h or C7h)
		SSEL1_High();			
		do
		{
		  	SSEL1_Low();			 
			Send_Byte(0x05);						// ���Ͷ�״̬�Ĵ�������
			StatRgVal = Get_Byte();					// ������õ�״̬�Ĵ���ֵ
			SSEL1_High();								
  		}
		while (StatRgVal == flashBUSY);				// һֱ�ȴ���ֱ��оƬ����
   		return (OK);
	}

	
	no_SecsToEr = sec2 - sec1 +1;					// ��ȡҪ������������Ŀ
	CurSecToEr  = sec1;								// ����ʼ������ʼ���� 	

	while (no_SecsToEr >= 1)
	{
	    SSEL1_Low();			
		Send_Byte(0x06);						   	// ����дʹ������
		SSEL1_High(); 
			
		SecnHdAddr = SEC_SIZE * CurSecToEr;			// ������������ʼ��ַ
	    SSEL1_Low();	
    	Send_Byte(0x20);							// ������������ָ�4k��
	    Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16)); // ����3���ֽڵĵ�ַ��Ϣ
   		Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
   		Send_Byte(SecnHdAddr & 0xFF);
  		SSEL1_High();			
		do
		{
		  	SSEL1_Low();			 
			Send_Byte(0x05);						// ���Ͷ�״̬�Ĵ�������
			StatRgVal = Get_Byte();					// ������õ�״̬�Ĵ���ֵ
			SSEL1_High();								
  		}
		while (StatRgVal == flashBUSY);				// һֱ�ȴ���ֱ��оƬ����
		CurSecToEr  += 1;
		no_SecsToEr -=  1;
	}
   /* ��������,�ָ�״̬�Ĵ�����Ϣ							*/
	
	SSEL1_Low();			
	Send_Byte(StatusWEN);							// ʹ״̬�Ĵ�����д
	SSEL1_High();
				
	SSEL1_Low();			
	Send_Byte(0x01);								// ����д״̬�Ĵ���ָ��
	Send_Byte(temp1);								// �ָ�״̬�Ĵ���������Ϣ 
	SSEL1_High(); 
	   
	return (OK);
}







