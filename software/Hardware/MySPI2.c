#include "UserConfig.h"
#include "stm32f10x.h" // Device header

void MySPI2_W_SS(uint8_t BitValue)
{
    GPIO_WriteBit(SPI2_PINS_PORT, SPI2_SS_PIN, (BitAction)BitValue);
}

void MySPI2_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    RCC_APB2PeriphClockCmd(SPI2_RCC_GPIO, ENABLE);
    RCC_APB1PeriphClockCmd(SPI2_RCC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = SPI2_SS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(SPI2_PINS_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPI2_SCK_PIN | SPI2_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(SPI2_PINS_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = SPI2_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(SPI2_PINS_PORT, &GPIO_InitStructure);

    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    // 32MHz / 128, ��ΪSPI2��APB1������
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CRCPolynomial = 7;                           // CRCУ�����ʽ����Ĭ��ֵ7
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  // ���ݿ��
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // ����˫��ȫ˫�����ü����ŵĹ���
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; // ��λ���л��ǵ�λ����, MSB��λ���У�LSB��λ����
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;          // ���һ�������Լ�ģ��

    SPI_Init(SPI2_X, &SPI_InitStructure);

    SPI_Cmd(SPI2_X, ENABLE);

    MySPI2_W_SS(1);
}

void MySPI2_Start(void)
{
    MySPI2_W_SS(0);
}

void MySPI2_Stop(void)
{
    MySPI2_W_SS(1);
}
// ģʽ0 �����ֽ�
uint8_t MySPI2_SwapByte(uint8_t ByteSend)
{
    // Ӳ��SPI�����Ƿ�����ͬʱ���գ�Ҫ����ձ������ǰ���ͣ������Ż�����ʱ��

    while (SPI_I2S_GetFlagStatus(SPI2_X, SPI_I2S_FLAG_TXE) != SET)
        ;
    // һ����λ�Ĵ����������ˣ�ʱ���ξͻ��Զ�����������Ҫ�����һ�����ֶ�����һЩ����ʱ��ĺ���
    // ����Ҳ����Ҫ�ֶ����� MOSI �ĸߵ͵�ƽ����ЩӲ�����������ʵ�֣�����ֻ��Ҫ�����ݷŵ�TDR�Ĵ����о���
    SPI_I2S_SendData(SPI2_X, ByteSend);

    while (SPI_I2S_GetFlagStatus(SPI2_X, SPI_I2S_FLAG_RXNE) != SET)
        ;

    return SPI_I2S_ReceiveData(SPI2_X); // ��־λ���Զ����
}
