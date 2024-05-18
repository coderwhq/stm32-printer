#ifndef __MY_SPI2_H
#define __MY_SPI2_H
#include "stm32f10x.h"

void MySPI2_Init(void);
void MySPI2_Start(void);
void MySPI2_Stop(void);
uint8_t MySPI2_SwapByte(uint8_t ByteSend);

#endif
