/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 7.1.0   2017-03-27

The MIT License (MIT)
Copyright (c) 2009-2017 Atollic AB

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

/***************************************************************
*   Nucleo411RE USART2 SIGNALS:                                *
*                                                              *
*   RxD        PA3        Input floating                       *
*   TxD        PA2        Alternate push-pull output           *
*                                                              *
****************************************************************/



/* Includes */
#include "stm32f4xx.h"
#include "stdio.h"

/* Private macro */
#define BIT_RATE_38400_16MHz   0x01A1
#define ERASE_SCREEN  "\033[2J"
#define POSITION_0_0  "\033[0;0H"

/* Private variables */
/* Private function prototypes */
/* Private functions */

int AnsiPuts(char *cPtr)
{
  for (;*cPtr;cPtr++)
    putchar(*cPtr);

  return 1;
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

  /* TODO - Add your application code here */

  unsigned int keycode;

  if(!(RCC->APB1ENR & RCC_APB1ENR_USART2EN))
  {
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
    __NOP();
    RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
  }

  USART2->CR1 = USART_CR1_TE | USART_CR1_RE;
  USART2->CR2 = 0U;
  USART2->CR3 = 0U;
  USART2->BRR = BIT_RATE_38400_16MHz;
  USART2->CR1 |= USART_CR1_UE;


  if(!(RCC->AHB1ENR & RCC_AHB1ENR_GPIOAEN))
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOARST;
    __NOP();
    RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOARST;
  }

  GPIOA->MODER = ((GPIOA->MODER & ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER2)) | GPIO_MODER_MODER3_1 | GPIO_MODER_MODER2_1 );
  GPIOA->OTYPER &= ~ GPIO_OTYPER_OT_2;
  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2;
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR3 | GPIO_PUPDR_PUPDR2);
  GPIOA->AFR[0] = (GPIOA->AFR[0] & 0xFFFF00FF) | 0x00007700;

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stdin, NULL, _IONBF, 0);

  puts("Test stdio ...");
  puts("Line2 ...");
  puts("Line3 ...");
  puts("Test stdio ...  Line4");

  keycode = getchar();
  AnsiPuts(ERASE_SCREEN);
  AnsiPuts(POSITION_0_0);

  while(1)
  {
    keycode = getchar();
    putchar(keycode);
  }

  /* Infinite loop
  while (1)
  {
    while(!(USART2->SR & USART_SR_RXNE))      // Wait for receive buffer not empty
    {
    }
    keycode = USART2->DR;                     // Read received keycode

    while(!(USART2->SR & USART_SR_TXE))       // Wait for transmit buffer empty
    {
    }
    USART2->DR = keycode;                     // Echo received character back to
  }
  */
}