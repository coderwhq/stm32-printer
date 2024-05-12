#include "stm32f10x.h" // Device header
#include <stdio.h>

/* Hardware includes */
#include "A4988.h"
#include "FontBitmap.h"
#include "LED.h"
#include "MySPI.h"

// #include "PWM.h"
#include "PrinterHead.h"
#include "PrinterMoto.h"
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

/* Task functions handle definitions */
TaskHandle_t appTaskCreateHandle = NULL;
TaskHandle_t testLEDFlashTaskHandle = NULL;

int main(void)
{
    BaseType_t xReturn = pdPASS;

    uint8_t i, j;
    char testString[] = " Hello World ! ";
    Peripherals_Init();
    printf("Peripherals Init finished. \r\n");

    for (i = 0; i < 16; i++)
    {
        for (j = 0; testString[j] != '\0'; j++)
        {
            dotLine[j] = fontBitmap_8x16[testString[j] - ' '][i];
        }

        PrinterHead_PrintDotLine();
    }
    PrinterMoto_Run_Circle(128);

    // xReturn = xTaskCreate(AppTaskCreate, "AppTaskCreate", 128, NULL, 1, &appTaskCreateHandle);
    // if (xReturn == pdPASS)
    // {
    //     vTaskStartScheduler();
    // }

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
    // A4988_Init();
    Serial_Init(9600);
    PrinterHead_Init();
    PrinterMoto_Init();
}

void AppTaskCreate(void *parameter)
{
    BaseType_t xReturn = pdPASS;
    taskENTER_CRITICAL();

    AppSemaphoreCreate(); /* 创建所有要用的信号量 */

    PrinterMoto_Run_Circle(4000);

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
