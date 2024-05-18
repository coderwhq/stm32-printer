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

    // ��ʼ�� LAT ��������Ĵ���
    GPIO_InitStructure.GPIO_Pin = PRINTER_LAT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_Init(PRINTER_LAT_PORT, &GPIO_InitStructure);
    // Ĭ�ϲ���������Ĵ������ߵ�ƽ�ر�
    GPIO_SetBits(PRINTER_LAT_PORT, PRINTER_LAT_PIN);

    // ��ʼ�� ѡͨ��������
    GPIO_InitStructure.GPIO_Pin = PRINTER_STB_PINS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_Init(PRINTER_STB_PORT, &GPIO_InitStructure);
    // Ĭ�ϲ��������壬�͵�ƽ�ر�
    GPIO_ResetBits(PRINTER_STB_PORT, PRINTER_STB_PINS);

    PrinterMoto_Init(); // ��ӡͷmoto��ʼ��
}
/*
    ÿ��384���㣬ÿ��STB����64���㡣
    ÿ�п��Դ�ӡ 384 / 8 = 48 ���ֽڣ��ַ���
    ÿ��STB���Կ��� 64 / 8 = 8 ���ֽڣ��ַ���

    ������384��CLKʱ�ӣ�Ҳ���� 384 / 8 = 48��SPI�ֽڽ�����һ�������ݴ�����ϣ�Ŀǰ�������λ�Ĵ���
    Ȼ������LATһ��ʱ�䣨�鿴�ֲᣩ�����������浽��������
    Ȼ������STBһ��ʱ����м��ȣ�����һ���е����ݾʹ�ӡ���ˣ���STBλ��ʱ���ڸ���ÿ����λ��1��0ѡ��õ��Ƿ���ȣ�
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

void PrinterHead_SendDotLineData(void)
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
    PrinterHead_ClearDotLineArray(); // �Զ����dotLine��������������dotLine�з�������
}

void PrinterHead_PrintDotLine(void)
{
    PrinterHead_SendDotLineData();

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
