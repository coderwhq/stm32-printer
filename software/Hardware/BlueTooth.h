#ifndef __BLUE_TOOTH_H
#define __BLUE_TOOTH_H

void BlueTooth_Init(uint32_t baudRate);

void BlueTooth_SendByte(uint8_t byte);
void BlueTooth_SendString(char *str);

#endif
