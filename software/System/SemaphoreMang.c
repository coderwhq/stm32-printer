#include "FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t serialSendDataMutexHandle = NULL;

void AppSemaphoreCreate(void)
{
    serialSendDataMutexHandle = xSemaphoreCreateMutex();
    if (serialSendDataMutexHandle == NULL)
    {
        /* 错误处理 */
    }
}
/* 使用串口发送数据时的互锁和解锁函数，临界资源保护 */
void SerialSendDataMutexTake(TickType_t blockTime)
{
    xSemaphoreTake(serialSendDataMutexHandle, blockTime);
}
void SerialSendDataMutexRelease(void)
{
    xSemaphoreGive(serialSendDataMutexHandle);
}
