/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
**
**                                 http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			spiFLASH.c
** modified Date:  		2010-05-19
** Last Version:		V1.00
** Descriptions:		spi flash读写底层驱动程序
**	
*********************************************************************************************************/
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "stm32lib.h"
#include "spiflash.h"
#include "api.h"


/* 移植本软件包时需要修改以下的函数或宏 */
#define		SSEL1_Low()		GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define     SSEL1_High()	GPIO_SetBits(GPIOA, GPIO_Pin_4)

/* 一些操作flash命令的定义，需要根据具体flash型号修改 ，其他相同命令未列出*/	
#define     flashBUSY		0X11   //(AT26DF161D为0X11    其他器件为0X03)
								   //要特别注意ATMEL的Serial FLASH的Status Register定义与其他家有区别		
								   //特别是写使能位（WEL),每次操作之后都会归0.
#define     StatusWEN		0X06   //(使能状态寄存器可写，AT26DF161D为0X06    SST25VF016B为0X50)

/********************************************************************************************************
**函数信息 ：void SPIInit(void)                         // WAN.CG // 2011.3.25
**功能描述 ：SPI初始化函数
**输入参数 ：无
**输出参数 ：无
**调用提示 ：
********************************************************************************************************/
void SPI1Init(void)
{
    SPI_InitTypeDef  	SPI_InitStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;

	//PA5,PA6,PA7配置为SPI功能
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //PA4配置为SPI片选功能
	GPIO_SetBits(GPIOA, GPIO_Pin_4);                       //预置为高
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;			   //选择第端口3
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	   //50M时钟速度
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	   //推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);				   //GPIO配置函数

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_SPI1, ENABLE);
	SPI_Cmd(SPI1, DISABLE); 												//必须先禁能,才能改变MODE
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//两线全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							//主
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;						//8位
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;								//CPOL=0 时钟悬空低
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;							//CPHA=0 数据捕获第1个
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;								//软件NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//2分频=36M
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;						//高位在前
    SPI_InitStructure.SPI_CRCPolynomial = 7;								//CRC7
    
	SPI_Init(SPI1, &SPI_InitStructure);
	//SPI_SSOutputCmd(SPI1, ENABLE); //使能NSS脚可用
    SPI_Cmd(SPI1, ENABLE); 	  
}


/************************************************************************
** 函数名称: Send_Byte													
** 函数功能：通过硬件SPI发送一个字节到SPI Serial Flash					
** 入口参数:data															
** 出口参数:无																
************************************************************************/
int8u Send_Byte(int8u data)
{ 		
	while((SPI1->SR & SPI_I2S_FLAG_TXE)==RESET);	//等待发送寄存器空*/  
	SPI1->DR = data;							    //发送一个字节*/ 					   
	while((SPI1->SR & SPI_I2S_FLAG_RXNE)==RESET);	//等待接收寄存器有效*/	
	return(SPI1->DR);     
}

/************************************************************************
** 函数名称:Get_Byte														
** 函数功能:通过硬件SPI接口接收一个字节到处理器						
** 入口参数:无																
** 出口参数:无																
************************************************************************/
int8u Get_Byte(void)
{ 		
	while((SPI1->SR & SPI_I2S_FLAG_TXE)==RESET);	//等待发送寄存器空*/  
	SPI1->DR = 0xff;							    //发送一个字节*/ 					   
	while((SPI1->SR & SPI_I2S_FLAG_RXNE)==RESET);	//等待接收寄存器有效*/	
	return(SPI1->DR);     
}
 
/* 以下函数在移植时无需修改 */
/************************************************************************
** 函数名称:spiFLASH_RD																										
** 函数功能:SPI Serial Flash的读函数,可选择读ID和读数据操作				
** 入口参数:int32u Dst：目标地址,范围 0x0 - MAX_ADDR（MAX_ADDR = 0x1FFFFF）				
**      	int32u NByte:	要读取的数据字节数
**			int8u* RcvBufPt:接收缓存的指针			
** 出口参数:操作成功则返回OK,失败则返回ERROR		
** 注	 意:若某功能下,某一入口参数无效,可在该入口参数处填Invalid，该参数将被忽略
************************************************************************/
int8u spiFLASH_RD(int32u Dst,int8u* RcvBufPt, int32u NByte)
{
	int32u i = 0;
	if ((Dst+NByte > ADDR_MAX)||(NByte == 0))	return (ERROR);	 //	检查入口参数
	
	SSEL1_Low();			
	Send_Byte(0x0B); 						// 发送读命令
	Send_Byte(((Dst & 0xFFFFFF) >> 16));	// 发送地址信息:该地址由3个字节组成
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	Send_Byte(0xFF);						// 发送一个哑字节以读取数据
	for (i = 0; i < NByte; i++)		
	{
		RcvBufPt[i] = Get_Byte();		
	}
	SSEL1_High();			
	return (OK);
}

