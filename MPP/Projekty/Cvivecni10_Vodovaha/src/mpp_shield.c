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

 void BuzzerTone(tone t,uint32_t timePlay,uint32_t timeMute)
 {
 	for(int i = 0;i<timePlay;i++)
 	{
 	GPIOWrite(BUZZER,true);
 	Sleep(t);
 	GPIOWrite(BUZZER,false);
 	Sleep(t);
     }
 	Sleep(timeMute);
 }

 void InicializaceSPILED(void)
{
	GPIOConfigurePin(LED_LE, ioPortOutputPushPull); // SHIFT - LE
	GPIOWrite(LED_LE, 0);
	GPIOConfigurePin(LED_OE, ioPortOutputPushPull); // SHIFT - OE
	GPIOWrite(LED_OE, 0);

	#ifdef USE_SPI

	GPIOConfigurePin(LED_CLK,ioPortAlternatrPushPull); // SHIFT - CLK
	GPIOConfigureAlternativFunction(LED_CLK,5);
	GPIOConfigurePin(LED_SDI,ioPortAlternatrPushPull); // SHIFT - SDI
	GPIOConfigureAlternativFunction(LED_SDI,5);

	if (!(RCC->APB2ENR & RCC_APB2ENR_SPI1EN)) {
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
		RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
		RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
	}
	SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM; // CPOL = 0, CPHA = 0, DFF = 0 = 8b data, BR = 0 = clk/2
	SPI1->CR2 = 0; // zadne preruseni ani DMA
	SPI1->CR1 |= SPI_CR1_SPE; // enable
#else
			GPIOConfigurePin(LED_CLK, ioPortOutputPushPull); // SHIFT - CLK
			GPIOWrite(LED_CLK, 0);
			GPIOConfigurePin(LED_SDI, ioPortOutputPushPull);// SHIFT - SDI
#endif
}

void WriteSPILED(uint8_t value) {
#ifdef USE_SPI
//TODO otestuj, zda je odvysilano predchozi
	SPI1->DR = value;
	while (!(SPI1->SR & SPI_SR_TXE) || (SPI1->SR & SPI_SR_BSY));
	GPIOWrite(LED_LE, 1); // puls na LE
	GPIOWrite(LED_LE, 0);
#else
	for (int i = 0; i < 8; i++) {

		uint8_t bitValue = ((value & (0x01 << i)) >> i);

		GPIOWrite(LED_SDI, bitValue);

		GPIOWrite(LED_CLK, 1);
		GPIOWrite(LED_CLK, 0);

	}

	GPIOWrite(LED_LE, 1);
	GPIOWrite(LED_LE, 0);
#endif
}
