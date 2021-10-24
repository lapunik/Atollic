/*
 * nucleo_uart.c
 *
 *  Created on: Apr 12, 2016
 *      Author: weiss
 */

#include "nucleo_uart.h"

bool Nucleo_StartUart2(uint32_t baudrate)
{
#ifdef UART_V2
  Nucleo_SetPinGPIO(GPIOA, 2, alter);
  Nucleo_SetPinAFGPIO(GPIOA, 2, 7);
  Nucleo_SetPinGPIO(GPIOA, 3, alter);
  Nucleo_SetPinAFGPIO(GPIOA, 3, 7);
#else
  if (!(RCC->AHB1ENR & RCC_AHB1ENR_GPIOAEN))
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIOARST;
    RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIOARST;
  }

  GPIOA->MODER |= 0x02 << (2 * 2);  // 10 = AF
  GPIOA->MODER |= 0x02 << (3 * 2);  // 10 = AF

  GPIOA->OSPEEDR |= 0x03 << (2 * 2);  // 11 = high-speed
  GPIOA->OSPEEDR |= 0x03 << (3 * 2);  // 11 = high-speed

  GPIOA->AFR[0] &= ~(0x0f << (2 * 4));  // clear AF code
  GPIOA->AFR[0] |= 0x07 << (2 * 4);  // AF code
  GPIOA->AFR[0] &= ~(0x0f << (3 * 4));  // clear AF code
  GPIOA->AFR[0] |= 0x07 << (3 * 4);
#endif

  if (!(RCC->APB1ENR & RCC_APB1ENR_USART2EN))     // not running yet ?
  {
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
  }

  USART2->CR1 = USART_CR1_RE | USART_CR1_TE |  USART_CR1_RXNEIE; // tady jsem pøidal RXNEIE
  USART2->CR2 = 0;
  USART2->CR3 = 0;

  {  //---------------------------- USART BRR Configuration -----------------------
    uint32_t tmpreg = 0x00, apbclock = 0x00;
    uint32_t integerdivider = 0x00;
    uint32_t fractionaldivider = 0x00;

    apbclock = RCC->CFGR & RCC_CFGR_PPRE1;   // zachovej bity 12:10, zbytek zahodit
    apbclock >>= 10;
/*
0xx: AHB clock not divided
100: AHB clock divided by 2
101: AHB clock divided by 4
110: AHB clock divided by 8
111: AHB clock divided by 16
*/
    if ((apbclock & 0x04) == 0)
      apbclock = SystemCoreClock;
    else
      apbclock = SystemCoreClock >> ((apbclock & 0x03) + 1);   // spodni 2 bity

    if ((USART2->CR1 & USART_CR1_OVER8) != 0)  // Determine the integer part
    {
      /* Integer part computing in case Oversampling mode is 8 Samples */
      integerdivider = ((25 * apbclock) / (2 * baudrate));
    }
    else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
    {
      /* Integer part computing in case Oversampling mode is 16 Samples */
      integerdivider = ((25 * apbclock) / (4 * baudrate));
    }
    tmpreg = (integerdivider / 100) << 4;

    /* Determine the fractional part */
    fractionaldivider = integerdivider - (100 * (tmpreg >> 4));

    /* Implement the fractional part in the register */
    if ((USART2->CR1 & USART_CR1_OVER8) != 0)
    {
      tmpreg |= ((((fractionaldivider * 8) + 50) / 100)) & ((uint8_t)0x07);
    }
    else /* if ((USARTx->CR1 & USART_CR1_OVER8) == 0) */
    {
      tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
    }

    /* Write to USART BRR register */
    USART2->BRR = (uint16_t)tmpreg;
  }

  USART2->CR1 |= USART_CR1_UE;

#ifdef UART_V2
  // pozor, bez tohoto nefunguje putchar a musel by byt fputc(.., stdout)
  setvbuf(stdout,NULL,_IONBF,0);
  setvbuf(stdin,NULL,_IONBF,0);
#endif

  return true;
}

static volatile bool _addCRtoLF = true;

int usart2_putc(int ch)
{
  while(!(USART2->SR & USART_SR_TXE))       // Wait for transmit buffer empty
    ;

  if (_addCRtoLF && (ch == '\n'))
  {
    USART2->DR = ch;

    while(!(USART2->SR & USART_SR_TXE))       // Wait for transmit buffer empty
      ;

    ch = '\r';
  }

  return USART2->DR = ch;
}

int usart2_getc(void)
{
  while(!(USART2->SR & USART_SR_RXNE))  // Wait for receive buffer not empty
    ;
  return USART2->DR;                    // Return received character
}

unsigned char isCharRecv(void)
{
  return (USART2->SR & USART_SR_RXNE);  // log 1 = something in recv buffer
}

void SetAddCRtoLF(bool val)
{
  _addCRtoLF = val;
}
