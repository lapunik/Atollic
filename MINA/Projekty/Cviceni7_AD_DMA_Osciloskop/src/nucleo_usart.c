/*
 * nucleo_usart.c
 *
 *  Created on: 21. 3. 2019
 *      Author: Lapunik
 */

#include "nucleo_usart.h"

int Usart2Send(char c) //vstup znak ktery se ma odeslat, navratova hodnota int znak ktery se odesilal
{

	//Flag TXE je nastaven do jednicky pokud je prazdny datovy registr, tud� m��eme za��st odes�lat data, flag se automaticky nuluje kdy� se datov� registr zapln�
	// vlastn� �ik� �ekej dokud nebude volno, cht�lo by to ochranu proto�e kdy� tuhle funkci zavol�m a usart bude vypnut�, nikdy tento jev nenastane a bude to vlastn� while(true)
	while (!(USART2->SR & USART_SR_TXE)) {
		// TXE nemusim nulovat, dela to sam ze zapisu nove data
	}
	USART2->DR = c; // do datov�ho registru vlo��m znak kter� chci odeslat
	return c;
}

void Usart2SendString(char *txt) //vstup znak ktery se ma odeslat, navratova hodnota int znak ktery se odesilal
{

	//Flag TXE je nastaven do jednicky pokud je prazdny datovy registr, tud� m��eme za��st odes�lat data, flag se automaticky nuluje kdy� se datov� registr zapln�
	// vlastn� �ik� �ekej dokud nebude volno, cht�lo by to ochranu proto�e kdy� tuhle funkci zavol�m a usart bude vypnut�, nikdy tento jev nenastane a bude to vlastn� while(true)
	while (*txt) {
		Usart2Send(*txt); // na konci je ukoncovaci nula(jako�e pr�zdn� data)
		txt++;
	}
}

int Usart2Recived(void) {
	while (!(USART2->SR & USART_SR_RXNE)) {
		// RXNE nemusim nulovat, dela to sam kdy� data vy�tu, kdy� je nevy�tu, nulovat se nebude
	}
	return USART2->DR;
}

bool IsUsart2Recived(void) {
	return (USART2->SR & USART_SR_RXNE) != 0; // testov�n� jestli jsou n�jak� data k p�e�ten�
}


void Usart2Initialization(int baudRate){ // baud zatim nefunkcni

	if (!(RCC->APB1ENR & RCC_APB1ENR_USART2EN)) // neni povolen USART2
	{
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
		RCC->APB1RSTR |= RCC_APB1RSTR_USART2RST;
		RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;
	}

	// v nucleo schematu lze najit ze USART TX a USART RX (�ten� z procesoru, z�pis do n�j) je na pinech PA2 a PA3
	GPIOConfigurePin(GPIOA, 2, ioPortAlternatrPushPull); // vys�l�n� procesoru
	GPIOConfigureAlternativFunction(GPIOA, 2, 7); // z DS kap. 4 table 9 je vid�t �e pro PA2 resp. 3 je USART2 nastaven na alternativn� funkci 7
	GPIOConfigurePin(GPIOA, 3, ioPortAlternatrPushPull); // p��jem procesoru, pro p��jem vede cesta jinudy tak�e nez�le�� jestli push pull nebo openDrain
	GPIOConfigureAlternativFunction(GPIOA, 3, 7);
// do te� jsme nastavovali GPIO na USART, ale te� mus�me nastavit samotnou periferii USART

	USART2->CR1 = USART_CR1_RE | USART_CR1_TE; // Do konfigura�n�ho registru jedna "zapnu" recived(p�ijm�n�) a transmit (vys�l�n�)
	USART2->CR2 = 0; // nic nenastavujeme zat�m
	USART2->CR3 = 0; // nic nenastavujeme zat�m

	uint sampling = (USART2->CR1 & USART_CR1_OVER8) ? 8 : 16;
	uint32_t apb1, mant, tmp, frac;
	apb1 = GetBusClock(busClockAPB1);
	mant = apb1 * 16 / (sampling * baudRate); // v setinach
	tmp = mant / 16;
	frac = mant -(tmp * 16); // zbyvajici cast 0-99 nutno prevest na 0-15
	//frac = ((((frac * sampling) + 50) / 100)); // +50 kv�li zaokrouhlovani o�ezem intu
	USART2->BRR = (tmp << 4) | (frac & 0x0f); // mantisa vyssich 12b, zlomek dolni 4b, celkem 16b

	USART2->CR1 |= USART_CR1_UE; // nakonec povoluji usart


	setvbuf(stdout,NULL,_IONBF,0); // datov� proud, nepovin� parametr, typ(nebufrovat), d�lka v tomto p��pad� nesmysln� argument, tak�e nula
	setvbuf(stdin,NULL,_IONBF,0); // pou��v�m pro vstupn� i v�stupn� (printf a podobn� v�echno pou��v� stdint/stdout)
}