/************************************************************************
** 函数名称:spiFLASH_RdID																										
** 函数功能:SPI Serial Flash的读ID函数,可选择读ID和读数据操作				
** 入口参数:idtype IDType:ID类型。用户可在Jedec_ID,Dev_ID,Manu_ID三者里选择
**			int32u* RcvbufPt:存储ID变量的指针
** 出口参数:操作成功则返回OK,失败则返回ERROR		
** 注	 意:若填入的参数不符合要求，则返回ERROR
************************************************************************/
int8u spiFLASH_RdID(idtype IDType, int32u* RcvbufPt)
{
	int32u temp = 0;
	if (IDType == Jedec_ID)
	{
		SSEL1_Low();			
		Send_Byte(0x9F);		 		// 发送读JEDEC ID命令(9Fh)
    	temp = (temp | Get_Byte()) << 8;// 接收数据 
		temp = (temp | Get_Byte()) << 8;
		temp = (temp | Get_Byte()) << 8;	
		temp = (temp | Get_Byte()); 	// 在本例中,temp的值应为0x1F460100
		SSEL1_High();			
		*RcvbufPt = temp;
		return (OK);
	}
	
	if ((IDType == Manu_ID) || (IDType == Dev_ID) )
	{
		SSEL1_Low();			
		Send_Byte(0x90);				// 发送读ID命令 (90h or ABh)
    	Send_Byte(0x00);				// 发送地址
		Send_Byte(0x00);				// 发送地址
		Send_Byte(IDType);				// 发送地址 - 不是00H就是01H
		temp = Get_Byte();				// 接收获取的数据字节
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
** 函数名称:spiFLASH_WR											
** 函数功能:SPI Serial Flash的写函数，可写1个和多个数据到指定地址									
** 入口参数:int32u Dst：目标地址,范围 0x0 - MAX_ADDR（MAX_ADDR = 0x1FFFFF）				
**			int8u* SndbufPt:发送缓存区指针
**      	int32u NByte:要写的数据字节数
** 出口参数:操作成功则返回OK,失败则返回ERROR		
** 注	 意:若某功能下,某一入口参数无效,可在该入口参数处填Invalid，该参数将被忽略
************************************************************************/
int8u spiFLASH_WR(int32u Dst, int8u* SndbufPt, int32u NByte)
{
	int32u temp = 0,i = 0,StatRgVal = 0;
	if (( (Dst+NByte-1 > ADDR_MAX)||(NByte == 0) ))
	{
		return (ERROR);	 //	检查入口参数
	}
	
   	SSEL1_Low();			 
	Send_Byte(0x05);							// 发送读状态寄存器命令
	temp = Get_Byte();							// 保存读得的状态寄存器值
	SSEL1_High(); 							

	SSEL1_Low();			
	Send_Byte(StatusWEN);						// 使状态寄存器可写
	SSEL1_High();			
	SSEL1_Low();			
	Send_Byte(0x01);							// 发送写状态寄存器指令
	Send_Byte(0);								// 清0BPx位，使Flash芯片全区可写 
	SSEL1_High();			

		
	for(i = 0; i < NByte; i++)
	{
		SSEL1_Low();			
		Send_Byte(0x06);						// 发送写使能命令
		SSEL1_High();			

		SSEL1_Low();			
		Send_Byte(0x02); 						// 发送字节数据烧写命令
		Send_Byte((((Dst+i) & 0xFFFFFF) >> 16));// 发送3个字节的地址信息 
		Send_Byte((((Dst+i) & 0xFFFF) >> 8));
		Send_Byte((Dst+i) & 0xFF);
		Send_Byte(SndbufPt[i]);					// 发送被烧写的数据
		SSEL1_High();			

		do
		{
		  	SSEL1_Low();			 
			Send_Byte(0x05);					// 发送读状态寄存器命令
			StatRgVal = Get_Byte();				// 保存读得的状态寄存器值
			SSEL1_High();								
  		}
		while (StatRgVal == flashBUSY);			// 一直等待，直到芯片空闲
	}

	SSEL1_Low();			
	Send_Byte(StatusWEN);						// 使状态寄存器可写
	SSEL1_High();			
	SSEL1_Low();			
	Send_Byte(0x01);							// 发送写状态寄存器指令
	Send_Byte(temp);							// 恢复状态寄存器设置信息 
	SSEL1_High();

	return (OK);		
}

/************************************************************************
** 函数名称:spiFLASH_Erase												
** 函数功能:根据指定的扇区号选取最高效的算法擦除								
** 入口参数:int32u sec1：起始扇区号,范围(0~511)
**			int32u sec2：终止扇区号,范围(0~511)
** 出口参数:操作成功则返回OK,失败则返回ERROR		
************************************************************************/
int8u spiFLASH_Erase(int32u sec1, int32u sec2)
{
	int8u  temp1 = 0,temp2 = 0,StatRgVal = 0;
    int32u SecnHdAddr = 0;	  			
	int32u no_SecsToEr = 0;				   			// 要擦除的扇区数目
	int32u CurSecToEr = 0;	  						// 当前要擦除的扇区号
	
	/*  检查入口参数 */
	if ((sec1 > SEC_MAX)||(sec2 > SEC_MAX))	
	{
		return (ERROR);	
	}
   	
   	SSEL1_Low();			 
	Send_Byte(0x05);								// 发送读状态寄存器命令
	temp1 = Get_Byte();								// 保存读得的状态寄存器值
	SSEL1_High();	
								
	SSEL1_Low();			
	Send_Byte(StatusWEN);							// 使状态寄存器可写
	SSEL1_High();			
	SSEL1_Low();								  	
	Send_Byte(0x01);								// 发送写状态寄存器指令
	Send_Byte(0);									// 清0BPx位，使Flash芯片全区可写 
	SSEL1_High(); 
	
	SSEL1_Low();			
	Send_Byte(0x06);						   		 // 发送写使能命令
	SSEL1_High();

	/* 如果用户输入的起始扇区号大于终止扇区号，则在内部作出调整 */
	if (sec1 > sec2)
	{
	   temp2 = sec1;
	   sec1  = sec2;
	   sec2  = temp2;
	} 
						
	
	if (sec2 - sec1 == SEC_MAX)					    //是否整片擦除
	{
		SSEL1_Low();			
		Send_Byte(0x60);							// 发送芯片擦除指令(60h or C7h)
		SSEL1_High();			
		do
		{
		  	SSEL1_Low();			 
			Send_Byte(0x05);						// 发送读状态寄存器命令
			StatRgVal = Get_Byte();					// 保存读得的状态寄存器值
			SSEL1_High();								
  		}
		while (StatRgVal == flashBUSY);				// 一直等待，直到芯片空闲
   		return (OK);
	}

	
	no_SecsToEr = sec2 - sec1 +1;					// 获取要擦除的扇区数目
	CurSecToEr  = sec1;								// 从起始扇区开始擦除 	

	while (no_SecsToEr >= 1)
	{
	    SSEL1_Low();			
		Send_Byte(0x06);						   	// 发送写使能命令
		SSEL1_High(); 
			
		SecnHdAddr = SEC_SIZE * CurSecToEr;			// 计算扇区的起始地址
	    SSEL1_Low();	
    	Send_Byte(0x20);							// 发送扇区擦除指令（4k）
	    Send_Byte(((SecnHdAddr & 0xFFFFFF) >> 16)); // 发送3个字节的地址信息
   		Send_Byte(((SecnHdAddr & 0xFFFF) >> 8));
   		Send_Byte(SecnHdAddr & 0xFF);
  		SSEL1_High();			
		do
		{
		  	SSEL1_Low();			 
			Send_Byte(0x05);						// 发送读状态寄存器命令
			StatRgVal = Get_Byte();					// 保存读得的状态寄存器值
			SSEL1_High();								
  		}
		while (StatRgVal == flashBUSY);				// 一直等待，直到芯片空闲
		CurSecToEr  += 1;
		no_SecsToEr -=  1;
	}
   /* 擦除结束,恢复状态寄存器信息							*/
	
	SSEL1_Low();			
	Send_Byte(StatusWEN);							// 使状态寄存器可写
	SSEL1_High();
				
	SSEL1_Low();			
	Send_Byte(0x01);								// 发送写状态寄存器指令
	Send_Byte(temp1);								// 恢复状态寄存器设置信息 
	SSEL1_High(); 
	   
	return (OK);
}







