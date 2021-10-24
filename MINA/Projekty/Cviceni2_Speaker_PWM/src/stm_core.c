/*
 * stm_core.c
 *
 *  Created on: 28. 2. 2019
 *      Author: Lapunik
 */
#include "stm_core.h"

bool GPIOConfigurePin(GPIO_TypeDef *gpio,uint32_t bitNumber, ioPortMode mode) // konfigurace GPIO do p�edp�ipraven�ch stav�
{

	uint32_t enableMask = 0;
	uint32_t resetMask = 0;

	switch((uint32_t)gpio) // je pot�eba zapnout hodiny podle toho kter� gpio chceme vyuzivat => ulozime bity pro reset a pro zapnuti do masek a pote masku aplikujeme
	{
	case (uint32_t)GPIOA: // GPIOA je definovana konstanta ve form�tu "ukazatel", proto ms�m p�etypovat na celo��selnou hodnotu
        enableMask = RCC_AHB1ENR_GPIOAEN;
        resetMask = RCC_AHB1RSTR_GPIOARST;
	break;

	case (uint32_t)GPIOB:
        enableMask = RCC_AHB1ENR_GPIOBEN;
        resetMask = RCC_AHB1RSTR_GPIOBRST;
	break;

	case (uint32_t)GPIOC:
        enableMask = RCC_AHB1ENR_GPIOCEN;
        resetMask = RCC_AHB1RSTR_GPIOCRST;
	break;

	case (uint32_t)GPIOD:
        enableMask = RCC_AHB1ENR_GPIODEN;
        resetMask = RCC_AHB1RSTR_GPIODRST;
	break;

	case (uint32_t)GPIOE:
        enableMask = RCC_AHB1ENR_GPIOEEN;
        resetMask = RCC_AHB1RSTR_GPIOERST;
	break;
#if defined(STM32F411xE) // 411RE nem� GPIOF a GPIOG
#else
	case (uint32_t)GPIOF:
        enableMask = RCC_AHB1ENR_GPIOFEN;
        resetMask = RCC_AHB1RSTR_GPIOFRST;
	break;

	case (uint32_t)GPIOG:
        enableMask = RCC_AHB1ENR_GPIOGEN;
        resetMask = RCC_AHB1RSTR_GPIOGRST;
	break;
#endif
	case (uint32_t)GPIOH:
        enableMask = RCC_AHB1ENR_GPIOHEN;
        resetMask = RCC_AHB1RSTR_GPIOHRST;
	break;

	}

	if((resetMask == 0)||(enableMask == 0)) // pokud se do masek neulo�ila ��dn� hodnota, ukon�i konfiguraci a vrat zpravu false o ne�sp�chu
	{
		return false;
	}
	else
	{
		  if(!(RCC -> AHB1ENR & enableMask)) // jestli�e nejsou masky pr�zdn�, pou�i je pro nastaven� hodin
		  {
		    RCC->AHB1ENR |= enableMask;
		    RCC->AHB1RSTR |= resetMask;
		    RCC->AHB1RSTR &= ~resetMask;
		  }
	}

gpio -> MODER &= ~(0x03 << (bitNumber*2)); // vynulovani zdvojen�ch bitu p�ed nastavov�n�m (kdyby n�kde byla kombinace 10 a j� tam ORoval 01 tak vysledna kombinace je 11 misto 01)...u registru kde se nastavuje pouze jeden bit toho nehroz�
gpio -> PUPDR &= ~(0x03 << (bitNumber*2));
gpio -> OSPEEDR &= ~(0x03 << (bitNumber*2));


switch(mode)
{
case ioPortOutputPushPull:

	gpio->MODER |= 0x01<<(2*bitNumber); // output
	gpio->OTYPER &= ~(0x01<<bitNumber); // push-pull
	gpio->OSPEEDR |= 0x03<<(2*bitNumber); // height speed
	gpio->PUPDR &= ~(0x03<<bitNumber); // no pull up, no pull down
    break;

case ioPortOutputOpenDrain:

	gpio->MODER |= 0x01<<(2*bitNumber); // output
	gpio->OTYPER |= (0x01<<bitNumber); // open drain
	gpio->OSPEEDR |= 0x03<<(2*bitNumber); // height speed
	gpio->PUPDR &= ~(0x03<<bitNumber); // no pull up, no pull down
    break;

case ioPortAnalog:

	gpio->MODER |= 0x03<<(2*bitNumber); // analog
    break;

case ioPortInputFloat:

	gpio->PUPDR &= ~(0x03<<bitNumber); // no pull up, no pull down
    break;

case ioPortInputPullUp:

	gpio->PUPDR |= ~(0x01<<bitNumber); // pull up
    break;

case ioPortInputPullDown:

	gpio->PUPDR |= ~(0x02<<bitNumber); // pull down
    break;

case ioPortAlternatrPushPull:

	gpio->MODER |= 0x02<<(2*bitNumber); // alternate function
	gpio->OTYPER &= ~(0x01<<bitNumber); // push pull
	gpio->OSPEEDR |= 0x03<<(2*bitNumber); // height speed
	gpio->PUPDR &= ~(0x03<<bitNumber); // no pull up, no pull down
    break;

case ioPortAlternatrOpenDrain:

	gpio->MODER |= 0x02<<(2*bitNumber); // alternate function
	gpio->OTYPER |= (0x01<<bitNumber); // open drain
	gpio->OSPEEDR |= 0x03<<(2*bitNumber); // height speed
	gpio->PUPDR &= ~(0x03<<bitNumber); // no pull up, no pull down
    break;


}
    return true;
}

