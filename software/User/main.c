#include "UserConfig.h"
#include "stm32f10x.h" // Device header
#include "string.h"
#include <stdio.h>

/* Hardware includes */
#include "BlueTooth.h"
#include "Button.h"
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
void PrintPicTask(void *parameters);
void WaitSerialRxTask(void *parameters);
void RollPaperTask(void *parameters);

/* Task functions handle definitions */
TaskHandle_t appTaskCreateHandle = NULL;
TaskHandle_t printTaskHandle = NULL;
TaskHandle_t printPicTaskHandle = NULL;
TaskHandle_t waitSerialRxTaskHandle = NULL;
TaskHandle_t rollPaperTaskHandle = NULL;

/* Semaphore handle */
SemaphoreHandle_t serialSendDataMutexHandle = NULL;

/* Queue handle */
QueueHandle_t printMsgQueueHandle = NULL;

/* Other */
uint8_t currentPrintMode[20] = CMD_DEFAULT; // ʹ��һ��ȫ�ֱ�����ά����ǰ��ӡ���Ĵ�ӡģʽ
uint8_t currentRxType[20] = "";             // Serial  ����   Bluetooth  ����

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

// �����ʼ��
void Peripherals_Init(void)
{
    // �����ж���λ��ռ���ȼ�
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); // �ָ�PA15 PB3 PB4ΪGPIO

    LED_Init();
    MySPI_Init();
    Serial_Init(115200);
    PrinterHead_Init(); // �ڲ��Ѿ��� moto �����˳�ʼ��
    W25Q64_Init();      // �ڲ���SPI2�����˳�ʼ��
    Button_Init();
    BlueTooth_Init(9600);
}

void AppTaskCreate(void *parameter)
{
    BaseType_t xReturn = pdPASS;
    taskENTER_CRITICAL();

    serialSendDataMutexHandle = xSemaphoreCreateMutex();
    if (serialSendDataMutexHandle == NULL)
    {
        /* ������ */
    }
    /* ��ʵΪ�˼��� ���Ӵ�ӡ�ַ��������ܹ���ӡͼƬ
     * �����������������������У���Ϊ���߶���Ԫ�ش�С��һ�£�ǰ�����⣬���߹̶�Ϊ384��
     * ����Ϊ�˷���ѡ���˼��ٴ�ӡ�ַ���ʹ��һ������
     */
    printMsgQueueHandle = xQueueCreate(10, MAX_RX_BYTE); // ���10����ӡ������ӡָ���ڶ�����
    if (printMsgQueueHandle == NULL)
    {
        /* ������ */
    }

    xReturn = xTaskCreate(PrintTask, "PrintTask", 384, NULL, 9, &printTaskHandle);
    if (xReturn != pdPASS)
    {
        /* ������ */
    }

    xReturn = xTaskCreate(PrintPicTask, "PrintPicTask", 384, NULL, 9, &printPicTaskHandle);
    if (xReturn != pdPASS)
    {
        /* ������ */
    }

    xReturn = xTaskCreate(WaitSerialRxTask, "WaitSerialRxTask", 384, NULL, 5, &waitSerialRxTaskHandle);
    if (xReturn != pdPASS)
    {
        /* ������ */
    }

    xReturn = xTaskCreate(RollPaperTask, "RollPaperTask", 16, NULL, 3, &rollPaperTaskHandle);
    if (xReturn != pdPASS)
    {
        /* ������ */
    }

    vTaskDelete(appTaskCreateHandle);
    taskEXIT_CRITICAL();
}

