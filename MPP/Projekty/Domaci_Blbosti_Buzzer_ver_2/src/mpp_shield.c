/*
 * mpp_shield.c
 *
 *  Created on: 28. 2. 2019
 *      Author: Lapunik
 */

#include "mpp_shield.h"

  void Inicializace8LED()
  {
  if(!(RCC ->AHB1ENR |= RCC_AHB1ENR_GPIOCEN))
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOCRST;
    RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOCRST;
  }

  GPIOC -> MODER &= (~0xffffffff); // nastavujeme pro dolnich 16 jednièek nulu(MODER má 32 bitù pro 16 portù), abychom mìli jistotu že tam není nìjaká jednièka nasatavená tam kde nechceme
  GPIOC -> MODER |= 0x5555; // až teï mùžeme nastavit pro každý z osmi dolnich dvojic bitù kombinace 010101..
  GPIOC -> OTYPER &= (~0xff); // dolnich 8 bitù potøebujeme mít jako nula (push pull), (má pouze 16bitù)
  GPIOC -> OSPEEDR |= 0xffffffff;  // zase má 32 bitù a my chceme spodnich 16 mit v jednickach (11 - hight speed)
  // proc by tady mel byt return?
  }

  void Write8LED(uint8_t value)
  {
  GPIOC -> ODR &= (~(0xff)) ; // bitovou maskou 0000 0000 1111 1111 vynulujeme spodních 8 bitù
  GPIOC -> ODR |= (uint16_t)value; // bitovou masku mám ze vsupního argumentu, pomocí OR nastavím jednièky na mnou požadovaných bitech
  // proc by tady mel byt return?
  }
 void Sleep(uint32_t value)
 {
	 for(int i = 0;i<value;i++);
 }
