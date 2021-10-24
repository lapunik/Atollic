#ifndef MINA_SHIELD_H_
#define MINA_SHIELD_H_

#include "stm_core.h"


#define JOY_UP  GPIOA,4
#define JOY_DOWN  GPIOB,0
#define JOY_LEFT  GPIOC,1
#define JOY_RIGHT  GPIOC,0
#define JOY_CENTER  GPIOB,5

#define RGBLED_RED  GPIOB,4
#define RGBLED_GREEN  GPIOC,7
#define RGBLED_BLUE  GPIOA,9

#define SPEAKER  GPIOB,10

#define POT_LEFT  GPIOA,0
#define POT_RIGHT  GPIOA,1

#define LED_SDI  GPIOA,7
#define LED_CLK  GPIOA,5
#define LED_OE  GPIOA,9
#define LED_LE  GPIOA,8

void InicializaceSPILED(void);
void WriteSPILED(uint8_t value);

#endif /* MINA_SHIELD_H_ */
