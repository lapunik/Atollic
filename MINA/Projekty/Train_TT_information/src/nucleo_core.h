/*
 * nucleo_core.h
 *
 *  Created on: Apr 12, 2016
 *      Author: weiss
 */

#ifndef NUCLEO_CORE_H_
#define NUCLEO_CORE_H_

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#if (HSE_VALUE != 8000000)
#error HSE_VALUE must be set to 8MHz - parameters of project (or system_stm32f4xx.c) !!
#endif

void SystemClock_100MHz(void);

typedef enum nucleoPinAttribute_enum
{
  unknown = 0,
  input,
  input_pullup,
  output,
  output_open,
  analog,
  alter,
  alter_open
} nucleoPinAttribute;

bool Nucleo_SetPinGPIO(GPIO_TypeDef *gpio, uint32_t bitnum, nucleoPinAttribute typ);
bool Nucleo_SetPinAFGPIO(GPIO_TypeDef *gpio, uint32_t bitnum, uint32_t afValue);

#define GPIO_READ(gpio, bitnum)     ((gpio->IDR & (1 << bitnum)) ? true : false)


typedef enum {
  busClockAHB,
  busClockAPB1,
  busClockAPB2,
  timersClockAPB1,
  timersClockAPB2
} eBusClocks;

uint32_t GetBusClock(eBusClocks clk);

// atomic write: lower 16 bits = set to 1,  higher 16 bits = set to 0
#if defined(STM32F40X)   // F401, prip. F429 = nutno doplnit
#define GPIO_WRITE(gpio, bitnum, state)    { if (state) { gpio->BSRRL = 1 << bitnum; } else { gpio->BSRRH = 1 << bitnum; } }
#elif defined(STM32F411xE)
#define GPIO_WRITE(gpio, bitnum, state) { gpio->BSRR = (state) ? (1 << (bitnum)) : ((1 << (bitnum)) << 16); }
#else
#error Nutno specifikovat rodinu procesoru podle prace s BSRR registrem
#endif
#define GPIO_TOGGLE(gpio, bitnum)     { gpio->ODR ^= (1 << bitnum); }

#ifndef _ALWAYS_INLINE
#define _ALWAYS_INLINE __attribute__((always_inline)) static inline
#endif

//! prekvapive musi byt STATIC, pripadne take __attribute__((always_inline))
//__attribute__((always_inline)) static inline void GPIOToggle(GPIO_TypeDef *gpio, uint32_t bitnum)
// a navic je telo funkce v H
_ALWAYS_INLINE void GPIOToggle(GPIO_TypeDef *gpio, uint32_t bitnum)
{
  gpio->ODR ^= (1 << bitnum);
}

_ALWAYS_INLINE bool GPIORead(GPIO_TypeDef *gpio, uint32_t bitnum)
{
  return ((gpio->IDR & (1 << bitnum)) != 0);    // porovnani = vynucena logicka hodnota
}

_ALWAYS_INLINE void GPIOWrite(GPIO_TypeDef *gpio, uint32_t bitnum, bool state)
{
// non atomic access  gpio->ODR |= (1 << bitnum);
#if defined(STM32F40XX) // | F429 ??
  if (state) { gpio->BSRRL = 1 << bitnum; } else { gpio->BSRRH = 1 << bitnum; }
#else
  gpio->BSRR = (state) ? (1 << (bitnum)) : ((1 << (bitnum)) << 16);
#endif
  // BSRR register - lower 16 bits = Set to 1,  higher 16 bits = Reset to 0
}

uint32_t GetTimerClock(int timerNum);

#define SPI_IS_BUSY(SPIn) ((((SPIn)->SR & (SPI_SR_TXE | SPI_SR_RXNE)) == 0) \
                          || (((SPIn)->SR & SPI_SR_BSY)))
#define SPI_WAIT(SPIn)            while (SPI_IS_BUSY(SPIn))

#endif /* NUCLEO_CORE_H_ */
