#ifndef __SEMAPHORE_MANG_H
#define __SEMAPHORE_MANG_H
#include "FreeRTOS.h"

void AppSemaphoreCreate(void);

void SerialSendDataMutexTake(TickType_t blockTime);
void SerialSendDataMutexRelease(void);

#endif
