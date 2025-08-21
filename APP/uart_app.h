#ifndef __UARTAPP_H__
#define __UARTAPP_H__

#include "bsp_system.h"
void uart0_init(void);
void uart_proc();
int fputc(int ch, FILE *f);
#endif
