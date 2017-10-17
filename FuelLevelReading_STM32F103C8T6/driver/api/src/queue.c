/****************************************Copyright (c)****************************************************
** 
**                                      深圳格兰瑞科技
**
**                                   http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			queue.c
** modified Date:  		2008.8.20
** Last Version:		V1.00
** Descriptions:		数据队列
**	
*********************************************************************************************************/

#include "api.h"
#include "queue.h"

/*********************************************************************************************************
** 函数名称: QueueCreate
** 功能描述: 初始化数据队列
** 输  　入: Buf      ：为队列分配的存储空间地址
**           SizeOfBuf：为队列分配的存储空间大小（字节）
**           ReadEmpty：为队列读空时处理程序
**           WriteFull：为队列写满时处理程序
** 输  　出: NOT_OK:参数错误
**           QUEUE_OK:成功
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
int8u QueueCreate(void *Buf, int32u SizeOfBuf, int8u (* ReadEmpty)(), int8u (* WriteFull)())
{ 
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
	    DataQueue *Queue;
    
    if (Buf != NULL && SizeOfBuf >= (sizeof(DataQueue)))        /* 判断参数是否有效 */
    {
        Queue = (DataQueue *)Buf;

        OS_ENTER_CRITICAL();
                                                                /* 初始化结构体数据 */
        Queue->MaxData = (SizeOfBuf - (int32u)(((DataQueue *)0)->Buf)) / 
                         sizeof(QUEUE_DATA_TYPE);               /* 计算队列可以存储的数据数目 */
        Queue->End = Queue->Buf + Queue->MaxData;               /* 计算数据缓冲的结束地址 */
        Queue->Out = Queue->Buf;
        Queue->In = Queue->Buf;
        Queue->NData = 0;
        Queue->ReadEmpty = ReadEmpty;							/* 队列读空处理函数，由用户定义 */
        Queue->WriteFull = WriteFull;							/* 队列写满处理函数，由用户定义 */

        OS_EXIT_CRITICAL();

        return QUEUE_OK;
    }
    else
    {
        return NOT_OK;
    }
}	

/*********************************************************************************************************
** 函数名称: QueueRead
** 功能描述: 获取队列中的数据
** 输  　入: Ret:存储返回的消息的地址
**           Buf:指向队列的指针
** 输  　出: NOT_OK     ：参数错误
**           QUEUE_OK   ：收到消息
**           QUEUE_EMPTY：无消息
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/
int8u QueueRead(QUEUE_DATA_TYPE *Ret, void *Buf)
{
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
    int8u err;
    DataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                            /* 队列是否有效 */
    {                                                           /* 有效 */
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData > 0)                                   /* 队列是否为空 */
        {                                                       /* 不空         */
            *Ret = Queue->Out[0];                               /* 数据出队     */
            Queue->Out++;                                       /* 调整出队指针 */
            if (Queue->Out >= Queue->End)
            {
                Queue->Out = Queue->Buf;
            }
            Queue->NData--;                                     /* 数据减少      */
            err = QUEUE_OK;
        }
        else
        {                                                       /* 空              */
            err = QUEUE_EMPTY;
            if (Queue->ReadEmpty != NULL)                       /* 调用用户处理函数 */
            {
                err = Queue->ReadEmpty(Ret, Queue);
            }
        }
        OS_EXIT_CRITICAL();
    }
    return err;
}

/*********************************************************************************************************
** 函数名称: QueueReadOnly
** 功能描述: 获取队列中的数据,但不改变原数据队列
** 输  　入: Ret:存储返回的消息的地址
**           Buf:指向队列的指针
** 输  　出: NOT_OK     ：参数错误
**           QUEUE_OK   ：收到消息
**           QUEUE_EMPTY：无消息
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/
int8u QueueReadOnly(QUEUE_DATA_TYPE *Ret, void *Buf)
{
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
    int8u err;
    DataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                            /* 队列是否有效 */
    {                                                           /* 有效 */
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData > 0)                                   /* 队列是否为空 */
        {                                                       /* 不空         */
            *Ret = Queue->Out[0];                               /* 数据出队     */
            err = QUEUE_OK;
        }
        else
        {                                                       /* 空              */
            err = QUEUE_EMPTY;
        }
        
        OS_EXIT_CRITICAL();
    }
    return err;
}

