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

  GPIOC -> MODER &= (~0xffffffff); // nastavujeme pro dolnich 16 jedni�ek nulu(MODER m� 32 bit� pro 16 port�), abychom m�li jistotu �e tam nen� n�jak� jedni�ka nasataven� tam kde nechceme
  GPIOC -> MODER |= 0x5555; // a� te� m��eme nastavit pro ka�d� z osmi dolnich dvojic bit� kombinace 010101..
  GPIOC -> OTYPER &= (~0xff); // dolnich 8 bit� pot�ebujeme m�t jako nula (push pull), (m� pouze 16bit�)
  GPIOC -> OSPEEDR |= 0xffffffff;  // zase m� 32 bit� a my chceme spodnich 16 mit v jednickach (11 - hight speed)
  // proc by tady mel byt return?
  }

  void Write8LED(uint8_t value)
  {
  GPIOC -> ODR &= (~(0xff)) ; // bitovou maskou 0000 0000 1111 1111 vynulujeme spodn�ch 8 bit�
  GPIOC -> ODR |= (uint16_t)value; // bitovou masku m�m ze vsupn�ho argumentu, pomoc� OR nastav�m jedni�ky na mnou po�adovan�ch bitech
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

	void WriteSPILED(uint8_t val)
	{
	int i;
	for(i = 0; i < 8; i++)
	{
	GPIOWrite(GPIOA, 7, (val & 0x80) != 0); // maska 1000 0000
	GPIOWrite(GPIOA, 5, 1); // puls na CLK
	GPIOWrite(GPIOA, 5, 0);
	val <<= 1;
	}
	GPIOWrite(GPIOA, 8, 1); // puls na LE
	GPIOWrite(GPIOA, 8, 0);
	}