void WaitSerialRxTask(void *parameters)
{
    while (1)
    {
        uint8_t tempPeekBuffer[MAX_RX_BYTE];
        if (xQueuePeek(printMsgQueueHandle, &tempPeekBuffer, portMAX_DELAY) == pdTRUE) // peek���������ݳ���
        {
            if (strncmp(tempPeekBuffer, CMD_DEFAULT, strlen(CMD_DEFAULT)) == 0 ||
                strncmp(tempPeekBuffer, CMD_PIC, strlen(CMD_PIC)) == 0)
            {
                // ��ӡָ�� ���ý���ָ��ĺ���
                // ָ��Ӧ�ó��ӣ�����Ҫ�ö���ά��ָ��
                xQueueReceive(printMsgQueueHandle, &tempPeekBuffer, portMAX_DELAY);
                ParsePrinterCommand(tempPeekBuffer);
            }
            else
            {
                // ��ӡ���� ���ݲ�ͬ��ָ��֪ͨ��ӡ������д�ӡ
                if (strncmp(currentPrintMode, CMD_DEFAULT, strlen(CMD_DEFAULT)) == 0)
                {
                    xTaskNotifyGiveIndexed(printTaskHandle, 0);
                }
                else if (strncmp(currentPrintMode, CMD_PIC, strlen(CMD_PIC)) == 0)
                {
                    xTaskNotifyGiveIndexed(printPicTaskHandle, 0);
                }
            }
        }
        vTaskDelay(20);
    }
}

