/****************************************Copyright (c)****************************************************
** 
**                                      ���ڸ�����Ƽ�
**
**                                   http://www.mcu118.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:			queue.c
** modified Date:  		2008.8.20
** Last Version:		V1.00
** Descriptions:		���ݶ���
**	
*********************************************************************************************************/

#include "api.h"
#include "queue.h"

/*********************************************************************************************************
** ��������: QueueCreate
** ��������: ��ʼ�����ݶ���
** ��  ����: Buf      ��Ϊ���з���Ĵ洢�ռ��ַ
**           SizeOfBuf��Ϊ���з���Ĵ洢�ռ��С���ֽڣ�
**           ReadEmpty��Ϊ���ж���ʱ�������
**           WriteFull��Ϊ����д��ʱ�������
** ��  ����: NOT_OK:��������
**           QUEUE_OK:�ɹ�
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
int8u QueueCreate(void *Buf, int32u SizeOfBuf, int8u (* ReadEmpty)(), int8u (* WriteFull)())
{ 
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
	    DataQueue *Queue;
    
    if (Buf != NULL && SizeOfBuf >= (sizeof(DataQueue)))        /* �жϲ����Ƿ���Ч */
    {
        Queue = (DataQueue *)Buf;

        OS_ENTER_CRITICAL();
                                                                /* ��ʼ���ṹ������ */
        Queue->MaxData = (SizeOfBuf - (int32u)(((DataQueue *)0)->Buf)) / 
                         sizeof(QUEUE_DATA_TYPE);               /* ������п��Դ洢��������Ŀ */
        Queue->End = Queue->Buf + Queue->MaxData;               /* �������ݻ���Ľ�����ַ */
        Queue->Out = Queue->Buf;
        Queue->In = Queue->Buf;
        Queue->NData = 0;
        Queue->ReadEmpty = ReadEmpty;							/* ���ж��մ����������û����� */
        Queue->WriteFull = WriteFull;							/* ����д�������������û����� */

        OS_EXIT_CRITICAL();

        return QUEUE_OK;
    }
    else
    {
        return NOT_OK;
    }
}	

/*********************************************************************************************************
** ��������: QueueRead
** ��������: ��ȡ�����е�����
** ��  ����: Ret:�洢���ص���Ϣ�ĵ�ַ
**           Buf:ָ����е�ָ��
** ��  ����: NOT_OK     ����������
**           QUEUE_OK   ���յ���Ϣ
**           QUEUE_EMPTY������Ϣ
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/
int8u QueueRead(QUEUE_DATA_TYPE *Ret, void *Buf)
{
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
    int8u err;
    DataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                            /* �����Ƿ���Ч */
    {                                                           /* ��Ч */
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData > 0)                                   /* �����Ƿ�Ϊ�� */
        {                                                       /* ����         */
            *Ret = Queue->Out[0];                               /* ���ݳ���     */
            Queue->Out++;                                       /* ��������ָ�� */
            if (Queue->Out >= Queue->End)
            {
                Queue->Out = Queue->Buf;
            }
            Queue->NData--;                                     /* ���ݼ���      */
            err = QUEUE_OK;
        }
        else
        {                                                       /* ��              */
            err = QUEUE_EMPTY;
            if (Queue->ReadEmpty != NULL)                       /* �����û������� */
            {
                err = Queue->ReadEmpty(Ret, Queue);
            }
        }
        OS_EXIT_CRITICAL();
    }
    return err;
}

/*********************************************************************************************************
** ��������: QueueReadOnly
** ��������: ��ȡ�����е�����,�����ı�ԭ���ݶ���
** ��  ����: Ret:�洢���ص���Ϣ�ĵ�ַ
**           Buf:ָ����е�ָ��
** ��  ����: NOT_OK     ����������
**           QUEUE_OK   ���յ���Ϣ
**           QUEUE_EMPTY������Ϣ
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/
int8u QueueReadOnly(QUEUE_DATA_TYPE *Ret, void *Buf)
{
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
    int8u err;
    DataQueue *Queue;

    err = NOT_OK;
    if (Buf != NULL)                                            /* �����Ƿ���Ч */
    {                                                           /* ��Ч */
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData > 0)                                   /* �����Ƿ�Ϊ�� */
        {                                                       /* ����         */
            *Ret = Queue->Out[0];                               /* ���ݳ���     */
            err = QUEUE_OK;
        }
        else
        {                                                       /* ��              */
            err = QUEUE_EMPTY;
        }
        
        OS_EXIT_CRITICAL();
    }
    return err;
}

