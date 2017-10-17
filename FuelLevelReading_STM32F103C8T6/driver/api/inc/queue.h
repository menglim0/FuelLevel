/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
**
**                                   http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			queue.h
** modified Date:  		2008.8.20
** Last Version:		V1.00
** Descriptions:		数据队列头文件
**	
*********************************************************************************************************/
#ifndef  IN_QUEUE
#define  IN_QUEUE
 
#ifndef  QUEUE_DATA_TYPE
#define  QUEUE_DATA_TYPE         int8u		/* 数据队列中存储的数据类型     */

#endif
/********************************************************************************************************/
#ifndef NOT_OK
#define NOT_OK              0xff        /* 参数错误               */
#endif

#define QUEUE_OK            1           /* 操作成功                */
#define QUEUE_FULL          2           /* 队列满                  */
#define QUEUE_EMPTY         4           /* 无数据                  */ 

typedef struct {
    QUEUE_DATA_TYPE     *Out;                   /* 指向数据输出位置         */
    QUEUE_DATA_TYPE     *In;                    /* 指向数据输入位置         */
    QUEUE_DATA_TYPE     *End;                   /* 指向Buf的结束位置        */
    int16u              NData;                  /* 队列中数据个数           */
    int16u              MaxData;                /* 队列中允许存储的数据个数 */     
    int8u               (* ReadEmpty)();        /* 读空处理函数             */
    int8u               (* WriteFull)();        /* 写满处理函数             */
    QUEUE_DATA_TYPE     Buf[1];                 /* 存储数据的空间           */
} DataQueue; 

/********************************************************************************************************/
#define EN_QUEUE_WRITE            1     /* 禁止(0)或允许(1)FIFO发送数据       */
#define EN_QUEUE_WRITE_FRONT      0     /* 禁止(0)或允许(1)LIFO发送数据       */
#define EN_QUEUE_NDATA            1     /* 禁止(0)或允许(1)取得队列数据数目   */
#define EN_QUEUE_SIZE             1     /* 禁止(0)或允许(1)取得队列数据总容量 */
#define EN_QUEUE_FLUSH            1     /* 禁止(0)或允许(1)清空队列           */

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
