#include "UserConfig.h"
#include "stm32f10x.h" // Device header
#include "string.h"
#include <stdio.h>

/* Hardware includes */
#include "LED.h"
#include "MySPI.h"
#include "PrinterHead.h"
#include "Serial.h"
#include "W25Q64.h"

/* System includes */

/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

/* Functions definitions */
void Peripherals_Init(void);
void ParsePrinterCommand(uint8_t *command);

/* Task functions definitions */
void AppTaskCreate(void *parameter);
void PrintTask(void *parameters);
void WaitSerialRxTask(void *parameters);

/* Task functions handle definitions */
TaskHandle_t appTaskCreateHandle = NULL;
TaskHandle_t printTaskHandle = NULL;
TaskHandle_t waitSerialRxTaskHandle = NULL;

/* Semaphore handle */
SemaphoreHandle_t serialSendDataMutexHandle = NULL;

/* Queue handle */
QueueHandle_t printMsgQueueHandle = NULL;

/* Other */
uint8_t currentPrintMode[100] = CMD_DEFAULT; // 使用一个全局变量来维护当前打印机的打印模式

int main(void)
{
    BaseType_t xReturn = pdPASS;

    Peripherals_Init();

    printf("Peripherals Init finished. \r\n");

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

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // 恢复PA15 PB3 PB4为GPIO

    LED_Init();
    MySPI_Init();
    Serial_Init(9600);
    PrinterHead_Init(); // 内部已经对 moto 进行了初始化
    W25Q64_Init();      // 内部对SPI2进行了初始化
}

void AppTaskCreate(void *parameter)
{
    BaseType_t xReturn = pdPASS;
    taskENTER_CRITICAL();

    serialSendDataMutexHandle = xSemaphoreCreateMutex();
    if (serialSendDataMutexHandle == NULL)
    {
        /* 错误处理 */
    }
    printMsgQueueHandle = xQueueCreate(10, MAX_PRINT_BYTE); // 最多10个打印任务或打印指令在队列中
    if (printMsgQueueHandle == NULL)
    {
        /* 错误处理 */
    }

    xReturn = xTaskCreate(PrintTask, "PrintTask", 512, NULL, 9, &printTaskHandle);
    if (xReturn != pdPASS)
    {
        /* 错误处理 */
    }

    xReturn = xTaskCreate(WaitSerialRxTask, "WaitSerialRxTask", 512, NULL, 99, &waitSerialRxTaskHandle);
    if (xReturn != pdPASS)
    {
        /* 错误处理 */
    }

    vTaskDelete(appTaskCreateHandle);
    taskEXIT_CRITICAL();
}

void WaitSerialRxTask(void *parameters)
{
    while (1)
    {
        uint8_t tempPeekBuffer[MAX_PRINT_BYTE];
        if (xQueuePeek(printMsgQueueHandle, &tempPeekBuffer, portMAX_DELAY) == pdTRUE) // peek不会让数据出队
        {
            if (strncmp(tempPeekBuffer, CMD_DEFAULT, strlen(CMD_DEFAULT)) == 0 ||
                strncmp(tempPeekBuffer, CMD_PIC, strlen(CMD_PIC)) == 0)
            {
                // 打印指令 调用解析指令的函数
                // 指令应该出队，不需要用队列维护指令
                xQueueReceive(printMsgQueueHandle, &tempPeekBuffer, portMAX_DELAY);
                ParsePrinterCommand(tempPeekBuffer);
            }
            else
            {
                // 打印数据 通知打印任务进行打印
                xTaskNotifyGiveIndexed(printTaskHandle, 0);
            }
        }
        vTaskDelay(20);
    }
}

