#include "stm32f10x.h" // Device header

/* Hardware includes */
#include "A4988.h"
#include "LED.h"
#include "MySPI.h"
#include "PrinterHead.h"

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"

/* Functions definitions */
void Peripherals_Init(void);

/* Task functions definitions */
void AppTaskCreate(void *parameter);
void TestLED_Task(void *parameter);

/* Task functions handle definitions */
TaskHandle_t appTaskCreateHandle = NULL;
TaskHandle_t testLEDFlashTaskHandle = NULL;

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
}

void AppTaskCreate(void *parameter)
{
    BaseType_t xReturn = pdPASS;
    taskENTER_CRITICAL();

    xReturn = xTaskCreate(TestLED_Task, "TestLED_Task", 128, NULL, 2, &testLEDFlashTaskHandle);
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
