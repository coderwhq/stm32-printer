#include "FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t serialSendDataMutexHandle = NULL;

void AppSemaphoreCreate(void)
{
    serialSendDataMutexHandle = xSemaphoreCreateMutex();
    if (serialSendDataMutexHandle == NULL)
    {
        /* ������ */
    }
}
/* ʹ�ô��ڷ�������ʱ�Ļ����ͽ����������ٽ���Դ���� */
void SerialSendDataMutexTake(TickType_t blockTime)
{
    xSemaphoreTake(serialSendDataMutexHandle, blockTime);
}
void SerialSendDataMutexRelease(void)
{
    xSemaphoreGive(serialSendDataMutexHandle);
}
