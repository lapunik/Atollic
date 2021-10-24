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

void OnLED (uint8_t value)
{
  GPIOB -> ODR |= value<<13;
}

void OffLED (uint8_t value)
{
  GPIOB -> ODR &= (~(value))<<13;
}

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
      RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
      RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOBRST;
      RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOBRST;

    }

   GPIOB->MODER |= (0x15<<26);
   GPIOB->OTYPER &= ~(0xe000);
   GPIOB->OSPEEDR |= (0x3f<<26);

   uint8_t j = 1;

   /* Infinite loop */
   while (1)
   {
      OffLED(0x07);
      OnLED(j);

      j++;

      if(j==0x08)
      {
        j = 1;
      }

      for(int i = 0;i<1000000;i++){}

   }
 }