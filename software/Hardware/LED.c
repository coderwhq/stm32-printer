#include "stm32f10x.h"

// 如果没有高电平使能的宏定义
#ifndef ACTIVE_HIGH_LEVEL
// 那么默认使用低电平使能
#define LED_ON_LEVEL (BitAction)0
#define LED_OFF_LEVEL (BitAction)1

#else

#define LED_ON_LEVEL (BitAction)1
#define LED_OFF_LEVEL (BitAction)0

#endif

#define LED_RCC_GPIO RCC_APB2Periph_GPIOB
#define LED_PORT GPIOB
#define LED_PIN GPIO_Pin_0

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(LED_RCC_GPIO, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LED_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(LED_PORT, &GPIO_InitStructure);

    // 默认关闭
    GPIO_WriteBit(LED_PORT, LED_PIN, LED_OFF_LEVEL);
}

void LED_ON(void)
{
    GPIO_WriteBit(LED_PORT, LED_PIN, LED_ON_LEVEL);
}

void LED_OFF(void)
{
    GPIO_WriteBit(LED_PORT, LED_PIN, LED_OFF_LEVEL);
}

void LED_Switch(void)
{
    if (GPIO_ReadOutputDataBit(LED_PORT, LED_PIN) == LED_ON_LEVEL)
    {
        GPIO_WriteBit(LED_PORT, LED_PIN, LED_OFF_LEVEL);
    }
    else
    {
        GPIO_WriteBit(LED_PORT, LED_PIN, LED_ON_LEVEL);
    }
}
