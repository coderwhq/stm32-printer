#ifndef __PRINTER_MOTO_H
#define __PRINTER_MOTO_H
#include "stm32f10x.h"

void PrinterMoto_Init(void);
void PrinterMoto_Run(uint8_t motor);
void PrinterMoto_Stop(void);
void PrinterMoto_Run_Circle(uint8_t times);

#endif
