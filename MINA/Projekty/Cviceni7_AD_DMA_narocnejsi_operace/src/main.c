#include "mina_shield_mbed.h"
#include "nucleo_usart.h"
#include "string.h"
#include "stdio.h"

volatile uint32_t ticks = 0;

/*
// Pøed prùmìrováním hodnt a umístìním do pole
//uint16_t w1 = 0;
//uint16_t w2 = 0;
*/

#define ARRAY_SIZE 16

uint16_t arrayAD[ARRAY_SIZE];
uint32_t average_Array = 0;

// Pøed DMA:
//uint8_t position = 0;

void SysTick_Handler(void) {
	ticks++;
}
/*// Tohle už nepotøebuju (mám DMA)... který to bere bez ohledu na interrupt
void ADC_IRQHandler(void)
{

    ADC1 -> SR &= ~ADC_SR_EOC; // shodím pøíznak EOC

// tenhle IRQ už asi není potøeba myslím (data jdou pøed DMA rovnou do buferu)

	arrayAD[position] = ADC1->DR;
	position++;

	if(position>ARRAY_SIZE-1)
	{
		position = 0;
	}

	prumer_arrayAD = soucet/ARRAY_SIZE;

}
*/
/*// Tohle už nepotøebuju (mám DMA)... spouštím prevod pres TRGO TIM2
 *
void TIM2_IRQHandler(void) // AD pøevodník má spouštìný start od èasovaèe TIM2
{
	TIM2->SR &= ~TIM_SR_UIF; // shození pøíznaku TIM2
	ADC1->CR2 |= ADC_CR2_SWSTART; // spusteni prevodu

}*/

void DMA2_Stream0_IRQHandler(void)
{

	average_Array = 0;
	// zjistím od koho pøišel interupt (complete nebo half)
	// full transfrer
	GPIOWrite(POMOCNY_PIN3,true);
	if ((DMA2->LISR & DMA_LISR_TCIF0) != 0) // na bitu DMA_LISR_TCIF0 bude jednièka pokud je transfer dokonèen
	{
		// spocitej prumer pro druhou polovinu pole
		for(int i = ARRAY_SIZE/2; i<ARRAY_SIZE;i++)
		{
		     average_Array += arrayAD[i];
		}

	}
	// polovièní fransfer
	if((DMA2->LISR & DMA_LISR_HTIF0) != 0)
	{
		// spocitej prumer pro prvni polovinu pole
		for(int i = 0; i<ARRAY_SIZE/2;i++)
		{
		     average_Array += arrayAD[i];
		}
	}
	average_Array /= (ARRAY_SIZE/2);
	GPIOWrite(POMOCNY_PIN3,false);

	// shodím pøíznaky:
	DMA2->LIFCR = DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTEIF0| DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0; // shození pøíznaku pro všechny interrupty kromì complete transfer a half transfer
	// LIFCR pro kanály 0 až 3
	// HIFCR pro kanály 4 až 7


}


