#include "stm32lib.h"
#include"usart_debug.h"
void GPIO_Configuration(void)
{
GPIO_InitTypeDef  GPIO_InitStructure;
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能UASRT的时钟
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOA的时钟，开始的时候，我没用这句话，调了两天，跟源码一句一句比才知道
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9;
GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; //波特率较高，IO翻转需较高频率
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出;我看网上有人说设置成GPIO_Mode_Out_PP普通推挽输出也行，但实践出真知，我试了发送是乱码。;
GPIO_Init(GPIOA,&GPIO_InitStructure);
 
GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //配置成浮空输入，既然是输入所以就不用配置IO口的频率了
GPIO_Init(GPIOA,&GPIO_InitStructure);
}
 
void USART1_config(void)
{
USART_InitTypeDef     USART_InitStructure;
USART_InitStructure.USART_BaudRate= 115200;     //配置波特率
USART_InitStructure.USART_WordLength = USART_WordLength_8b; //配置数据位
USART_InitStructure.USART_StopBits = USART_StopBits_1;      //停止位
USART_InitStructure.USART_Parity= USART_Parity_No ;         //奇偶校验位
 
USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None; //硬件流
 
USART_InitStructure.USART_Mode =USART_Mode_Rx |USART_Mode_Tx;
USART_Init(USART1, &USART_InitStructure);
 
USART_Cmd(USART1, ENABLE);
}
//这段很重要，如果要使用printf函数打印信息，需要加fputc函数，就需要对printf函数重定向到串口，以前工作中他们老是提重定向，什么串口重定向，USB重定向什么的，我也是云里雾里，如今给我的感觉就是将上层函数实现对底层硬件的操作
int  fputc(int ch,  FILE*f){
while(USART_GetFlagStatus(USART1, USART_FLAG_TC) !=SET); //网上的一些函数里面是没有这一句代码，如果不加的话，打印时第一个字符就会没有，原因据说是硬件复位后，USART_FLAG_TC被置一了，而要发送数据必须让其为底才可以，一表示数据发送发出的标志，也可以用这样一句USART_ClearFlag(USART2,USART_FLAG_TC);清楚标准位。可是我没这样做一样发成功了，这个疑惑以后再想明白。
USART_SendData(USART1, (uint16_t)ch);
 
while(USART_GetFlagStatus(USART1, USART_FLAG_TC) !=SET);
return ch;
 
}
void usart_debug_config(void)  //提供给main函数调用的串口配置函数
{
GPIO_Configuration(); //IO口配置
USART1_config();   //串口配置
}
