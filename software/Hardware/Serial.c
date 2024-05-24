#include "FreeRTOS.h"
#include "UserConfig.h"
#include "queue.h"
#include "stm32f10x.h"
#include "task.h"
#include <stdarg.h>
#include <stdio.h>

// #define SERIAL_BUFFER_SIZE DOTLINE_SIZE_PIXELS
// uint8_t serialRxBuffer[SERIAL_BUFFER_SIZE];

uint8_t serialRxBuffer[MAX_RX_BYTE] = "";

/**
 * @brief Serial initialization
 *
 * @param baudRate be assigned to USART_BaudRate
 */
void Serial_Init(uint32_t baudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = SERIAL_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(SERIAL_PINS_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = SERIAL_RX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(SERIAL_PINS_PORT, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudRate;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;

    USART_Init(SERIAL_USART_X, &USART_InitStructure);

    USART_ITConfig(SERIAL_USART_X, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(SERIAL_USART_X, ENABLE);
}

void Serial_SendByte(uint8_t byte)
{
    USART_SendData(SERIAL_USART_X, byte); /* 会将TXE为置1 */
    while (USART_GetFlagStatus(SERIAL_USART_X, USART_FLAG_TXE) != SET)
    {
    }
}

void Serial_SendArray(uint8_t *arr, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        Serial_SendByte(arr[i]);
    }
}

void Serial_SendString(char *str)
{
    uint8_t i;
    for (i = 0; str[i] != '\0'; i++)
    {
        Serial_SendByte((uint8_t)str[i]);
    }
}
/* 重写printf内调用的fputc函数 */
int fputc(int ch, FILE *f)
{
    Serial_SendByte(ch);
    return ch;
}

void USART2_IRQHandler(void)
{
    static uint8_t receiveState = 0;
    static uint16_t receiveDataIdx = 0;

    if (USART_GetITStatus(SERIAL_USART_X, USART_IT_RXNE) == SET)
    {

        extern uint8_t currentPrintMode[];
        if (strncmp(currentPrintMode, CMD_DEFAULT, strlen(CMD_DEFAULT)) == 0)
        {
            uint8_t receiveByte = USART_ReceiveData(SERIAL_USART_X);
            if (receiveState == 0)
            {
                if (receiveByte == '@')
                {
                    receiveState = 1;
                    receiveDataIdx = 0;
                }
            }
            else if (receiveState == 1)
            {
                if (receiveByte == '\r')
                {
                    receiveState = 2;
                }
                else
                {
                    serialRxBuffer[receiveDataIdx++] = receiveByte;
                }
            }
            else if (receiveState == 2)
            {
                if (receiveByte == '\n')
                {
                    extern QueueHandle_t printMsgQueueHandle;
                    BaseType_t pxHigherPriorityTaskWoken = 0;

                    serialRxBuffer[receiveDataIdx] = '\0';
                    receiveState = 0;
                    receiveDataIdx = 0;

                    xQueueSendFromISR(printMsgQueueHandle, &serialRxBuffer, &pxHigherPriorityTaskWoken);
                    // pxHigherPriorityTaskWoken如果被设置为true，那么需要进行上下文切换，确保直接返回到最高优先级的任务
                    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
                }
            }
        }
        else if (strncmp(currentPrintMode, CMD_PIC, strlen(CMD_PIC)) == 0)
        {
            uint8_t receiveByte = USART_ReceiveData(SERIAL_USART_X);

            serialRxBuffer[receiveDataIdx++] = receiveByte;

            if (receiveDataIdx >= MAX_RX_BYTE)
            {
                extern QueueHandle_t printMsgQueueHandle;
                extern uint8_t currentRxType[];
                BaseType_t pxHigherPriorityTaskWoken = 0;

                memcpy(currentRxType, RXTYPE_SERIAL, strlen(RXTYPE_SERIAL));

                receiveDataIdx = 0;

                xQueueSendFromISR(printMsgQueueHandle, &serialRxBuffer, &pxHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            }
        }
        USART_ClearITPendingBit(SERIAL_USART_X, USART_IT_RXNE);
    }
}
