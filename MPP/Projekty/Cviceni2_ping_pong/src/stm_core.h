/*
 * stm_core.h
 *
 *  Created on: 28. 2. 2019
 *      Author: Lapunik
 */

#ifndef STM_CORE_H_
#define STM_CORE_H_

#include "stm32f4xx.h"
#include <stdbool.h>

#define BUTTON_BLUE GPIOC,13
#define BOARD_LED GPIOA,5

typedef enum
{
	ioPortOutputPushPull, // re?im v?stup (MODER) v podob? a push-pull (OTYPER)
	ioPortOutputOpenDrain, // re?im v?stup (MODER) v  podob? otev?en?ho kolektoru (OPTYPER)
	ioPortAnalog, // analogov? re?im (MODER)
	ioPortInputFloat, // re?im vstupu (MODER) bez pull up nebo pull down rezistoru (PUPDR)
	ioPortInputPullUp, //re?im vstupu (MODER) s pull up rezistorem (PUPDR)
	ioPortInputPullDown, //re?im vstupu (MODER) s pull down rezistorem (PUPDR)
    ioPortAlternatrPushPull,
	ioPortAlternatrOpenDrain,
}ioPortMode;

bool GPIOConfigurePin(GPIO_TypeDef *gpio,uint32_t bitNumber, ioPortMode mode); //abych mohl funkce pou??vat, mus? b?t t?la v c souboru ale tady v h souboru mus? b?t jejich hlavi?ky
void GPIOToggle(GPIO_TypeDef *gpio, uint32_t bitnum);
bool GPIORead(GPIO_TypeDef *gpio, uint32_t bitnum);
void GPIOWrite(GPIO_TypeDef *gpio, uint32_t bitnum, bool state);

#endif /* STM_CORE_H_ */