bool GPIOConfigureAlternativFunction(GPIO_TypeDef *gpio,uint32_t bitNumber, uint32_t afValue) // konfigurace GPIO do alternativn� funkce
{
	// alternativni funkce se nastavuje pomoci registr� AFR,(to ��k� kter� alternativn� funkce je p�ipojena), registry jsou dva(AFRL,AFRH) proto�e pot�ebujeme 64bit (je tam �estn�ct alternativn�ch kan�l� na ka�dej vstup a v�stup(to zanemn� 4 bity na jeden kan�l))
	// v hlavi�kov�ch souborech neni AFRL a AFRH ale je tam dvouprvkove uint 32 bitove pole (AFR[0] = AFRL,AFR[1] = AFRH)
	uint8_t afr = (bitNumber < 8) ? 0 : 1 ;

    gpio->AFR[afr] &= ~(0x0f << (4 * (bitNumber & 0x07))); // vynulujeme 4 bity naseho kanalu, pro jistotu maskuju bit number
    gpio->AFR[afr] |= ((afValue & 0x0f) << (4 * (bitNumber & 0x07))); // nastavim pozadovanou hodnotu alternativni funkce, prom jistotu, kdyby byla afValue vetsi ne� 4 bity(nem�la by b�t) si maskujeme jenom dan� 4 bity kter� chceme pou��vat
    return true;

  //  gpio->AFR[(bitNumber < 8) ? 0 : 1] &= ~(0x0f << (4 * (bitNumber & 0x07))); // vynuluj AF bity
  //  gpio->AFR[(bitNumber < 8) ? 0 : 1] |= ((afValue & 0x0f) << (4 * (bitNumber & 0x07))); // nastav AF bity



}

void GPIOToggle(GPIO_TypeDef *gpio, uint32_t bitNumber) // funkce pro zm�nu na v�stupu ODR pro bit na zadan�m m�st�
{
gpio -> ODR ^= (1<<bitNumber); // prost� jen zm�� hobnotu v ODR pro p��slu�n� bit
}

bool GPIORead(GPIO_TypeDef *gpio, uint32_t bitNumber) // funkce pro zji�t�n� hodnoty na v�stupu ODR pro bit na zadan�m m�st�
{
	if((gpio -> MODER & (0x03<<(bitNumber*2))) == 0x00) // jestli�e je pin nastaven jako vstup
	{
		 return ((gpio -> IDR) & (1<<bitNumber)); // AND ODR s maskou vr�t� 0 (false) pokud zkoumany bit je nula, a pokud neni nula a je to jakekoliv jine cislo, nezalezi na to jake, vrati (true)

	}else if ((gpio -> MODER & (0x03<<(bitNumber*2))) == (0x01<<(bitNumber*2))) // pokud ne, tak je na prislusnem bitNumber kombinace 01
	{
		 return ((gpio -> ODR) & (1<<bitNumber)); // AND ODR s maskou vr�t� 0 (false) pokud zkoumany bit je nula, a pokud neni nula a je to jakekoliv jine cislo, nezalezi na to jake, vrati (true)
	}else
	{
		return false; // je pot�eba dod�lat!!!!! pro analog a alternativvn� funkce nebude fungovat!!!!!!!!
	}

}

void GPIOWrite(GPIO_TypeDef *gpio, uint32_t bitNumber,bool state) // funkce pro zapsani hodnoty na v�stupu ODR pro bit na zadan�m m�st�
{ // BSRR je registr kter nastavi natvrdo hodnotu na registru ODR, m�me celkem 16 v�stup�, BSRR m� 32 v�stup�, s t�m �e pokud d�m jedni�ku na n�kter� ze spodn�ch 16, ��k�m t�m nastav jedna na konkr�tn� bit(0-16) a pokud d�m jedni�ku na n�kter� z horn�ch 16 bit� BSRR, tak vlastn� ��k�m, nastav nulu na konkr�tn�m bitu (0-16)
 if(state) // pokud chci zapsat jednicku
 {
	gpio -> BSRR = (0x01<<bitNumber); // zapis ji do spodni poloviny BSRR na m�sto (0-16)
 }else // pokud ne
 {
	 gpio -> BSRR = ((0x01<<bitNumber)<<16); // zapis ji do horni poloviny registru BSRR(17-31), co� zajist� vynulov�n� p��slu�n�ho bitu
 }
}
