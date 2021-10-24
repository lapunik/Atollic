#include "mina_shield_mbed.h"
#include "nucleo_usart.h"
#include "string.h"
#include "stdio.h"

volatile uint32_t ticks = 0;

/*
// P�ed pr�m�rov�n�m hodnt a um�st�n�m do pole
//uint16_t w1 = 0;
//uint16_t w2 = 0;
*/

#define ARRAY_SIZE 16

uint32_t arrayAD[ARRAY_SIZE];
uint32_t prumer_arrayAD = 0;
uint8_t position = 0;

void SysTick_Handler(void) {
	ticks++;
}

void ADC_IRQHandler(void)
{
    ADC1 -> SR &= ~ADC_SR_JEOC; // shod�m p��znak JEOC

	arrayAD[position] = ADC1->JDR1;
	position++;
	arrayAD[position] = ADC1->JDR2;
	position++;

	if(position>ARRAY_SIZE-1)
	{
		position = 0;
	}

	uint32_t soucet = 0;

	for(int i = 0;i<ARRAY_SIZE;i++)
	{

       soucet += arrayAD[i];

	}

	prumer_arrayAD = soucet/ARRAY_SIZE;

}

void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_SR_UIF; // shozen� p��znaku TIM3

	ADC1->CR2 |= ADC_CR2_JSWSTART;// spu�t�n� p�evodu
}

int main(void) {

	SystemCoreClockUpdate();

	SetClock100MHz(clockSourceHSE);
	SysTick_Config(100000000 / 1000);
	Usart2Initialization(38400);

	NVIC_EnableIRQ(ADC_IRQn); // povol�m handler od ADC
	NVIC_EnableIRQ(TIM2_IRQn); // povol�m handler od TIM2

	///////////////// TIM2 ///////////////////////////////////////////////////////////////////////////////
	if (!(RCC->APB1ENR & RCC_APB1ENR_TIM2EN))
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
		RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
		RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
	}

	TIM2->CR1 = TIM_CR1_DIR; // DIR = 1 = dekrementace
	TIM2->CNT &= ~ TIM_CNT_CNT; // pro jistotu nulovani CNT
	TIM2->PSC = GetBusClock(timersClockAPB1)/ 1000000-1; // 1us
	TIM2->ARR = 1000 - 1; // 1ms
	TIM2->DIER = TIM_DIER_UIE; // Nastaven� generov�n� p�eru�en� ven, sm�r do j�dra (jedni�ka)
	TIM2->CR1 |= TIM_CR1_CEN;

	///////////////// TIM2 ///////////////////////////////////////////////////////////////////////////////
	///////////////// AD /////////////////////////////////////////////////////////////////////////////////
	GPIOConfigurePin(POT_LEFT, ioPortAnalog);
	GPIOConfigurePin(POT_RIGHT, ioPortAnalog);

	if (!(RCC->APB2ENR & RCC_APB2ENR_ADC1EN)) {
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
		RCC->APB2RSTR |= RCC_APB2RSTR_ADCRST; // ADCRST je reset bit pro v�echny ADC
		RCC->APB2RSTR &= ~RCC_APB2RSTR_ADCRST; //
	}

	ADC1->CR1 = 0 | ADC_CR1_SCAN; // povolen� pr�ce s SQR kan�ly
	ADC1->CR1 |= ADC_CR1_JEOCIE;
	ADC1->CR2 = 0; // ALIGN = 0 (zarovn�n� vpravo)
	ADC1->SMPR1 = 0; // ur�en� doby p�evodu, chceme pouze pro kan�l 0 tak�e SMPR2
	ADC1->SMPR2 = ADC_SMPR2_SMP0_1; // 010, pro kan�l nula = 28 cyklu
    /*
	// Regular:
	//ADC1->SQR1 = 0; // L = 0000 = 1 konverze
	//ADC1->SQR2 = 0;
	//ADC1->SQR3 = 0; // SQ1 = 00000 = AD kanal 0
    */
	ADC1->JSQR |= ADC_JSQR_JL_0; // dv� konverze

	ADC1->JSQR &= ~ADC_JSQR_JSQ4; // kan�l 0 (musi byt JSQ4 nula)
	ADC1->JSQR &= ~ADC_JSQR_JSQ3;
	ADC1->JSQR |= ADC_JSQR_JSQ3_0; // kan�l 1 (musi byt JSQ3 jedna)


    ADC->CCR |= ADC_CCR_ADCPRE_0; // TSVREFE = 0 (neni teplotn� sensor, neni intern� reference), ADCPRE = 1 (APB2 / 4)
	// CCR spole�n� pro v�echny, proto nem� ��slo
	ADC1->CR2 |= ADC_CR2_ADON; // zapnuti AD
	///////////////// AD /////////////////////////////////////////////////////////////////////////////////

	uint32_t tm = 20;
	/*
	// Regular:
	//uint16_t w = 0;
	// Injection p�ed handlerem :
	//uint16_t w1 = 0;
	//uint16_t w2 = 0;


	// JEOC SE NESHAZUJE S�M !!!
    */
	puts("Zacatek aplikace\n");

	while (1) {

		if (ticks >= tm) {
			tm = ticks + 20;
            /*
			// Regular:
			//ADC1->CR2 |= ADC_CR2_SWSTART; // spusteni prevodu
			//while (!(ADC1->SR & ADC_SR_EOC))
			//w = ADC1->DR;

			// Injection p�ed handlerem :
			ADC1->CR2 |= ADC_CR2_JSWSTART; // start prevod
			while (!(ADC1->SR & ADC_SR_JEOC))
				;

			w1 = ADC1->JDR1;
			w2 = ADC1->JDR2;


			// ADC1->CR2 |= ADC_CR2_JSWSTART;// spu�t�n� p�evodu, dokud jsem nem�l p�eru�en� TIm2, bylo pot�eba tady, te� u� je pase
			//printf("AD kanal 0 (JDR1): %d     AD kanal 1 (JDR2): %d\n", w1, w2);
			 */

			printf("Prumer pole: %d\n", (int)prumer_arrayAD);


		}

	}
}

