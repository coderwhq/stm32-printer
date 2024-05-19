#include "FreeRTOS.h"
#include "MySPI.h"
#include "PrinterMoto.h"
#include "UserConfig.h"
#include "stm32f10x.h"
#include "task.h"

/* JX-2R-01 */

uint8_t dotLine[DOTLINE_SIZE] = {0};

void PrinterHead_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(PRINTER_LAT_RCC_GPIO, ENABLE);
    RCC_APB2PeriphClockCmd(PRINTER_STB_RCC_GPIO, ENABLE);

    // 初始化 LAT 数据锁存寄存器
    GPIO_InitStructure.GPIO_Pin = PRINTER_LAT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_Init(PRINTER_LAT_PORT, &GPIO_InitStructure);
    // 默认不启用锁存寄存器，高电平关闭
    GPIO_SetBits(PRINTER_LAT_PORT, PRINTER_LAT_PIN);

    // 初始化 选通脉冲引脚
    GPIO_InitStructure.GPIO_Pin = PRINTER_STB_PINS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_Init(PRINTER_STB_PORT, &GPIO_InitStructure);
    // 默认不启用脉冲，低电平关闭
    GPIO_ResetBits(PRINTER_STB_PORT, PRINTER_STB_PINS);

    PrinterMoto_Init(); // 打印头moto初始化
}
/*
    每行384个点，每个STB控制64个点。
    每行可以打印 384 / 8 = 48 个字节（字符）
    每个STB可以控制 64 / 8 = 8 个字节（字符）

    当经过384个CLK时钟，也就是 384 / 8 = 48次SPI字节交换后，一点行数据传输完毕，目前存放在移位寄存器
    然后，拉低LAT一段时间（查看手册），将数据锁存到锁存器中
    然后拉高STB一段时间进行加热，这样一点行的内容就打印完了（当STB位打开时，在根据每个点位的1或0选择该点是否加热）
*/

void PrinterHead_LAT_Enable(void)
{
    GPIO_ResetBits(PRINTER_LAT_PORT, PRINTER_LAT_PIN);
}

void PrinterHead_LAT_Disable(void)
{
    GPIO_SetBits(PRINTER_LAT_PORT, PRINTER_LAT_PIN);
}

void PrinterHead_Heat_Enable(void)
{
    GPIO_SetBits(PRINTER_STB_PORT, PRINTER_STB_PINS);
}

void PrinterHead_Heat_Disable(void)
{
    GPIO_ResetBits(PRINTER_STB_PORT, PRINTER_STB_PINS);
}

void PrinterHead_Heat_Circle(void)
{
    GPIO_WriteBit(PRINTER_STB_PORT, PRINTER_STB1_PIN, (BitAction)1); // Enable
    vTaskDelay(HEAT_TIME);
    GPIO_WriteBit(PRINTER_STB_PORT, PRINTER_STB1_PIN, (BitAction)0); // Disable

    GPIO_WriteBit(PRINTER_STB_PORT, PRINTER_STB2_PIN, (BitAction)1);
    vTaskDelay(HEAT_TIME);
    GPIO_WriteBit(PRINTER_STB_PORT, PRINTER_STB2_PIN, (BitAction)0);

    GPIO_WriteBit(PRINTER_STB_PORT, PRINTER_STB3_PIN, (BitAction)1);
    vTaskDelay(HEAT_TIME);
    GPIO_WriteBit(PRINTER_STB_PORT, PRINTER_STB3_PIN, (BitAction)0);

    GPIO_WriteBit(PRINTER_STB_PORT, PRINTER_STB4_PIN, (BitAction)1);
    vTaskDelay(HEAT_TIME);
    GPIO_WriteBit(PRINTER_STB_PORT, PRINTER_STB4_PIN, (BitAction)0);

    GPIO_WriteBit(PRINTER_STB_PORT, PRINTER_STB5_PIN, (BitAction)1);
    vTaskDelay(HEAT_TIME);
    GPIO_WriteBit(PRINTER_STB_PORT, PRINTER_STB5_PIN, (BitAction)0);

    GPIO_WriteBit(PRINTER_STB_PORT, PRINTER_STB6_PIN, (BitAction)1);
    vTaskDelay(HEAT_TIME);
    GPIO_WriteBit(PRINTER_STB_PORT, PRINTER_STB6_PIN, (BitAction)0);
}

