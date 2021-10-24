#include "mina_shield_mbed.h"
#include "nucleo_usart.h"
#include "mbed_shield_lcd.h"
#include "string.h"
#include "stdio.h"

volatile uint32_t ticks = 0;

uint32_t average_Array = 0;

#define ARRAY_SIZE 16

#define LCD_WIDTH 128
#define LCD_HEIGHT 32

uint16_t arrayAD[ARRAY_SIZE];
uint16_t arrayOSC[LCD_WIDTH]; // velikost displeje

void SysTick_Handler(void) {
	ticks++;
}

void DMA2_Stream0_IRQHandler(void)
{


	average_Array = 0;
	// zjist�m od koho p�i�el interupt (complete nebo half)
	// full transfrer
	if ((DMA2->LISR & DMA_LISR_TCIF0) != 0) // na bitu DMA_LISR_TCIF0 bude jedni�ka pokud je transfer dokon�en
	{
		// spocitej prumer pro druhou polovinu pole
		for(int i = ARRAY_SIZE/2; i<ARRAY_SIZE;i++)
		{
			 arrayAD[i] = arrayAD[i] >> 7; //z hodnoty 0 az 4096 d�l�m hodnotu 0 az 32 (velikost displeje)
		     average_Array += arrayAD[i];
		}

	}
	// polovi�n� fransfer
	if((DMA2->LISR & DMA_LISR_HTIF0) != 0)
	{
		// spocitej prumer pro prvni polovinu pole
		for(int i = 0; i<ARRAY_SIZE/2;i++)
		{
			 arrayAD[i] = arrayAD[i] >> 7; //z hodnoty 0 az 4096 d�l�m hodnotu 0 az 32 (velikost displeje)
		     average_Array += arrayAD[i];
		}
	}
	average_Array /= (ARRAY_SIZE/2);

	// shod�m p��znaky:
	DMA2->LIFCR = DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTEIF0| DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0; // shozen� p��znaku pro v�echny interrupty krom� complete transfer a half transfer
	// LIFCR pro kan�ly 0 a� 3
	// HIFCR pro kan�ly 4 a� 7


}

void DeleateBackground(void) {

	MBED_LCD_FillRect(0, 0, 128, 32, false);

	MBED_LCD_VideoRam2LCD();

}

void DrawTrace(void)
{
	DeleateBackground();

	for(int i = 0;i<LCD_WIDTH-1;i++)
	{
		arrayOSC[i] = arrayOSC[i+1];
	}

	arrayOSC[LCD_WIDTH-1] = average_Array;

	for(int i = 0;i<LCD_WIDTH;i++)
	{
		MBED_LCD_PutPixel(i,LCD_HEIGHT - arrayOSC[i],true);
	}

	MBED_LCD_VideoRam2LCD();
}

