#include "FreeRTOS.h"
#include "UserConfig.h"
#include "serial.h"
#include "stm32f10x.h"
#include "task.h"

/*
    {A, B, A-, B-}
*/
const uint8_t motorTable[][4] = {
    {1, 0, 0, 0}, {1, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 1, 1}, {0, 0, 1, 0}, {0, 1, 1, 0}, {0, 1, 0, 0}, {1, 1, 0, 0},
};

void PrinterMoto_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(PRINTER_MOTO_RCC_GPIO, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = PRINTER_A2 | PRINTER_A1 | PRINTER_B2 | PRINTER_B1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_Init(PRINTER_MOTO_PORT, &GPIO_InitStructure);
    // 默认关闭
    GPIO_WriteBit(PRINTER_MOTO_PORT, PRINTER_A1, (BitAction)0); /* A */
    GPIO_WriteBit(PRINTER_MOTO_PORT, PRINTER_B1, (BitAction)0); /* B */
    GPIO_WriteBit(PRINTER_MOTO_PORT, PRINTER_A2, (BitAction)0); /* A- */
    GPIO_WriteBit(PRINTER_MOTO_PORT, PRINTER_B2, (BitAction)0); /* B- */
}

void PrinterMoto_Run(uint8_t motor)
{
    GPIO_WriteBit(PRINTER_MOTO_PORT, PRINTER_A1, (BitAction)motorTable[motor][0]); /* A */
    GPIO_WriteBit(PRINTER_MOTO_PORT, PRINTER_B1, (BitAction)motorTable[motor][1]); /* B */
    GPIO_WriteBit(PRINTER_MOTO_PORT, PRINTER_A2, (BitAction)motorTable[motor][2]); /* A- */
    GPIO_WriteBit(PRINTER_MOTO_PORT, PRINTER_B2, (BitAction)motorTable[motor][3]); /* B- */
}

void PrinterMoto_Stop(void)
{
    GPIO_WriteBit(PRINTER_MOTO_PORT, PRINTER_A1, (BitAction)0); /* A */
    GPIO_WriteBit(PRINTER_MOTO_PORT, PRINTER_B1, (BitAction)0); /* B */
    GPIO_WriteBit(PRINTER_MOTO_PORT, PRINTER_A2, (BitAction)0); /* A- */
    GPIO_WriteBit(PRINTER_MOTO_PORT, PRINTER_B2, (BitAction)0); /* B- */
}

void PrinterMoto_Run_Circle(uint16_t times)
{
    static uint8_t motor = 0; // 为保证下次进入函数时motor和上次退出函数的值一样，所以使用静态变量
    uint16_t i;

    for (i = 0; i < times; i++)
    {
        PrinterMoto_Run(motor);
        vTaskDelay(PRINTER_MOTO_DELAY / 1000); // 1s / 4ms = 250   250Hz
        motor++;
        if (motor >= 8)
        {
            motor = 0;
        }
    }
    PrinterMoto_Stop();
}
