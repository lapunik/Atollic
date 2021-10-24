/*
 * nucleo_usart.h
 *
 *  Created on: 21. 3. 2019
 *      Author: Lapunik
 */

#ifndef NUCLEO_USART_H_
#define NUCLEO_USART_H_

#include "stm_core.h"
#include <stdio.h>

#endif /* NUCLEO_USART_H_ */

int Usart2Send(char c);
void Usart2SendString(char *txt);
int Usart2Recived(void);
bool IsUsart2Recived(void);
void Usart2Initialization(int baudSpeed);
