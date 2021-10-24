/*
 * nucleo_usart.c
 *
 *  Created on: 21. 3. 2019
 *      Author: Lapunik
 */

#include "nucleo_usart.h"

int Usart2Send(char c) //vstup znak ktery se ma odeslat, navratova hodnota int znak ktery se odesilal
{

	//Flag TXE je nastaven do jednicky pokud je prazdny datovy registr, tudíž mùžeme zaèíst odesílat data, flag se automaticky nuluje když se datový registr zaplní
	// vlastnì øiká èekej dokud nebude volno, chtìlo by to ochranu protože když tuhle funkci zavolám a usart bude vypnutý, nikdy tento jev nenastane a bude to vlastnì while(true)
	while (!(USART2->SR & USART_SR_TXE)) {
		// TXE nemusim nulovat, dela to sam ze zapisu nove data
	}
	USART2->DR = c; // do datového registru vložím znak který chci odeslat
	return c;
}

void Usart2SendString(char *txt) //vstup znak ktery se ma odeslat, navratova hodnota int znak ktery se odesilal
{

	//Flag TXE je nastaven do jednicky pokud je prazdny datovy registr, tudíž mùžeme zaèíst odesílat data, flag se automaticky nuluje když se datový registr zaplní
	// vlastnì øiká èekej dokud nebude volno, chtìlo by to ochranu protože když tuhle funkci zavolám a usart bude vypnutý, nikdy tento jev nenastane a bude to vlastnì while(true)
	while (*txt) {
		Usart2Send(*txt); // na konci je ukoncovaci nula(jakože prázdný data)
		txt++;
	}
}

int Usart2Recived(void) {
	while (!(USART2->SR & USART_SR_RXNE)) {
		// RXNE nemusim nulovat, dela to sam když data vyètu, když je nevyètu, nulovat se nebude
	}
	return USART2->DR;
}

bool IsUsart2Recived(void) {
	return (USART2->SR & USART_SR_RXNE) != 0; // testování jestli jsou nìjaká data k pøeètení
}


void Usart2Initialization(int baudRate){ // baud zatim nefunkcni

	if (!(RCC->APB1ENR & RCC_APB1ENR_USART2EN)) // neni povolen USART2
	{
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
		RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
		RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
	}

	// v nucleo schematu lze najit ze USART TX a USART RX (ètení z procesoru, zápis do nìj) je na pinech PA2 a PA3
	GPIOConfigurePin(GPIOA, 2, ioPortAlternatrPushPull); // vysílání procesoru
	GPIOConfigureAlternativFunction(GPIOA, 2, 7); // z DS kap. 4 table 9 je vidìt že pro PA2 resp. 3 je USART2 nastaven na alternativní funkci 7
	GPIOConfigurePin(GPIOA, 3, ioPortAlternatrPushPull); // pøíjem procesoru, pro pøíjem vede cesta jinudy takže nezáleží jestli push pull nebo openDrain
	GPIOConfigureAlternativFunction(GPIOA, 3, 7);
// do teï jsme nastavovali GPIO na USART, ale teï musíme nastavit samotnou periferii USART

	USART2->CR1 = USART_CR1_RE | USART_CR1_TE; // Do konfiguraèního registru jedna "zapnu" recived(pøijmání) a transmit (vysílání)
	USART2->CR2 = 0; // nic nenastavujeme zatím
	USART2->CR3 = 0; // nic nenastavujeme zatím

	uint sampling = (USART2->CR1 & USART_CR1_OVER8) ? 8 : 16;
	uint32_t apb1, mant, tmp, frac;
	apb1 = GetBusClock(busClockAPB1);
	mant = apb1 * 16 / (sampling * baudRate); // v setinach
	tmp = mant / 16;
	frac = mant -(tmp * 16); // zbyvajici cast 0-99 nutno prevest na 0-15
	//frac = ((((frac * sampling) + 50) / 100)); // +50 kvùli zaokrouhlovani oøezem intu
	USART2->BRR = (tmp << 4) | (frac & 0x0f); // mantisa vyssich 12b, zlomek dolni 4b, celkem 16b

	USART2->CR1 |= USART_CR1_UE; // nakonec povoluji usart


	setvbuf(stdout,NULL,_IONBF,0); // datový proud, nepoviný parametr, typ(nebufrovat), délka v tomto pøípadì nesmyslný argument, takže nula
	setvbuf(stdin,NULL,_IONBF,0); // používám pro vstupní i výstupní (printf a podobnì všechno používá stdint/stdout)
}
