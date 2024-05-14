#ifndef __PRINTER_HEAD_H
#define __PRINTER_HEAD_H
#include "stm32f10x.h"

extern uint8_t dotLine[];

void PrinterHead_Init(void);
void PrinterHead_PrintDotLine(void);
void PrinterHead_PrintBlankLine(void);

#endif