void PrintTask(void *parameters)
{
    while (1)
    {

        LED_OFF();                                         // 没有打印
        ulTaskNotifyTakeIndexed(0, pdTRUE, portMAX_DELAY); // pdTRUE设置为二值模式
        LED_ON();                                          // 打印中……

        uint8_t tempBuffer[MAX_PRINT_BYTE] = {0};

        xQueueReceive(printMsgQueueHandle, &tempBuffer, portMAX_DELAY);

        xSemaphoreTake(serialSendDataMutexHandle, portMAX_DELAY);
        printf("请等待，正在打印中…… \r\n");
        xSemaphoreGive(serialSendDataMutexHandle);

        uint8_t i;
        uint16_t j;

        const uint8_t wordHeight = FONTSIZE + 1; // 多了一行

        const uint8_t wordWidth = FONTSIZE;
        const uint8_t wordBitmapSize = wordHeight * wordWidth / 8; // Byte

        const uint8_t wordWidthHalf = wordWidth / 2;
        const uint8_t wordBitmapSizeHalf = wordHeight * wordWidthHalf / 8; // Byte

        uint8_t lineWordBitmap[DOTLINE_SIZE] = {0};

        uint16_t pToLineHead = 0; // 每次循环的开始总是指向本次行的头部
        // 行循环
        while (tempBuffer[pToLineHead] != '\0')
        {

            for (i = 0; i < wordHeight; i++)
            {
                uint8_t offsetStore = 0;
                uint8_t storedBytesCount = 0;
                // 处理每行的点阵
                for (j = pToLineHead; tempBuffer[j] != '\0';)
                {
                    uint8_t q, w;
                    uint32_t offsetAddress;                                   // flash中的地址偏移
                    uint8_t restBytesCount = DOTLINE_SIZE - storedBytesCount; // 一行中还剩余多少字节未取
                    // 判断是否为汉字，汉字最高位为1，ASCll最高位 0111 1111
                    if (tempBuffer[j] & 0x80)
                    {
                        if (restBytesCount < wordWidth / 8)
                        {
                            break;
                        }
                        q = tempBuffer[j] - 0xA0;
                        w = tempBuffer[j + 1] - 0xA0;
                        offsetAddress = (94 * (q - 1) + (w - 1)) * wordBitmapSize;

                        W25Q64_ReadData(offsetAddress + (wordWidth / 8 * i), lineWordBitmap + offsetStore,
                                        wordWidth / 8);

                        offsetStore += wordWidth / 8;
                        storedBytesCount += wordWidth / 8;
                        j += 2;
                    }
                    else
                    {
                        if (restBytesCount < wordWidthHalf / 8)
                        {
                            break;
                        }
                        q = tempBuffer[j];
                        offsetAddress = ASCII + (q - 0x20) * wordBitmapSizeHalf;

                        W25Q64_ReadData(offsetAddress + (wordWidthHalf / 8 * i), lineWordBitmap + offsetStore,
                                        wordWidthHalf / 8);

                        offsetStore += wordWidthHalf / 8;
                        storedBytesCount += wordWidthHalf / 8;
                        j += 1;
                    }
                }
                // 打印
                memcpy(dotLine, lineWordBitmap, DOTLINE_SIZE);
                PrinterHead_PrintDotLine();
                memset(lineWordBitmap, 0, sizeof(lineWordBitmap));
            }
            PrinterHead_PrintLineSpace();
            pToLineHead = j;
        }
        PrinterHead_PrintSegmentSpace();

        xSemaphoreTake(serialSendDataMutexHandle, portMAX_DELAY);
        printf("成功打印 print successfully \r\n");
        xSemaphoreGive(serialSendDataMutexHandle);

        vTaskDelay(20);
    }
}

void ParsePrinterCommand(uint8_t *command)
{
    memcpy(currentPrintMode, command, strlen(command)); // 更改为最新的打印模式

    xSemaphoreTake(serialSendDataMutexHandle, portMAX_DELAY);
    printf("已经切换至模式：%s \r\n", currentPrintMode);
    xSemaphoreGive(serialSendDataMutexHandle);

    if (strncmp(command, CMD_DEFAULT, strlen(CMD_DEFAULT)) == 0)
    {
        // 英文打印模式配置
    }
    else if (strncmp(command, CMD_PIC, strlen(CMD_PIC)) == 0)
    {
        // 图片打印模式配置
    }
}
