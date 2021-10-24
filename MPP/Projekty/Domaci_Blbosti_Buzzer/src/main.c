/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2019-02-24

The MIT License (MIT)
Copyright (c) 2019 STMicroelectronics

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{

  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used. 
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates 
  *  SCB->VTOR register.  
  *  E.g.  SCB->VTOR = 0x20000000;  
  */
  if(!(RCC ->AHB1ENR |= RCC_AHB1ENR_GPIOBEN))
   {
     RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; // zapni hodiny pro periferii GPIO (Nastavuji bit(kter� m� ulo�enou hodnotu pod konstanou "RCC_AHB1ENR_GPIOAEN") v registru AHB1ENR do jedi�ky(or jedni�ka na tom bitu))
     RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOBRST; // prove� restart periferie GPIO (te� ovl�d�me registr AHB1RSTR, nastavime bit "RCC_AHB1RSTR_GPIOARST" do jednicky a o radek niz do nuly aby se resetovala periferie)
     RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOBRST; // konec restartu periferie GPIO (zp�t do nuly(negaci bitu "RCC_AHB1RSTR_GPIOARST" (to ale ovsem znamen� �e na v�ech ostatnich mistech krom na tomhle bitu je nyn� jedni�ka) andujeme s p�vodn�m registrem(v�ude jsou jedni�ky, tam se hodnota nemeni, jen tak kde je nula bude bit nastaven do nuly) ))

   }

  GPIOB->MODER |= (0x01);
  GPIOB->OTYPER &= ~(0x01);
  GPIOB->OSPEEDR |= (0x03);

  /* Infinite loop */
  while (1)
  {
    GPIOB -> ODR |= 0x01;
    for(int i = 0;i<100;i++){}
    GPIOB -> ODR &= ~0x01;
    for(int i = 0;i<100;i++){}

  }
}