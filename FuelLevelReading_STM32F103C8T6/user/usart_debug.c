#include "stm32lib.h"
#include"usart_debug.h"
void GPIO_Configuration(void)
{
GPIO_InitTypeDef  GPIO_InitStructure;
RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��UASRT��ʱ��
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��GPIOA��ʱ�ӣ���ʼ��ʱ����û����仰���������죬��Դ��һ��һ��Ȳ�֪��
RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9;
GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz; //�����ʽϸߣ�IO��ת��ϸ�Ƶ��
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������;�ҿ���������˵���ó�GPIO_Mode_Out_PP��ͨ�������Ҳ�У���ʵ������֪�������˷��������롣;
GPIO_Init(GPIOA,&GPIO_InitStructure);
 
GPIO_InitStructure.GPIO_Pin =GPIO_Pin_10;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //���óɸ������룬��Ȼ���������ԾͲ�������IO�ڵ�Ƶ����
GPIO_Init(GPIOA,&GPIO_InitStructure);
}
 
void USART1_config(void)
{
USART_InitTypeDef     USART_InitStructure;
USART_InitStructure.USART_BaudRate= 115200;     //���ò�����
USART_InitStructure.USART_WordLength = USART_WordLength_8b; //��������λ
USART_InitStructure.USART_StopBits = USART_StopBits_1;      //ֹͣλ
USART_InitStructure.USART_Parity= USART_Parity_No ;         //��żУ��λ
 
USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None; //Ӳ����
 
USART_InitStructure.USART_Mode =USART_Mode_Rx |USART_Mode_Tx;
USART_Init(USART1, &USART_InitStructure);
 
USART_Cmd(USART1, ENABLE);
}
//��κ���Ҫ�����Ҫʹ��printf������ӡ��Ϣ����Ҫ��fputc����������Ҫ��printf�����ض��򵽴��ڣ���ǰ�����������������ض���ʲô�����ض���USB�ض���ʲô�ģ���Ҳ��������������ҵĸо����ǽ��ϲ㺯��ʵ�ֶԵײ�Ӳ���Ĳ���
int  fputc(int ch,  FILE*f){
while(USART_GetFlagStatus(USART1, USART_FLAG_TC) !=SET); //���ϵ�һЩ����������û����һ����룬������ӵĻ�����ӡʱ��һ���ַ��ͻ�û�У�ԭ���˵��Ӳ����λ��USART_FLAG_TC����һ�ˣ���Ҫ�������ݱ�������Ϊ�ײſ��ԣ�һ��ʾ���ݷ��ͷ����ı�־��Ҳ����������һ��USART_ClearFlag(USART2,USART_FLAG_TC);�����׼λ��������û������һ�����ɹ��ˣ�����ɻ��Ժ��������ס�
USART_SendData(USART1, (uint16_t)ch);
 
while(USART_GetFlagStatus(USART1, USART_FLAG_TC) !=SET);
return ch;
 
}
void usart_debug_config(void)  //�ṩ��main�������õĴ������ú���
{
GPIO_Configuration(); //IO������
USART1_config();   //��������
}
