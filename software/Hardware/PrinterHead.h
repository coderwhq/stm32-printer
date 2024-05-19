#ifndef __PRINTER_HEAD_H
#define __PRINTER_HEAD_H
#include "stm32f10x.h"

extern uint8_t dotLine[];

void PrinterHead_Init(void);
void PrinterHead_PrintDotLine(void);
void PrinterHead_PrintBlankLine(void);
void PrinterHead_PrintLineSpace(void);
void PrinterHead_PrintSegmentSpace(void);

void PrinterHead_RunDotLine(void);
void PrinterHead_Heat_Custom(uint8_t selectSTB, uint8_t heatTime, uint8_t isCircleHeat);
void PrinterHead_PrintDotLineCustom(uint8_t autoClear, uint8_t selectSTB, uint8_t heatTime, uint8_t isCircleHeat,
                                    uint8_t autoRunDotLine);
#endif