void PrinterHead_ClearDotLineArray(void)
{
    uint8_t i;
    for (i = 0; i < DOTLINE_SIZE; i++)
    {
        dotLine[i] = 0;
    }
}

void PrinterHead_SendDotLineData(uint8_t autoClear)
{
    uint8_t i, j;
    uint8_t temp;
    for (i = 0; i < DOTLINE_SIZE; i++)
    {
        temp = MySPI_SwapByte(dotLine[i]);
    }
    PrinterHead_LAT_Enable();
    vTaskDelay(LAT_TIME);
    PrinterHead_LAT_Disable();
    if (autoClear != 0)
    {
        PrinterHead_ClearDotLineArray(); // 自动清除dotLine，方便外界继续向dotLine中放入数据
    }
}

void PrinterHead_PrintDotLine(void)
{
    PrinterHead_SendDotLineData(1); // 自动清除dotLine

#if !USE_CIRCLE_HEAT
    PrinterHead_Heat_Enable();
    vTaskDelay(HEAT_TIME);
    PrinterHead_Heat_Disable();
#else
    PrinterHead_Heat_Circle();
#endif

    PrinterMoto_Run_Circle(4);
}

void PrinterHead_PrintBlankLine(void)
{
    PrinterMoto_Run_Circle(BlankDotLine);
}

void PrinterHead_PrintLineSpace(void)
{
    PrinterMoto_Run_Circle(LINE_SPACE);
}

void PrinterHead_PrintSegmentSpace(void)
{
    PrinterMoto_Run_Circle(SEGMENT_SPACE);
}

void PrinterHead_RunDotLine(void)
{
    PrinterMoto_Run_Circle(4);
}

void PrinterHead_Heat_Custom(uint8_t selectSTB, uint8_t heatTime, uint8_t isCircleHeat)
{
    uint16_t tempSTBPins[] = {PRINTER_STB1_PIN, PRINTER_STB2_PIN, PRINTER_STB3_PIN,
                              PRINTER_STB4_PIN, PRINTER_STB5_PIN, PRINTER_STB6_PIN};
    if (isCircleHeat)
    {
        //  根据selectSTB加热 1到6位依次控制 STB1~STB6
        for (uint8_t i = 0; i < 6; i++)
        {
            if (selectSTB & (0x01 << i))
            {
                GPIO_WriteBit(PRINTER_STB_PORT, tempSTBPins[i], (BitAction)1); // Enable
                vTaskDelay(heatTime);
                GPIO_WriteBit(PRINTER_STB_PORT, tempSTBPins[i], (BitAction)0); // Disable
            }
        }
    }
    else
    {
        uint16_t portVal = 0x0000;
        for (uint8_t i = 0; i < 6; i++)
        {
            if (selectSTB & (0x01 << i))
            {
                portVal |= tempSTBPins[i];
            }
        }
        GPIO_Write(PRINTER_STB_PORT, portVal);
        vTaskDelay(heatTime);
        GPIO_Write(PRINTER_STB_PORT, portVal);
    }
}

void PrinterHead_PrintDotLineCustom(uint8_t autoClear, uint8_t selectSTB, uint8_t heatTime, uint8_t isCircleHeat,
                                    uint8_t autoRunDotLine)
{
    PrinterHead_SendDotLineData(autoClear);

    PrinterHead_Heat_Custom(selectSTB, heatTime, isCircleHeat);

    if (autoRunDotLine != 0)
    {
        PrinterMoto_Run_Circle(4);
    }
}
