/****************************************Copyright (c)****************************************************
** 
**                                      ���ڸ�����Ƽ�
**
**                                   http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			queue.h
** modified Date:  		2008.8.20
** Last Version:		V1.00
** Descriptions:		���ݶ���ͷ�ļ�
**	
*********************************************************************************************************/
#ifndef  IN_QUEUE
#define  IN_QUEUE
 
#ifndef  QUEUE_DATA_TYPE
#define  QUEUE_DATA_TYPE         int8u		/* ���ݶ����д洢����������     */

#endif
/********************************************************************************************************/
#ifndef NOT_OK
#define NOT_OK              0xff        /* ��������               */
#endif

#define QUEUE_OK            1           /* �����ɹ�                */
#define QUEUE_FULL          2           /* ������                  */
#define QUEUE_EMPTY         4           /* ������                  */ 

typedef struct {
    QUEUE_DATA_TYPE     *Out;                   /* ָ���������λ��         */
    QUEUE_DATA_TYPE     *In;                    /* ָ����������λ��         */
    QUEUE_DATA_TYPE     *End;                   /* ָ��Buf�Ľ���λ��        */
    int16u              NData;                  /* ���������ݸ���           */
    int16u              MaxData;                /* ����������洢�����ݸ��� */     
    int8u               (* ReadEmpty)();        /* ���մ�����             */
    int8u               (* WriteFull)();        /* д��������             */
    QUEUE_DATA_TYPE     Buf[1];                 /* �洢���ݵĿռ�           */
} DataQueue; 

/********************************************************************************************************/
#define EN_QUEUE_WRITE            1     /* ��ֹ(0)������(1)FIFO��������       */
#define EN_QUEUE_WRITE_FRONT      0     /* ��ֹ(0)������(1)LIFO��������       */
#define EN_QUEUE_NDATA            1     /* ��ֹ(0)������(1)ȡ�ö���������Ŀ   */
#define EN_QUEUE_SIZE             1     /* ��ֹ(0)������(1)ȡ�ö������������� */
#define EN_QUEUE_FLUSH            1     /* ��ֹ(0)������(1)��ն���           */

extern int8u  QueueCreate  (void *Buf,int32u SizeOfBuf,int8u (* ReadEmpty)(),int8u (* WriteFull)());
extern int8u  QueueRead(QUEUE_DATA_TYPE *Ret, void *Buf);
extern int8u  QueueReadOnly(QUEUE_DATA_TYPE *Ret, void *Buf);        
extern int8u  QueueWrite(void *Buf, QUEUE_DATA_TYPE Data);
extern int8u  QueueWriteFront(void *Buf, QUEUE_DATA_TYPE Data);
extern int16u QueueNData(void *Buf); 
extern int16u QueueSize(void *Buf);
extern void   QueueFlush(void *Buf);

#endif	
/*********************************************************************************************************
**                            End Of File
*********************************************************************************************************/
