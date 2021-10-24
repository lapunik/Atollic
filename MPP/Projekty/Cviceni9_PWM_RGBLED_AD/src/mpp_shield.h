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

#define BUZZER  GPIOB,0

#define LED_SDI  GPIOA,7
#define LED_CLK  GPIOA,5
#define LED_OE  GPIOA,9
#define LED_LE  GPIOA,8

#define POTENCIOMETR GPIOA, 0

typedef enum
{
	A = 1000,
	B = 2000,
	C = 3000,
}tone;

void Inicializace8LED();
void Write8LED(uint8_t value);
void WriteSPILED(uint8_t val);
void Sleep(uint32_t value);

void BuzzerTone(tone t,uint32_t timePlay,uint32_t timeMute);


#endif /* MPP_SHIELD_H_ */
