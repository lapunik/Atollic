/*
 * nucleo_uart.h
 *
 *  Created on: Apr 12, 2016
 *      Author: weiss
 */

#ifndef NUCLEO_UART_H_
#define NUCLEO_UART_H_

#include <stdio.h>
#include "nucleo_core.h"

bool Nucleo_StartUart2(uint32_t baudrate);

int usart2_putc(int ch);
int usart2_getc(void);
unsigned char isCharRecv(void);
void SetAddCRtoLF(bool val);

#endif /* NUCLEO_UART_H_ */