int main(void) {

	SystemCoreClockUpdate();

	SetClock100MHz(clockSourceHSE);
	SysTick_Config(GetBusClock(busClockAPB1) / 1000);
	Usart2Initialization(38400);

	// Tohle už nepotøebuju (mám DMA)... popis u funkcí
	//NVIC_EnableIRQ(ADC_IRQn); // povolím handler od ADC
	//NVIC_EnableIRQ(TIM2_IRQn); // povolím handler od TIM2
	NVIC_EnableIRQ(DMA2_Stream0_IRQn); // povolím handler od DMAèka

	GPIOConfigurePin(POMOCNY_PIN2,ioPortOutputPushPull);
	GPIOConfigurePin(POMOCNY_PIN3,ioPortOutputPushPull);

	///////////////// DMA ////////////////////////////////////////////////////////////////////////////////

	if (!(RCC->AHB1ENR & RCC_AHB1ENR_DMA2EN)) {
			RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
			RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA2RST;
			RCC->AHB1RSTR &= ~RCC_AHB1RSTR_DMA2RST;
		}

		DMA2_Stream0->CR &= ~DMA_SxCR_EN; // EN = 0 !!, jinak nepujdou zmeny
		DMA2_Stream0->CR &= ~DMA_SxCR_CHSEL; // CHSEL = 000 = kanál 0
		DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0; // MSIZE = 01 = 16bit
		DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0; // PSIZE = 01
		DMA2_Stream0->CR |= DMA_SxCR_MINC; // MINC = 1 = memory increment
		DMA2_Stream0->CR &= ~DMA_SxCR_PINC; // PINC = 0
		DMA2_Stream0->CR &= ~DMA_SxCR_DIR; // DIR = 00 = pheriph to memory
		DMA2_Stream0->CR |= DMA_SxCR_CIRC; // cirkulaèní mód
		DMA2_Stream0->CR |= DMA_SxCR_TCIE; // Pøerušení úplného naplnìní
		DMA2_Stream0->CR |= DMA_SxCR_HTIE; // Pøerušení polovièního naplnìní

		DMA2_Stream0->NDTR = ARRAY_SIZE; // velikost bufferu
		DMA2_Stream0->PAR = (uint32_t)&ADC1->DR; // Adresa registru DR (nebo JDR1..)
		DMA2_Stream0->M0AR = (uint32_t) arrayAD; // Adresa bufferu

		DMA2->LIFCR = DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTEIF0| DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0; // Shození všech pøíznakù "interrupty"
		// LIFCR pro kanály 0 až 3
		// HIFCR pro kanály 4 až 7
		DMA2_Stream0->CR |= DMA_SxCR_EN; // povolení DMA

    ///////////////// DMA ////////////////////////////////////////////////////////////////////////////////
	///////////////// AD /////////////////////////////////////////////////////////////////////////////////
	GPIOConfigurePin(POT_LEFT, ioPortAnalog);
	GPIOConfigurePin(POT_RIGHT, ioPortAnalog);

	if (!(RCC->APB2ENR & RCC_APB2ENR_ADC1EN)) {
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
		RCC->APB2RSTR |= RCC_APB2RSTR_ADCRST; // ADCRST je reset bit pro všechny ADC
		RCC->APB2RSTR &= ~RCC_APB2RSTR_ADCRST; //
	}

	ADC1->CR1 = 0 | ADC_CR1_SCAN; // povolení práce s SQR kanály
	ADC1->CR1 |= ADC_CR1_EOCIE;
	ADC1->CR2 = 0; // ALIGN = 0 (zarovnání vpravo)
	ADC1->CR2 |= ADC_CR2_DMA; // povolení DMA pro ADC
	ADC1->CR2 |= ADC_CR2_DDS; // aby stále pøevádìl i v cirkulaèním režimu
    ADC1->CR2 |= ADC_CR2_EXTEN_0; // Triger rising edge (AD spuštìný od TIM2)
	ADC1->CR2 |= (ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2); // aby mohl být AD spuštìný od TIM2 pomocí TRGO
	ADC1->SMPR1 = 0; // urèení doby pøevodu, chceme pouze pro kanál 0 takže SMPR2
	ADC1->SMPR2 = ADC_SMPR2_SMP0_1; // 010, pro kanál nula = 28 cyklu

	ADC1->SQR1 = 0; // L = 0000 = 1 konverze
	ADC1->SQR2 = 0;
	ADC1->SQR3 = 0; // SQ1 = 00000 = AD kanal 0


    ADC->CCR |= ADC_CCR_ADCPRE_0; // TSVREFE = 0 (neni teplotní sensor, neni interní reference), ADCPRE = 1 (APB2 / 4)
	// CCR spoleèný pro všechny, proto nemá èíslo
	ADC1->CR2 |= ADC_CR2_ADON; // zapnuti AD
	///////////////// AD /////////////////////////////////////////////////////////////////////////////////
	///////////////// TIM2 ///////////////////////////////////////////////////////////////////////////////
	if (!(RCC->APB1ENR & RCC_APB1ENR_TIM2EN))
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
		RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
		RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
	}

	TIM2->CR1 = TIM_CR1_DIR; // DIR = 1 = dekrementace
	TIM2->CNT &= ~TIM_CNT_CNT; // pro jistotu nulovani CNT
	TIM2->PSC = GetBusClock(timersClockAPB1)/ 1000000-1; // 1us
	TIM2->ARR = 1000 - 1; // 1ms
	//TIM2->DIER = TIM_DIER_UIE; // Nastavení generování pøerušení ven, smìr do jádra (jednièka)
	TIM2->CR2 |= TIM_CR2_MMS_1;
	TIM2->CR1 |= TIM_CR1_CEN;

	///////////////// TIM2 ///////////////////////////////////////////////////////////////////////////////

	uint32_t tm = 20;

	puts("Zacatek aplikace\n");

	while (1) {

		if (ticks >= tm) {
			tm = ticks + 20;

			GPIOToggle(POMOCNY_PIN2);


		 printf("Prumer: %d   Pole: [", (int)average_Array);

			for(int i = 0; i<ARRAY_SIZE-1;i++)
			{
				printf("%d, ", arrayAD[i]);
			}
			printf("%d]\n",arrayAD[ARRAY_SIZE-1]);


		}

	}
}

