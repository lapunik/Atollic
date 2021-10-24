/*
 * nucleo_core.c
 *
 *  Created on: Apr 12, 2016
 *      Author: weiss
 */

#include "nucleo_core.h"

void SystemClock_100MHz(void)
{
  RCC->CR |= RCC_CR_HSION;            // pro jistotu zapnout HSI
  while(!(RCC->CR & RCC_CR_HSIRDY))   // cekej na zapnuti
    ;

  RCC->CFGR &= ~RCC_CFGR_SW;      // HSI as clock (kombinace 00)
  RCC->CR &= ~RCC_CR_PLLON;       // vypnout PLL, pokud by byla z HSE
  RCC->CR = 0x81;                 // vypnout vse krome HSI, nejspis totiz bylo HSE
                                  // HSI TRIM reset state - bits [7..3]

  RCC->APB1ENR |= RCC_APB1ENR_PWREN;  // zapnout PWR blok
  PWR->CR = (PWR->CR & ~PWR_CR_VOS) | PWR_CR_VOS_0 | PWR_CR_VOS_1;  // scale mode 1

  RCC->CR |= RCC_CR_HSEON;
  while(!(RCC->CR & RCC_CR_HSERDY))   //TODO - timeout a prip. zustat na HSI
    ;

  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE;  // a ostatni vynulovat

  // Cube: HSE 8M, CLK 100M - /M = 4, PLL: N = 100x, P = /2
  // AHB = /1, APB1 = /2, APB2 = /1

  RCC->PLLCFGR |= 100 << 6;   // N 100x = 001100100 na [14..6]
  RCC->PLLCFGR |= 4;          // M   /4 = 000100    na [5..0]
                              // P   /2 = 00        na [17..16]

  RCC->CFGR &= ~RCC_CFGR_HPRE;   // 0xxx = not divided
  RCC->CFGR &= ~RCC_CFGR_PPRE1;  // 0xx = not divided
//  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;  // 100 = /2
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;  // 100 = /4 - podle Cube sice max. 50MHz, ale omezuje to I2C max. 42
  RCC->CFGR &= ~RCC_CFGR_PPRE2;  // 0xx = not divided

  RCC->CR |= RCC_CR_PLLON;
  while(!(RCC->CR & RCC_CR_PLLRDY))
    ;

  FLASH->ACR &= ~FLASH_ACR_LATENCY;    // dle RM 3.4.1, Table 5
  FLASH->ACR |= FLASH_ACR_LATENCY_3WS;

  RCC->CFGR |= RCC_CFGR_SW_PLL;    // a jede se z PLL
  //TODO test SWS - clock status
}