/*********************************************************************************************************
** ��������: QueueWrite
** ��������: FIFO��ʽ��������
** ��  ����: Buf :ָ����е�ָ��
**           Data:��Ϣ����
** ��  ����: NOT_OK   :��������
**           QUEUE_FULL:������
**           QUEUE_OK  :���ͳɹ�
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
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
    if (Buf != NULL)                                                    /* �����Ƿ���Ч */
    {
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData < Queue->MaxData)                              /* �����Ƿ���  */
        {                                                               /* ����        */
            Queue->In[0] = Data;                                        /* �������    */
            Queue->In++;                                                /* �������ָ��*/
            if (Queue->In >= Queue->End)
            {
                Queue->In = Queue->Buf;
            }
            Queue->NData++;                                             /* ��������    */
            err = QUEUE_OK;
        }
        else
        {                                                               /* ��           */
            err = QUEUE_FULL;
            if (Queue->WriteFull != NULL)                               /* �����û������� */
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
** ��������: QueueWriteFront
** ��������: LIFO��ʽ��������
** ��  ����: Buf:ָ����е�ָ��
**           Data:��Ϣ����
** �䡡  ��: QUEUE_FULL:������
**           QUEUE_OK:���ͳɹ�
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
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
    if (Buf != NULL)                                                    /* �����Ƿ���Ч */
    {
        Queue = (DataQueue *)Buf;
        
        OS_ENTER_CRITICAL();
        
        if (Queue->NData < Queue->MaxData)                              /* �����Ƿ���  */
        {                                                               /* ���� */
            Queue->Out--;                                               /* ��������ָ�� */
            if (Queue->Out < Queue->Buf)
            {
                Queue->Out = Queue->End - 1;
            }
            Queue->Out[0] = Data;                                       /* �������     */
            Queue->NData++;                                             /* ������Ŀ���� */
            err = QUEUE_OK;
        }
        else
        {                                                               /* ��           */
            err = QUEUE_FULL;
            if (Queue->WriteFull != NULL)                               /* �����û������� */
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
** ��������: QueueNData
** ��������: ȡ�ö�����������
** ��  ����: Buf:ָ����е�ָ��
** ��  ����: ��Ϣ��
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/
#if EN_QUEUE_NDATA > 0					  
int16u QueueNData(void *Buf)
{
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
    int16u temp;
    
    temp = 0;                                                   /* ������Ч����0 */
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
** ��������: QueueSize
** ��������: ȡ�ö���������
** ��  ����: Buf:ָ����е�ָ��
** ��  ����: ����������
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/ 
#if EN_QUEUE_SIZE > 0				   
int16u QueueSize(void *Buf)
{
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
    int16u temp;
    
    temp = 0;                                                   /* ������Ч����0 */
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
** ��������: OSQFlush
** ��������: ��ն���
** �䡡  ��: Buf:ָ����е�ָ��
** ��  ����: ��
** ȫ�ֱ���: ��
** ����ģ��: OS_ENTER_CRITICAL,OS_EXIT_CRITICAL
********************************************************************************************************/
#if EN_QUEUE_FLUSH > 0								 
void QueueFlush(void *Buf)
{
#if OS_CRITICAL_METHOD == 3         
	OS_CPU_SR  cpu_sr;
#endif
    DataQueue *Queue;
    
    if (Buf != NULL)                                                /* �����Ƿ���Ч */
    {                                                               /* ��Ч         */
        Queue = (DataQueue *)Buf;
        OS_ENTER_CRITICAL();
        Queue->Out = Queue->Buf;
        Queue->In = Queue->Buf;
        Queue->NData = 0;                                           /* ������ĿΪ0 */
        OS_EXIT_CRITICAL();
    }
} 
#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
