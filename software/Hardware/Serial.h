#ifndef __SERIAL_H
#define __SERIAL_H
#include "stm32f10x.h"

void Serial_Init(uint32_t baudRate);
void Serial_SendByte(uint8_t byte);
void Serial_SendString(char *str);

#endif
