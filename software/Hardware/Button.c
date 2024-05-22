#include "FreeRTOS.h"
#include "UserConfig.h"
#include "stdio.h"
#include "stm32f10x.h"
#include "task.h"

void Button_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_APB2PeriphClockCmd(BUTTON_RCC_GPIO, ENABLE);
    RCC_APB2PeriphClockCmd(BUTTON_RCC_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = BUTTON_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);

    GPIO_EXTILineConfig(BUTTON_EXTI_AFIO_PORT, BUTTON_EXTI_AFIO_LINE);

    EXTI_InitStruct.EXTI_Line = BUTTON_EXTI_LINE;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;

    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = BUTTON_EXTI_CHANNEL;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 10;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;

    NVIC_Init(&NVIC_InitStruct);
}

void BUTTON_EXTI_FUNCTION(void)
{
    if (EXTI_GetITStatus(BUTTON_EXTI_LINE) == SET)
    {
        BaseType_t pxHigherPriorityTaskWoken = 0;
        extern TaskHandle_t rollPaperTaskHandle;

        vTaskNotifyGiveIndexedFromISR(rollPaperTaskHandle, 0, &pxHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
        EXTI_ClearITPendingBit(BUTTON_EXTI_LINE);
    }
}
