#include "mina_shield_mbed.h"
#include "nucleo_usart.h"
#include "string.h"
#include "stdio.h"

#define BLOCK_BYTES_LEN 8192
#define BLOCK_COPY_COUNT 100
uint8_t blok_src[BLOCK_BYTES_LEN];
uint8_t blok_dest[BLOCK_BYTES_LEN];

volatile uint32_t ticks = 0;

void SysTick_Handler(void) {
	ticks++;
}

int main(void) {

	SystemCoreClockUpdate();

	// za normálních okolností tohle tady nebude a bude to odkomentováno v usart.c !!!!!!!

	// v projktu by mìlo být definovaný HSE_VALUE 8000000 všude!!! ne jenom v stm_core.c !!!!!!!!

	setvbuf(stdout,NULL,_IONBF,0); // datový proud, nepoviný parametr, typ(nebufrovat), délka v tomto pøípadì nesmyslný argument, takže nula
	setvbuf(stdin,NULL,_IONBF,0); // používám pro vstupní i výstupní (printf a podobnì všechno používá stdint/stdout)

	for(int time = 0;time<2;time++)
	{

		if(time == 0)
		{
			// HSI 16 000 000

			SetClockHSI();
			SysTick_Config(SystemCoreClock / 1000);
			Usart2Initialization(38400);
			puts("\nHSI 16Mhz  " __DATE__ " " __TIME__ "\r\n");

		}
		else
		{
			// HSE 100 000 000

			SetClock100MHz(clockSourceHSE);
			SysTick_Config(100000000 / 1000);
			Usart2Initialization(38400);
			puts("\n\nHSE 100Mhz  " __DATE__ " " __TIME__ "\r\n");
		}


	uint32_t t5 = 0;
	uint32_t t4 = 0;
	uint32_t t2 = 0;
	uint32_t t3 = 0;
	uint32_t t1 = 0;

	t1 = ticks;

	for (int i = 0; i < BLOCK_BYTES_LEN; i++) {
		blok_src[i] = i; // naplneni zdrojového pole
		blok_dest[i] = 0; // vynulovani ciloveho pole
	}

	for (int x = 0; x < 100; x++) {
		for (int i = 0; i < BLOCK_BYTES_LEN; i++)
			blok_dest[i] = blok_src[i];
	}

	t1 = ticks-t1;
	t2 = ticks;

	for (int x = 0; x < 100; x++) {
		uint8_t *pd = blok_dest, *ps = blok_src;
		for (int i = 0; i < BLOCK_BYTES_LEN; i++) {
			*pd++ = *ps++;
		}
	}

	t2 = ticks-t2;
	t3 = ticks;

	for (int x = 0; x < 100; x++) {
		uint32_t *pd = (uint32_t *) blok_dest, *ps = (uint32_t *) blok_src,
				*pf = ps + BLOCK_BYTES_LEN / 4;
		while (ps < pf) {
			*pd++ = *ps++;
		}
	}
	t3 = ticks - t3;

	// Teï pomocí DMA: ///////////////////////////////

	if (!(RCC->AHB1ENR & RCC_AHB1ENR_DMA2EN)) {
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
		RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA2RST;
		RCC->AHB1RSTR &= ~RCC_AHB1RSTR_DMA2RST;
	}

	DMA2_Stream6->CR &= DMA_SxCR_EN; // EN = 0 !!, jinak nepujdou zmeny
	DMA2_Stream6->CR = 0 | DMA_SxCR_CHSEL_2 // CHSEL = 100
			| DMA_SxCR_MSIZE_1 // MSIZE = 10 = 32bit
			| DMA_SxCR_PSIZE_1 // PSIZE = 10
			| DMA_SxCR_MINC // MINC = 1 = memory increment
			| DMA_SxCR_PINC // PINC = 0
			| DMA_SxCR_DIR_1 // DIR = 10 = memory 2 mem = PAR 2 M0AR
	;
	DMA2_Stream6->PAR = (uint32_t) blok_src;
	DMA2_Stream6->M0AR = (uint32_t) blok_dest;

	t4 = ticks;

	for (int x = 0; x < 100; x++) {

	DMA2_Stream6->NDTR = BLOCK_BYTES_LEN;
	DMA2->HIFCR = DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6
			| DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6;

	// LIFCR pro kanály 0 až 3
	// HIFCR pro kanály 4 až 7

	DMA2_Stream6->CR |= DMA_SxCR_EN;
	while (!(DMA2->HISR & DMA_HISR_TCIF6))
		;
	DMA2_Stream6->CR &= ~DMA_SxCR_EN;
	DMA2->HIFCR = DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6
			| DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6;

	USART2->CR3 &= ~USART_CR3_DMAT;
	}
	t4 = ticks-t4;

	t5 = ticks;

	for (int x = 0; x < 100; x++) {

	DMA2_Stream6->NDTR = BLOCK_BYTES_LEN / 4;
	DMA2->HIFCR = DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6
			| DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6;

	DMA2_Stream6->CR |= DMA_SxCR_EN;
	while (!(DMA2->HISR & DMA_HISR_TCIF6))
		;
	DMA2_Stream6->CR &= ~DMA_SxCR_EN;
	DMA2->HIFCR = DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6
			| DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6;

	USART2->CR3 &= ~USART_CR3_DMAT;
	}
	t5 = ticks-t5;

	///////////////////////////////////////////////////

	printf("Pres hranate zavorky: %d\nPres ukazatel na byte: %d\nPres ukazatele na 32bit najednou: %d\nPres DMA po jednom byte: %d\nPres DMA po 4 byte: %d\n",(int) t1, (int) t2, (int) t3, (int) t4, (int) t5);
	}
	while (1) {

	}
}