bool Nucleo_SetPinGPIO(GPIO_TypeDef *gpio, uint32_t bitnum, nucleoPinAttribute typ)
{
  uint32_t enr_mask = 0;
  uint32_t rstr_mask = 0;

  switch((uint32_t)gpio)
  {
    case (uint32_t)GPIOA:
      enr_mask = RCC_AHB1ENR_GPIOAEN;
      rstr_mask = RCC_AHB1RSTR_GPIOARST;
      break;
    case (uint32_t)GPIOB:
      enr_mask = RCC_AHB1ENR_GPIOBEN;
      rstr_mask = RCC_AHB1RSTR_GPIOBRST;
      break;
    case (uint32_t)GPIOC:
      enr_mask = RCC_AHB1ENR_GPIOCEN;
      rstr_mask = RCC_AHB1RSTR_GPIOCRST;
      break;
    case (uint32_t)GPIOD:
      enr_mask = RCC_AHB1ENR_GPIODEN;
      rstr_mask = RCC_AHB1RSTR_GPIODRST;
      break;
    case (uint32_t)GPIOE:
      enr_mask = RCC_AHB1ENR_GPIOEEN;
      rstr_mask = RCC_AHB1RSTR_GPIOERST;
      break;
      /* pozor, 411 nema F a G
    case (uint32_t)GPIOF:
      enr_mask = RCC_AHB1ENR_GPIOFEN;
      rstr_mask = RCC_AHB1RSTR_GPIOFRST;
      break;
    case (uint32_t)GPIOG:
      enr_mask = RCC_AHB1ENR_GPIOGEN;
      rstr_mask = RCC_AHB1RSTR_GPIOGRST;
      break;
      */
    case (uint32_t)GPIOH:
      enr_mask = RCC_AHB1ENR_GPIOHEN;
      rstr_mask = RCC_AHB1RSTR_GPIOHRST;
      break;
    default:
      return false;
  }

  if (!(RCC->AHB1ENR & enr_mask))     // not set yet ?
  {
    RCC->AHB1ENR |= enr_mask;         // enable peripheral clock
    RCC->AHB1RSTR |= rstr_mask;       // reset peripheral
    RCC->AHB1RSTR &= ~rstr_mask;
  }

  gpio->MODER &= ~(0x00000003 << (2 * bitnum));    // clear 2 bits
  switch(typ)
  {
    case input_pullup:
      gpio->PUPDR &= ~(0x00000003 << (2 * bitnum)); // 01 = pull-up
      gpio->PUPDR |= 0x00000001 << (2 * bitnum);
      break;
    case input:     // 00 = input mode, nothing to do
      gpio->PUPDR &= ~(0x00000003 << (2 * bitnum)); // 00 = no pull-up/dn
      break;
    case output:    // 01 = output mode
    case output_open:
      gpio->MODER |= 0x00000001 << (2 * bitnum);    // set bits

      if (typ == output_open)
        gpio->OTYPER |= 0x00000001 << bitnum;      // 1 = open-drain, one bit per GPIO
      else
        gpio->OTYPER &= ~(0x00000001 << bitnum);      // 0 = push-pull, one bit per GPIO

      gpio->OSPEEDR |= 0x00000003 << (2 * bitnum);  // high-speed = 11

      gpio->PUPDR &= ~(0x00000003 << (2 * bitnum)); // 00 = no pull-up/pull-down
      break;
    case alter:     // 10 = AF
    case alter_open:
      gpio->MODER |= 0x00000002 << (2 * bitnum);    // set bits
        // don't forget set AFR registers !!!
      if (typ == alter_open)
        gpio->OTYPER |= 0x00000001 << bitnum;      // 1 = open-drain, one bit per GPIO
      else
        gpio->OTYPER &= ~(0x00000001 << bitnum);      // 0 = push-pull, one bit per GPIO

      gpio->OSPEEDR |= 0x00000003 << (2 * bitnum);  // high-speed = 11
      break;
    case analog:    // 11 - analog mode
      gpio->MODER |= 0x00000003 << (2 * bitnum);    // set bits
      break;
    default:
      return false;
  }

  return true;
}

bool Nucleo_SetPinAFGPIO(GPIO_TypeDef *gpio, uint32_t bitnum, uint32_t afValue)
{
  gpio->AFR[(bitnum < 8) ? 0 : 1] &= ~(0x0000000f << (4 * (bitnum & 0x7)));    // clear 4 bits
  gpio->AFR[(bitnum < 8) ? 0 : 1] |= (afValue << (4 * (bitnum & 0x7)));        // set AF bits
  return true;
}

uint32_t GetTimerClock(int timerNum)
{
  uint32_t apbdiv = 0, timerClock = SystemCoreClock;

  switch(timerNum)
  {
    case 1:
    case 9:
    case 10:
    case 11:
      apbdiv = RCC->CFGR & RCC_CFGR_PPRE2;   // 0x0000E000 - [15..13]
      apbdiv >>= 13;
      break;
    case 2:
    case 3:
    case 4:
    case 5:
      apbdiv = RCC->CFGR & RCC_CFGR_PPRE1;   // 0x00001C00 - zachovej bity 12:10, zbytek zahodit
      apbdiv >>= 10;
      break;
    default:
      return 0;
  }

  if ((apbdiv & 0x04) == 0)       // nejvyssi bit z tech 3 == 0 ?
    timerClock = SystemCoreClock;   // not divided, x1
  else
    timerClock = 2 * (SystemCoreClock >> ((apbdiv & 0x03) + 1));   // spodni 2 bity

  return timerClock;
}
