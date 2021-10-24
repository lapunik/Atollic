#include "mina_shield_mbed.h"
#include "nucleo_usart.h"
#include "string.h"
#include "stdio.h"

int main(void) {

	//SystemCoreClockUpdate();
	//SysTick_Config(SystemCoreClock / 1000);

	// memory to memory umí pouze DMA2, DMA1 pouze memory to registr nebo registr to memory
	// pro každou periferii mám DMA jedna nebo dva a kanál urèeno v tabulce tab. 27/8 DMA1/2  (RM tab. 27/8, 9.3.3)
	// na jednou chanelu (øádku) si mùžu vybrat jenom jeden DMA, nelze použít na jednom kanálu více DMA

	// shození všech do nuly pøed zaèátkem, doporuèený postup:
	// DMA1->HIFCR = DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6 | DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6;

	// pokud chci dìlat zmìny v nastavení registru DMA, musím mít EN (enable) v nule!

	// povolení DMA usartu : 19.6.6 registr (USATR_CRx) bity (_DMAT_/DMAR)

	Usart2Initialization(38400);

	puts("Start APP - MINA DMA test " __DATE__ " " __TIME__ "\r");
	char *lorem ="Ye to misery wisdom plenty polite to as. Prepared interest proposal it he exercise. My wishing an in attempt ferrars. Visited eat you why service looking engaged. At place no walls hopes rooms fully in. Roof hope shy tore leaf joy paid boy. Noisier out brought entered detract because sitting sir. Fat put occasion rendered off humanity has. "; // https://loremipsumgenerator.com/generator/?n=10&t=s
	if (!(RCC->AHB1ENR & RCC_AHB1ENR_DMA1EN)) {
		RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
		RCC->AHB1RSTR |= RCC_AHB1RSTR_DMA1RST;
		RCC->AHB1RSTR &= ~RCC_AHB1RSTR_DMA1RST;
	}
	DMA1_Stream6->CR &= DMA_SxCR_EN; // EN = 0 !!, jinak nepujdou zmeny
	DMA1_Stream6->CR = 0 | DMA_SxCR_CHSEL_2 // CHSEL = 100 (4)
			| 0 // MSIZE = 00 = 8bit
			| 0 // PSIZE = 00 = 8bit
			| DMA_SxCR_MINC // MINC = 1 = memory increment
			| 0 // PINC = 0
			| DMA_SxCR_DIR_0 // DIR = 01 = memory 2 peripheral
	;
	DMA1_Stream6->NDTR = strlen(lorem); // delka textu
	DMA1_Stream6->PAR = (uint32_t) &(USART2->DR); //adresa
	DMA1_Stream6->M0AR = (uint32_t) lorem;
	USART2->CR3 |= USART_CR3_DMAT;
	DMA1->HIFCR = DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6 | DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6;
	// place for breakpoint
	DMA1_Stream6->CR |= DMA_SxCR_EN; // tady dám break point, ale i tak odešle, protože DMA není závislé na bìhu progrmu
	// tady mám zapnutí DMA, zaène jakmile ho program povolí, pak už si nevšímá co je v programu
	while (!(DMA1->HISR & DMA_HISR_TCIF6))
		;
	DMA1_Stream6->CR &= ~DMA_SxCR_EN;
	DMA1->HIFCR = DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6
			| DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6;
	USART2->CR3 &= ~USART_CR3_DMAT;
	puts("\nFinished");

	// v projektu "stm32_flash.ld" je napsáno že mám jen 2kB pameti stack: _Min_Stack_Size = 0x400; /* required amount of stack */

	while (1) {

	}
}

