#include "stm32f10x.h" // Device header

// Hardware includes
#include "A4988.h"
#include "Delay.h"
#include "LED.h"
#include "MySPI.h"
#include "PrinterHead.h"

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

int main(void)
{
    Peripherals_Init();

    while (1)
    {
        LED_Switch();
        Delay_ms(1000);
    }
}