void PrintTask(void *parameters)
{
    while (1)
    {

        LED_OFF();                                         // û�д�ӡ
        ulTaskNotifyTakeIndexed(0, pdTRUE, portMAX_DELAY); // pdTRUE����Ϊ��ֵģʽ
        LED_ON();                                          // ��ӡ�С���

        uint8_t tempBuffer[MAX_RX_BYTE] = {0};

        xQueueReceive(printMsgQueueHandle, &tempBuffer, portMAX_DELAY);

        xSemaphoreTake(serialSendDataMutexHandle, portMAX_DELAY);
        printf("��ȴ������ڴ�ӡ�С��� \r\n");
        xSemaphoreGive(serialSendDataMutexHandle);

        uint8_t i;
        uint16_t j;

        const uint8_t wordHeight = FONTSIZE + 1; // ����һ��

        const uint8_t wordWidth = FONTSIZE;
        const uint8_t wordBitmapSize = wordHeight * wordWidth / 8; // Byte

        const uint8_t wordWidthHalf = wordWidth / 2;
        const uint8_t wordBitmapSizeHalf = wordHeight * wordWidthHalf / 8; // Byte

        uint8_t lineWordBitmap[DOTLINE_SIZE] = {0};

        uint16_t pToLineHead = 0; // ÿ��ѭ���Ŀ�ʼ����ָ�򱾴��е�ͷ��
        // ��ѭ��
        while (tempBuffer[pToLineHead] != '\0')
        {

            for (i = 0; i < wordHeight; i++)
            {
                uint8_t offsetStore = 0;
                uint8_t storedBytesCount = 0;
                // ����ÿ�еĵ���
                for (j = pToLineHead; tempBuffer[j] != '\0';)
                {
                    uint8_t q, w;
                    uint32_t offsetAddress;                                   // flash�еĵ�ַƫ��
                    uint8_t restBytesCount = DOTLINE_SIZE - storedBytesCount; // һ���л�ʣ������ֽ�δȡ
                    // �ж��Ƿ�Ϊ���֣��������λΪ1��ASCll���λ 0111 1111
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
                // ��ӡ
                memcpy(dotLine, lineWordBitmap, DOTLINE_SIZE);
                PrinterHead_PrintDotLine();
                // PrinterHead_PrintDotLineCustom(1, 0x3F, 30, 1, 1);
                memset(lineWordBitmap, 0, sizeof(lineWordBitmap));
            }
            PrinterHead_PrintLineSpace();
            pToLineHead = j;
        }
        PrinterHead_PrintSegmentSpace();

        xSemaphoreTake(serialSendDataMutexHandle, portMAX_DELAY);
        printf("�ɹ���ӡ print successfully \r\n");
        xSemaphoreGive(serialSendDataMutexHandle);

        vTaskDelay(20);
    }
}

void PrintPicTask(void *parameters)
{
    while (1)
    {
        LED_OFF();                                         // û�д�ӡ
        ulTaskNotifyTakeIndexed(0, pdTRUE, portMAX_DELAY); // pdTRUE����Ϊ��ֵģʽ
        LED_ON();                                          // ��ӡ�С���

        static uint16_t lineCount = 0;

        uint8_t tempBuffer[MAX_RX_BYTE] = {0};
        uint16_t i;
        uint8_t j;

        xQueueReceive(printMsgQueueHandle, &tempBuffer, portMAX_DELAY);

        xSemaphoreTake(serialSendDataMutexHandle, portMAX_DELAY);
        printf("���Եȣ����ڴ�ӡͼƬ���� \r\n");
        xSemaphoreGive(serialSendDataMutexHandle);

        for (j = 1; j <= 8; j++)
        {
            uint16_t p = 0;
            for (i = 0; i < MAX_RX_BYTE; i++)
            {
                if (tempBuffer[i] >= ((j - 1) * 0x20) && tempBuffer[i] < (j * 0x20))
                {
                    uint8_t n = i / 8;
                    uint8_t m = i % 8;
                    dotLine[n] |= (0x80 >> m);
                }
            }
            PrinterHead_PrintDotLineCustom(1, 0x3F, (8 - j + 1) * 0.625, 1, 0); // ����ʱ�� 0.625 ~ 5 (7V) 5 ~ 40 (5V)
        }
        PrinterHead_RunDotLine();

        xSemaphoreTake(serialSendDataMutexHandle, portMAX_DELAY);
        printf("һ���д�ӡ���\r\n");
        xSemaphoreGive(serialSendDataMutexHandle);

        lineCount++;
        // ͼƬ��ӡ����Զ��л���Ĭ��ģʽ
        if (lineCount >= MAX_RX_BYTE) // ����������Ŀǰ�ȹ̶�������һ��
        {
            lineCount = 0;
            PrinterHead_PrintSegmentSpace();

            memcpy(currentPrintMode, CMD_DEFAULT, strlen(CMD_DEFAULT)); // ����Ϊ���µĴ�ӡģʽ

            xSemaphoreTake(serialSendDataMutexHandle, portMAX_DELAY);
            printf("ͼƬ��ӡ��ɡ�\r\n�Զ��л���ģʽ��%s \r\n", currentPrintMode);
            xSemaphoreGive(serialSendDataMutexHandle);
        }
        if (strncmp(currentRxType, RXTYPE_SERIAL, strlen(RXTYPE_SERIAL)) == 0)
        {
            Serial_SendByte(0xFF); // ֪ͨ���Ͷ��Ѿ������һ���еĴ�ӡ
        }
        else if (strncmp(currentRxType, RXTYPE_BLUETOOTH, strlen(RXTYPE_BLUETOOTH)) == 0)
        {
            BlueTooth_SendByte(0xFF);
        }
        vTaskDelay(20);
    }
}

void RollPaperTask(void *parameters)
{
    while (1)
    {
        ulTaskNotifyTakeIndexed(0, pdTRUE, portMAX_DELAY);

        xSemaphoreTake(serialSendDataMutexHandle, portMAX_DELAY);
        printf("��ֽ�С��� \r\n");
        xSemaphoreGive(serialSendDataMutexHandle);

        for (uint8_t i = 0; i < 8; i++)
        {
            PrinterHead_PrintSegmentSpace();
        }

        xSemaphoreTake(serialSendDataMutexHandle, portMAX_DELAY);
        printf("��ֽ��� \r\n");
        xSemaphoreGive(serialSendDataMutexHandle);

        vTaskDelay(20);
    }
}

void ParsePrinterCommand(uint8_t *command)
{
    memset(currentPrintMode, 0, sizeof(currentPrintMode));
    memcpy(currentPrintMode, command, strlen(command)); // ����Ϊ���µĴ�ӡģʽ

    xSemaphoreTake(serialSendDataMutexHandle, portMAX_DELAY);
    printf("�Ѿ��л���ģʽ��%s \r\n", currentPrintMode);
    xSemaphoreGive(serialSendDataMutexHandle);

    if (strncmp(command, CMD_DEFAULT, strlen(CMD_DEFAULT)) == 0)
    {
        // Ӣ�Ĵ�ӡģʽ����
    }
    else if (strncmp(command, CMD_PIC, strlen(CMD_PIC)) == 0)
    {
        // ͼƬ��ӡģʽ����
    }
}
