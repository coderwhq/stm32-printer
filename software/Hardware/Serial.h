#ifndef __SERIAL_H
#define __SERIAL_H
#include "stm32f10x.h"

extern char serialReceiveString[100];

void Serial_Init(uint32_t baudRate);
void Serial_SendByte(uint8_t byte);
void Serial_SendString(char *str);
uint8_t Serial_GetReceiveFlag(void);

#endif
