#include "stm32f10x.h" // Device header

// Hardware includes
#include "A4988.h"
#include "Delay.h"
#include "LED.h"
#include "MySPI.h"
#include "PrinterHead.h"

// �����ʼ��
void Peripherals_Init(void)
{
    // �����ж���λ��ռ���ȼ�
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
