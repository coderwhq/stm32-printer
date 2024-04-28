#include "stm32f10x.h"
/* JX-2R-01 */
#define PRINTER_LAT_RCC_GPIO RCC_APB2Periph_GPIOB
#define PRINTER_LAT_PORT GPIOB
#define PRINTER_LAT_PIN GPIO_Pin_1

#define PRINTER_STB_RCC_GPIO RCC_APB2Periph_GPIOB
#define PRINTER_STB_PORT GPIOB
#define PRINTER_STB1_PIN GPIO_Pin_10
#define PRINTER_STB2_PIN GPIO_Pin_11
#define PRINTER_STB3_PIN GPIO_Pin_12
#define PRINTER_STB4_PIN GPIO_Pin_13
#define PRINTER_STB5_PIN GPIO_Pin_14
#define PRINTER_STB6_PIN GPIO_Pin_15
#define PRINTER_STB_PINS                                                                                               \
    PRINTER_STB1_PIN | PRINTER_STB2_PIN | PRINTER_STB3_PIN | PRINTER_STB4_PIN | PRINTER_STB5_PIN | PRINTER_STB6_PIN

void PrinterHead_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(PRINTER_LAT_RCC_GPIO, ENABLE);
    RCC_APB2PeriphClockCmd(PRINTER_STB_RCC_GPIO, ENABLE);

    // ��ʼ�� LAT ��������Ĵ���
    GPIO_InitStructure.GPIO_Pin = PRINTER_LAT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(PRINTER_LAT_PORT, &GPIO_InitStructure);
    // Ĭ�ϲ���������Ĵ������ߵ�ƽ�ر�
    GPIO_SetBits(PRINTER_LAT_PORT, PRINTER_LAT_PIN);

    // ��ʼ�� ѡͨ��������
    GPIO_InitStructure.GPIO_Pin = PRINTER_STB_PINS;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(PRINTER_STB_PORT, &GPIO_InitStructure);
    // Ĭ�ϲ��������壬�͵�ƽ�ر�
    GPIO_ResetBits(PRINTER_STB_PORT, PRINTER_STB_PINS);
}
