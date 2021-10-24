#include "stm32f4xx.h"


  void __io_putchar(char ch)
  {
     while(!(USART2->SR & USART_SR_TXE))    // Wait for transmit buffer empty
     {
     }       
     USART2->DR = ch; 

   }



  int __io_getchar(void)
  {
   while(!(USART2->SR & USART_SR_RXNE))  // Wait for receive buffer not empty
   {
   }       

   return USART2->DR;                    // Return received character
  }
