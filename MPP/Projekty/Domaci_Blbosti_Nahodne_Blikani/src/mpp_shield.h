/*
 * mpp_shield.h
 *
 *  Created on: 28. 2. 2019
 *      Author: Lapunik
 */

#ifndef MPP_SHIELD_H_
#define MPP_SHIELD_H_

#include "stm_core.h"

#define RGBLED_RED  GPIOB,15
#define RGBLED_GREEN  GPIOB,14
#define RGBLED_BLUE  GPIOB,13

#define BUTTON_RIGHT  GPIOB,4
#define BUTTON_LEFT  GPIOB,5

void Inicializace8LED();
void Write8LED(uint8_t value);

#define BUZZER  GPIOB,0

#endif /* MPP_SHIELD_H_ */