int main(void) {


	SetClock100MHz(clockSourceHSE);
   SystemCoreClockUpdate();
	SysTick_Config(GetBusClock(timersClockAPB1) / 1000);  // SysTick nastaven na 1   [ms]
	Usart2Initialization(38400);


	//NVIC_EnableIRQ(DMA2_Stream0_IRQn); // povol�m handler od DMA�ka

	for(int i = 0;i<LCD_WIDTH;i++)
	{
		arrayOSC[i] = 16;
	}

	///////////////// DMA ////////////////////////////////////////////////////////////////////////////////

	NVIC_EnableIRQ(DMA2_Stream0_IRQn); // povol�m handler od DMA�ka

	if (!(RCC->AHB1ENR & RCC_AHB1ENR_DMA2EN)) {
			RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
			RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA2RST;
			RCC->AHB1RSTR &= ~RCC_AHB1RSTR_DMA2RST;
		}

		DMA2_Stream0->CR &= ~DMA_SxCR_EN; // EN = 0 !!, jinak nepujdou zmeny
		DMA2_Stream0->CR &= ~DMA_SxCR_CHSEL; // CHSEL = 000 = kan�l 0
		DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0; // MSIZE = 01 = 16bit
		DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0; // PSIZE = 01
		DMA2_Stream0->CR |= DMA_SxCR_MINC; // MINC = 1 = memory increment
		DMA2_Stream0->CR &= ~DMA_SxCR_PINC; // PINC = 0
		DMA2_Stream0->CR &= ~DMA_SxCR_DIR; // DIR = 00 = pheriph to memory
		DMA2_Stream0->CR |= DMA_SxCR_CIRC; // cirkula�n� m�d
		DMA2_Stream0->CR |= DMA_SxCR_TCIE; // P�eru�en� �pln�ho napln�n�
		DMA2_Stream0->CR |= DMA_SxCR_HTIE; // P�eru�en� polovi�n�ho napln�n�

		DMA2_Stream0->NDTR = ARRAY_SIZE; // velikost bufferu
		DMA2_Stream0->PAR = (uint32_t)&ADC1->DR; // Adresa registru DR (nebo JDR1..)
		DMA2_Stream0->M0AR = (uint32_t) arrayAD; // Adresa bufferu

		DMA2->LIFCR = DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTEIF0| DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0; // Shozen� v�ech p��znak� "interrupty"
		// LIFCR pro kan�ly 0 a� 3
		// HIFCR pro kan�ly 4 a� 7
		DMA2_Stream0->CR |= DMA_SxCR_EN; // povolen� DMA

    ///////////////// DMA ////////////////////////////////////////////////////////////////////////////////
	///////////////// AD /////////////////////////////////////////////////////////////////////////////////
	GPIOConfigurePin(POT_LEFT, ioPortAnalog);
	GPIOConfigurePin(POT_RIGHT, ioPortAnalog);

	if (!(RCC->APB2ENR & RCC_APB2ENR_ADC1EN)) {
		RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
		RCC->APB2RSTR |= RCC_APB2RSTR_ADCRST; // ADCRST je reset bit pro v�echny ADC
		RCC->APB2RSTR &= ~RCC_APB2RSTR_ADCRST; //
	}

	ADC1->CR1 = 0 | ADC_CR1_SCAN; // povolen� pr�ce s SQR kan�ly
	ADC1->CR1 |= ADC_CR1_EOCIE;
	ADC1->CR2 = 0; // ALIGN = 0 (zarovn�n� vpravo)
	ADC1->CR2 |= ADC_CR2_DMA; // povolen� DMA pro ADC
	ADC1->CR2 |= ADC_CR2_DDS; // aby st�le p�ev�d�l i v cirkula�n�m re�imu
  ADC1->CR2 |= ADC_CR2_EXTEN_0; // Triger rising edge (AD spu�t�n� od TIM2)
	ADC1->CR2 |= (ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2); // aby mohl b�t AD spu�t�n� od TIM2 pomoc� TRGO
	ADC1->SMPR1 = 0; // ur�en� doby p�evodu, chceme pouze pro kan�l 0 tak�e SMPR2
	ADC1->SMPR2 = ADC_SMPR2_SMP0_1; // 010, pro kan�l nula = 28 cyklu

	ADC1->SQR1 = 0; // L = 0000 = 1 konverze
	ADC1->SQR2 = 0;
	ADC1->SQR3 = 0; // SQ1 = 00000 = AD kanal 0


    ADC->CCR |= ADC_CCR_ADCPRE_0; // TSVREFE = 0 (neni teplotn� sensor, neni intern� reference), ADCPRE = 1 (APB2 / 4)
	// CCR spole�n� pro v�echny, proto nem� ��slo
	ADC1->CR2 |= ADC_CR2_ADON; // zapnuti AD
	///////////////// AD /////////////////////////////////////////////////////////////////////////////////
	///////////////// LCD ////////////////////////////////////////////////////////////////////////////////
	if (!MBED_LCD_init()) {while (1);}
	MBED_LCD_InitVideoRam(0x00);    // fill content with 0 = clear emory buffer

	DeleateBackground();

	///////////////// LCD ////////////////////////////////////////////////////////////////////////////////
	///////////////// TIM2 ///////////////////////////////////////////////////////////////////////////////
	if (!(RCC->APB1ENR & RCC_APB1ENR_TIM2EN))
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
		RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
		RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
	}

	TIM2->CR1 = TIM_CR1_DIR; // DIR = 1 = dekrementace
	TIM2->CNT &= ~TIM_CNT_CNT; // pro jistotu nulovani CNT
  TIM2->PSC = (GetBusClock(timersClockAPB1)/ 1000000)-1;         // PSC: 1   [us]    =>      1     [MHz]
  TIM2->ARR = 1000 - 1;                                          // ARR: 1   [ms]    =>      1000  [Hz]
	//TIM2->DIER = TIM_DIER_UIE; // Nastaven� generov�n� p�eru�en� ven, sm�r do j�dra (jedni�ka)
	TIM2->CR2 |= TIM_CR2_MMS_1;
	TIM2->CR1 |= TIM_CR1_CEN;

	///////////////// TIM2 ///////////////////////////////////////////////////////////////////////////////

	uint32_t tm = 20;

	puts("Zacatek aplikace\n");

	while (1) {

		if (ticks >= tm) {
			tm = ticks + 20;


		 printf("Value:  %d\n",(int)average_Array);
	     DrawTrace();

		}

	}
}

