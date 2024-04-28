#include "stm32f10x.h"
/* A4988 */
#define A4988_STEP_RCC_GPIO RCC_APB2Periph_GPIOB
#define A4988_STEP_PORT GPIOB
#define A4988_STEP_PIN GPIO_Pin_4

#define A4988_DIR_RCC_GPIO RCC_APB2Periph_GPIOB
#define A4988_DIR_PORT GPIOB
#define A4988_DIR_PIN GPIO_Pin_3

void A4988_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(A4988_STEP_RCC_GPIO, ENABLE);
    RCC_APB2PeriphClockCmd(A4988_DIR_RCC_GPIO, ENABLE);
    // ��ʼ���������������
    GPIO_InitStructure.GPIO_Pin = A4988_STEP_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(A4988_STEP_PORT, &GPIO_InitStructure);

    GPIO_ResetBits(A4988_STEP_PORT, A4988_STEP_PIN);
    // ��ʼ�����Ʒ��������
    GPIO_InitStructure.GPIO_Pin = A4988_DIR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(A4988_DIR_PORT, &GPIO_InitStructure);

    GPIO_ResetBits(A4988_DIR_PORT, A4988_DIR_PIN);
}