/*********************************************************************************************************
** 函数名称: QueueWrite
** 功能描述: FIFO方式发送数据
** 输  　入: Buf :指向队列的指针
**           Data:消息数据
** 输  　出: NOT_OK   :参数错误
**           QUEUE_FULL:队列满
**           QUEUE_OK  :发送成功
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/
#if EN_QUEUE_WRITE > 0 
int8u QueueWrite(void *Buf, QUEUE_DATA_TYPE Data)
{
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
    int8u err;
    DataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                                    /* 队列是否有效 */
    {
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData < Queue->MaxData)                              /* 队列是否满  */
        {                                                               /* 不满        */
            Queue->In[0] = Data;                                        /* 数据入队    */
            Queue->In++;                                                /* 调整入队指针*/
            if (Queue->In >= Queue->End)
            {
                Queue->In = Queue->Buf;
            }
            Queue->NData++;                                             /* 数据增加    */
            err = QUEUE_OK;
        }
        else
        {                                                               /* 满           */
            err = QUEUE_FULL;
            if (Queue->WriteFull != NULL)                               /* 调用用户处理函数 */
            {
                err = Queue->WriteFull(Queue, Data);
            }
        }
        OS_EXIT_CRITICAL();
    }
    return err;
}
#endif

/*********************************************************************************************************
** 函数名称: QueueWriteFront
** 功能描述: LIFO方式发送数据
** 输  　入: Buf:指向队列的指针
**           Data:消息数据
** 输　  出: QUEUE_FULL:队列满
**           QUEUE_OK:发送成功
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/
#if EN_QUEUE_WRITE_FRONT > 0	   
int8u QueueWriteFront(void *Buf, QUEUE_DATA_TYPE Data)
{
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
    int8u err;
    DataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                                    /* 队列是否有效 */
    {
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData < Queue->MaxData)                              /* 队列是否满  */
        {                                                               /* 不满 */
            Queue->Out--;                                               /* 调整出队指针 */
            if (Queue->Out < Queue->Buf)
            {
                Queue->Out = Queue->End - 1;
            }
            Queue->Out[0] = Data;                                       /* 数据入队     */
            Queue->NData++;                                             /* 数据数目增加 */
            err = QUEUE_OK;
        }
        else
        {                                                               /* 满           */
            err = QUEUE_FULL;
            if (Queue->WriteFull != NULL)                               /* 调用用户处理函数 */
            {
                err = Queue->WriteFull(Queue, Data);
            }
        }
        OS_EXIT_CRITICAL();
    }
    return err;
}
#endif

/*********************************************************************************************************
** 函数名称: QueueNData
** 功能描述: 取得队列中数据数
** 输  　入: Buf:指向队列的指针
** 输  　出: 消息数
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/
#if EN_QUEUE_NDATA > 0					  
int16u QueueNData(void *Buf)
{
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
    int16u temp;
    
    temp = 0;                                                   /* 队列无效返回0 */
    if (Buf != NULL)
    {
        OS_ENTER_CRITICAL();
        temp = ((DataQueue *)Buf)->NData;
        OS_EXIT_CRITICAL();
    }
    return temp;
} 
#endif	

/*********************************************************************************************************
** 函数名称: QueueSize
** 功能描述: 取得队列总容量
** 输  　入: Buf:指向队列的指针
** 输  　出: 队列总容量
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/ 
#if EN_QUEUE_SIZE > 0				   
int16u QueueSize(void *Buf)
{
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
    int16u temp;
    
    temp = 0;                                                   /* 队列无效返回0 */
    if (Buf != NULL)
    {
        OS_ENTER_CRITICAL();
        temp = ((DataQueue *)Buf)->MaxData;
        OS_EXIT_CRITICAL();
    }
    return temp;
}	  
#endif

/*********************************************************************************************************
** 函数名称: OSQFlush
** 功能描述: 清空队列
** 输　  入: Buf:指向队列的指针
** 输  　出: 无
** 全局变量: 无
** 调用模块: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/
#if EN_QUEUE_FLUSH > 0								 
void QueueFlush(void *Buf)
{
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
    DataQueue *Queue;
    
    if (Buf != NULL)                                                /* 队列是否有效 */
    {                                                               /* 有效         */
        Queue = (DataQueue *)Buf;
        OS_ENTER_CRITICAL();
        Queue->Out = Queue->Buf;
        Queue->In = Queue->Buf;
        Queue->NData = 0;                                           /* 数据数目为0 */
        OS_EXIT_CRITICAL();
    }
} 
#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
