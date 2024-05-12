#include "stm32f10x.h"
/* SPI */
#define USE_SPI_PERIPH 1 // ʹ��Ӳ��SPI

#define SPI_RCC RCC_APB2Periph_SPI1
#define SPI_RCC_GPIO RCC_APB2Periph_GPIOA

#define SPI_PINS_PORT GPIOA
#define SPI_SCK_PIN GPIO_Pin_5
#define SPI_MISO_PIN GPIO_Pin_6
#define SPI_MOSI_PIN GPIO_Pin_7

#if USE_SPI_PERIPH

#define SPI_X SPI1

#else

void MySPI_Write_SCK(uint8_t bit)
{
    GPIO_WriteBit(SPI_PINS_PORT, SPI_SCK_PIN, (BitAction)bit);
}

void MySPI_Write_MOSI(uint8_t bit)
{
    GPIO_WriteBit(SPI_PINS_PORT, SPI_MOSI_PIN, (BitAction)bit);
}

uint8_t MySPI_Read_MISO(void)
{
    return GPIO_ReadInputDataBit(SPI_PINS_PORT, SPI_MISO_PIN);
}

#endif

void MySPI_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
#if USE_SPI_PERIPH
    SPI_InitTypeDef SPI_InitStructure;
    RCC_APB2PeriphClockCmd(SPI_RCC, ENABLE);
#endif
    RCC_APB2PeriphClockCmd(SPI_RCC_GPIO, ENABLE);

    GPIO_InitStructure.GPIO_Mode =
#if USE_SPI_PERIPH
        GPIO_Mode_AF_PP;
#else
        GPIO_Mode_Out_PP;
#endif
    GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN | SPI_MOSI_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(SPI_PINS_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(SPI_PINS_PORT, &GPIO_InitStructure);
#if USE_SPI_PERIPH
    SPI_StructInit(&SPI_InitStructure);

    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CRCPolynomial = 7; // CRCУ�����ʽ����Ĭ��ֵ7
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // ����˫��ȫ˫��ģʽ���ü����Ź���
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                 // ��λ��ǰ
    // SPI_InitStructure.SPI_NSS = ;

    SPI_Init(SPI_X, &SPI_InitStructure);

    SPI_Cmd(SPI_X, ENABLE);
#else
    /* ����00ģʽ���䣬����ʱ��Ĭ��Ϊ�͵�ƽ */
    MySPI_Write_SCK(0);
#endif
}

uint8_t MySPI_SwapByte(uint8_t byte)
{
#if USE_SPI_PERIPH
    /* Ӳ��SPI�����ֽ� */
    while (SPI_I2S_GetFlagStatus(SPI_X, SPI_I2S_FLAG_TXE) != SET) // ���ͻ�������Ϊ��ʱ�ȴ�
    {
    }
    SPI_I2S_SendData(SPI_X, byte);
    while (SPI_I2S_GetFlagStatus(SPI_X, SPI_I2S_FLAG_RXNE) != SET) // ���ջ�����Ϊ��ʱ�ȴ�
    {
    }
    return SPI_I2S_ReceiveData(SPI_X);

#else
    /* ���ģ��SPI�����ֽ� */
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        MySPI_Write_MOSI(byte & 0x80);
        byte <<= 1;

        MySPI_Write_SCK(1);
        byte |= MySPI_Read_MISO(); // ����MISO���յ�������λ �� ������ĩβ������λ����

        MySPI_Write_SCK(0);
    }
    return byte;
#endif
}
