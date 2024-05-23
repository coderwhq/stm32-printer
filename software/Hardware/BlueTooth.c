#include "FreeRTOS.h"
#include "UserConfig.h"
#include "queue.h"
#include "stm32f10x.h"

#include "task.h"
#include <stdarg.h>
#include <stdio.h>

uint8_t BTRxBuffer[MAX_RX_BYTE] = "";

void BlueTooth_Init(uint32_t baudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(BLUE_TOOTH_RCC_GPIO, ENABLE);
    RCC_APB1PeriphClockCmd(BLUE_TOOTH_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = BLUE_TOOTH_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(BLUE_TOOTH_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = BLUE_TOOTH_RX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(BLUE_TOOTH_PORT, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = baudRate;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;

    USART_Init(BLUE_TOOTH_USART_X, &USART_InitStructure);

    USART_ITConfig(BLUE_TOOTH_USART_X, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    USART_Cmd(BLUE_TOOTH_USART_X, ENABLE);
}

void BlueTooth_SendByte(uint8_t byte)
{
    USART_SendData(BLUE_TOOTH_USART_X, byte); /* 会将TXE为置1 */
    while (USART_GetFlagStatus(BLUE_TOOTH_USART_X, USART_FLAG_TXE) != SET)
    {
    }
}

void BlueTooth_SendArray(uint8_t *arr, uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        BlueTooth_SendByte(arr[i]);
    }
}

void BlueTooth_SendString(char *str)
{
    uint8_t i;
    for (i = 0; str[i] != '\0'; i++)
    {
        BlueTooth_SendByte((uint8_t)str[i]);
    }
}

void USART3_IRQHandler(void)
{
    static uint8_t receiveBTState = 0;
    static uint16_t receiveBTDataIdx = 0;

    if (USART_GetITStatus(BLUE_TOOTH_USART_X, USART_IT_RXNE) == SET)
    {
        extern uint8_t currentPrintMode[];
        if (strncmp(currentPrintMode, CMD_DEFAULT, strlen(CMD_DEFAULT)) == 0)
        {
            uint8_t receiveBTByte = USART_ReceiveData(BLUE_TOOTH_USART_X);
            if (receiveBTState == 0)
            {
                if (receiveBTByte == '@')
                {
                    receiveBTState = 1;
                    receiveBTDataIdx = 0;
                }
            }
            else if (receiveBTState == 1)
            {
                if (receiveBTByte == '\r')
                {
                    receiveBTState = 2;
                }
                else
                {
                    BTRxBuffer[receiveBTDataIdx++] = receiveBTByte;
                }
            }
            else if (receiveBTState == 2)
            {
                if (receiveBTByte == '\n')
                {
                    extern QueueHandle_t printMsgQueueHandle;
                    BaseType_t pxHigherPriorityTaskWoken = 0;

                    BTRxBuffer[receiveBTDataIdx] = '\0';
                    receiveBTState = 0;
                    receiveBTDataIdx = 0;

                    xQueueSendFromISR(printMsgQueueHandle, &BTRxBuffer, &pxHigherPriorityTaskWoken);
                    // pxHigherPriorityTaskWoken如果被设置为true，那么需要进行上下文切换，确保直接返回到最高优先级的任务
                    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
                }
            }
        }
        else if (strncmp(currentPrintMode, CMD_PIC, strlen(CMD_PIC)) == 0)
        {
            uint8_t receiveByte = USART_ReceiveData(BLUE_TOOTH_USART_X);

            BTRxBuffer[receiveBTDataIdx++] = receiveByte;

            if (receiveBTDataIdx >= MAX_RX_BYTE)
            {
                extern QueueHandle_t printMsgQueueHandle;
                extern uint8_t currentRxType[];
                BaseType_t pxHigherPriorityTaskWoken = 0;

                memcpy(currentRxType, RXTYPE_BLUETOOTH, strlen(RXTYPE_BLUETOOTH));

                receiveBTDataIdx = 0;

                xQueueSendFromISR(printMsgQueueHandle, &BTRxBuffer, &pxHigherPriorityTaskWoken);
                portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
            }
        }
        USART_ClearITPendingBit(BLUE_TOOTH_USART_X, USART_IT_RXNE);
    }
}
