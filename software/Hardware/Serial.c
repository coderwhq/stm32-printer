#include "stm32f10x.h"
#include <stdarg.h>
#include <stdio.h>

#define SERIAL_USART_X USART2

#define SERIAL_RCC RCC_APB1Periph_USART2
#define SERIAL_RCC_GPIO RCC_APB2Periph_GPIOA

#define SERIAL_PINS_PORT GPIOA
#define SERIAL_TX_PIN GPIO_Pin_2
#define SERIAL_RX_PIN GPIO_Pin_3

char serialReceiveString[100];
uint8_t serialReceiveFlag;

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

uint8_t Serial_GetReceiveFlag(void)
{
    uint8_t tempFlag = 0;

    if (serialReceiveFlag == 1)
    {
        serialReceiveFlag = 0;
        tempFlag = 1;
    }

    return tempFlag;
}

void USART2_IRQHandler(void)
{
    static uint8_t receiveState = 0;
    static uint8_t receiveDataIdx = 0;
    if (USART_GetITStatus(SERIAL_USART_X, USART_IT_RXNE) == SET)
    {
        uint8_t receiveByte = USART_ReceiveData(SERIAL_USART_X);
        if (receiveState == 0 && serialReceiveFlag == 0)
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
                serialReceiveString[receiveDataIdx++] = receiveByte;
            }
        }
        else if (receiveState == 2)
        {
            if (receiveByte == '\n')
            {
                serialReceiveString[receiveDataIdx] = '\0';
                receiveState = 0;
                serialReceiveFlag = 1;
            }
        }
        USART_ClearITPendingBit(SERIAL_USART_X, USART_IT_RXNE);
    }
}
