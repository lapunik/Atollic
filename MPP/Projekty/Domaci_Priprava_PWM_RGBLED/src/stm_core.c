/*
 * stm_core.c
 *
 *  Created on: 28. 2. 2019
 *      Author: Lapunik
 */
#include "stm_core.h"

bool GPIOConfigurePin(GPIO_TypeDef *gpio,uint32_t bitNumber, ioPortMode mode) // konfigurace GPIO do pøedpøipravených stavù
{

	uint32_t enableMask = 0;
	uint32_t resetMask = 0;

	switch((uint32_t)gpio) // je potøeba zapnout hodiny podle toho který gpio chceme vyuzivat => ulozime bity pro reset a pro zapnuti do masek a pote masku aplikujeme
	{
	case (uint32_t)GPIOA: // GPIOA je definovana konstanta ve formátu "ukazatel", proto msím pøetypovat na celoèíselnou hodnotu
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
#if defined(STM32F411xE) // 411RE nemá GPIOF a GPIOG
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

	if((resetMask == 0)||(enableMask == 0)) // pokud se do masek neuložila žádná hodnota, ukonèi konfiguraci a vrat zpravu false o neúspìchu
	{
		return false;
	}
	else
	{
		  if(!(RCC -> AHB1ENR & enableMask)) // jestliže nejsou masky prázdné, použi je pro nastavení hodin
		  {
		    RCC->AHB1ENR |= enableMask;
		    RCC->AHB1RSTR |= resetMask;
		    RCC->AHB1RSTR &= ~resetMask;
		  }
	}

gpio -> MODER &= ~(0x03 << (bitNumber*2)); // vynulovani zdvojených bitu pøed nastavováním (kdyby nìkde byla kombinace 10 a já tam ORoval 01 tak vysledna kombinace je 11 misto 01)...u registru kde se nastavuje pouze jeden bit toho nehrozí
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

bool GPIOConfigureAlternativFunction(GPIO_TypeDef *gpio,uint32_t bitNumber, uint32_t afValue) // konfigurace GPIO do alternativní funkce
{
	// alternativni funkce se nastavuje pomoci registrù AFR,(to øíká která alternativní funkce je pøipojena), registry jsou dva(AFRL,AFRH) protože potøebujeme 64bit (je tam šestnáct alternativních kanálù na každej vstup a výstup(to zanemná 4 bity na jeden kanál))
	// v hlavièkových souborech neni AFRL a AFRH ale je tam dvouprvkove uint 32 bitove pole (AFR[0] = AFRL,AFR[1] = AFRH)
	uint8_t afr = (bitNumber < 8) ? 0 : 1 ;

    gpio->AFR[afr] &= ~(0x0f << (4 * (bitNumber & 0x07))); // vynulujeme 4 bity naseho kanalu, pro jistotu maskuju bit number
    gpio->AFR[afr] |= ((afValue & 0x0f) << (4 * (bitNumber & 0x07))); // nastavim pozadovanou hodnotu alternativni funkce, prom jistotu, kdyby byla afValue vetsi než 4 bity(nemìla by být) si maskujeme jenom dané 4 bity které chceme používat
    return true;

  //  gpio->AFR[(bitNumber < 8) ? 0 : 1] &= ~(0x0f << (4 * (bitNumber & 0x07))); // vynuluj AF bity
  //  gpio->AFR[(bitNumber < 8) ? 0 : 1] |= ((afValue & 0x0f) << (4 * (bitNumber & 0x07))); // nastav AF bity



}

void GPIOToggle(GPIO_TypeDef *gpio, uint32_t bitNumber) // funkce pro zmìnu na výstupu ODR pro bit na zadaném místì
{
gpio -> ODR ^= (1<<bitNumber); // prostì jen zmìò hobnotu v ODR pro pøíslušný bit
}

bool GPIORead(GPIO_TypeDef *gpio, uint32_t bitNumber) // funkce pro zjištìní hodnoty na výstupu ODR pro bit na zadaném místì
{
	if((gpio -> MODER & (0x03<<(bitNumber*2))) == 0x00) // jestliže je pin nastaven jako vstup
	{
		 return ((gpio -> IDR) & (1<<bitNumber)); // AND ODR s maskou vrátí 0 (false) pokud zkoumany bit je nula, a pokud neni nula a je to jakekoliv jine cislo, nezalezi na to jake, vrati (true)

	}else if ((gpio -> MODER & (0x03<<(bitNumber*2))) == (0x01<<(bitNumber*2))) // pokud ne, tak je na prislusnem bitNumber kombinace 01
	{
		 return ((gpio -> ODR) & (1<<bitNumber)); // AND ODR s maskou vrátí 0 (false) pokud zkoumany bit je nula, a pokud neni nula a je to jakekoliv jine cislo, nezalezi na to jake, vrati (true)
	}else
	{
		return false; // je potøeba dodìlat!!!!! pro analog a alternativvní funkce nebude fungovat!!!!!!!!
	}

}

void GPIOWrite(GPIO_TypeDef *gpio, uint32_t bitNumber,bool state) // funkce pro zapsani hodnoty na výstupu ODR pro bit na zadaném místì
{ // BSRR je registr kter nastavi natvrdo hodnotu na registru ODR, máme celkem 16 výstupù, BSRR má 32 výstupù, s tím že pokud dám jednièku na nìkterý ze spodních 16, øíkám tím nastav jedna na konkrétní bit(0-16) a pokud dám jednièku na nìkterý z horních 16 bitù BSRR, tak vlastnì øíkám, nastav nulu na konkrétním bitu (0-16)
 if(state) // pokud chci zapsat jednicku
 {
	gpio -> BSRR = (0x01<<bitNumber); // zapis ji do spodni poloviny BSRR na místo (0-16)
 }else // pokud ne
 {
	 gpio -> BSRR = ((0x01<<bitNumber)<<16); // zapis ji do horni poloviny registru BSRR(17-31), což zajistí vynulování pøíslušného bitu
 }
}
