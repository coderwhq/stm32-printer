#include "stm32f10x.h" // Device header
#include <stdio.h>
/* Hardware includes */
#include "A4988.h"
#include "LED.h"
#include "MySPI.h"
#include "PrinterHead.h"
#include "Serial.h"

/* System includes */
#include "SemaphoreMang.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* Functions definitions */
void Peripherals_Init(void);

/* Task functions definitions */
void AppTaskCreate(void *parameter);
void TestLED_Task(void *parameter);
void TestSerial_Task1(void *parameters);
void TestSerial_Task2(void *parameters);

/* Task functions handle definitions */
TaskHandle_t appTaskCreateHandle = NULL;
TaskHandle_t testLEDFlashTaskHandle = NULL;
TaskHandle_t testSerialTask1Handle = NULL;
TaskHandle_t testSerialTask2Handle = NULL;

int main(void)
{
    BaseType_t xReturn = pdPASS;
    Peripherals_Init();

    xReturn = xTaskCreate(AppTaskCreate, "AppTaskCreate", 128, NULL, 1, &appTaskCreateHandle);

    if (xReturn == pdPASS)
    {
        vTaskStartScheduler();
    }

    while (1)
    {
    }
}

// 外设初始化
void Peripherals_Init(void)
{
    // 设置中断四位抢占优先级
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    LED_Init();
    MySPI_Init();
    PrinterHead_Init();
    A4988_Init();
    Serial_Init(9600);
}

void AppTaskCreate(void *parameter)
{
    BaseType_t xReturn = pdPASS;
    taskENTER_CRITICAL();

    AppSemaphoreCreate(); /* 创建所有要用的信号量 */

    xReturn = xTaskCreate(TestLED_Task, "TestLED_Task", 128, NULL, 2, &testLEDFlashTaskHandle);
    if (xReturn != pdPASS)
    {
        /* 错误处理 */
    }
    xReturn = xTaskCreate(TestSerial_Task1, "TestSerial_Task1", 128, NULL, 2, &testSerialTask1Handle);
    if (xReturn != pdPASS)
    {
        /* 错误处理 */
    }
    xReturn = xTaskCreate(TestSerial_Task2, "TestSerial_Task2", 128, NULL, 2, &testSerialTask2Handle);
    if (xReturn != pdPASS)
    {
        /* 错误处理 */
    }

    vTaskDelete(appTaskCreateHandle);
    taskEXIT_CRITICAL();
}

void TestLED_Task(void *parameters)
{
    while (1)
    {
        LED_Switch();
        vTaskDelay(500);
    }
}

void TestSerial_Task1(void *parameters)
{
    while (1)
    {
        SerialSendDataMutexTake(portMAX_DELAY);
        printf("Serial Task1 test \r\n");
        SerialSendDataMutexRelease();
        vTaskDelay(20);
    }
}

void TestSerial_Task2(void *parameters)
{
    while (1)
    {
        SerialSendDataMutexTake(portMAX_DELAY);
        printf("Serial Task2 test \r\n");
        SerialSendDataMutexRelease();
        vTaskDelay(20);
    }
}
