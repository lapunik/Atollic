#include "mina_shield_mbed.h"
#include "nucleo_usart.h"
#include "string.h"
#include "stdio.h"

int main(void) {

	//SystemCoreClockUpdate();
	//SysTick_Config(SystemCoreClock / 1000);

	// memory to memory um� pouze DMA2, DMA1 pouze memory to registr nebo registr to memory
	// pro ka�dou periferii m�m DMA jedna nebo dva a kan�l ur�eno v tabulce tab. 27/8 DMA1/2  (RM tab. 27/8, 9.3.3)
	// na jednou chanelu (��dku) si m��u vybrat jenom jeden DMA, nelze pou��t na jednom kan�lu v�ce DMA

	// shozen� v�ech do nuly p�ed za��tkem, doporu�en� postup:
	// DMA1->HIFCR = DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6 | DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6;

	// pokud chci d�lat zm�ny v nastaven� registru DMA, mus�m m�t EN (enable) v nule!

	// povolen� DMA usartu : 19.6.6 registr (USATR_CRx) bity (_DMAT_/DMAR)

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
	DMA1_Stream6->CR |= DMA_SxCR_EN; // tady d�m break point, ale i tak ode�le, proto�e DMA nen� z�visl� na b�hu progrmu
	// tady m�m zapnut� DMA, za�ne jakmile ho program povol�, pak u� si nev��m� co je v programu
	while (!(DMA1->HISR & DMA_HISR_TCIF6))
		;
	DMA1_Stream6->CR &= ~DMA_SxCR_EN;
	DMA1->HIFCR = DMA_HIFCR_CTCIF6 | DMA_HIFCR_CHTIF6 | DMA_HIFCR_CTEIF6
			| DMA_HIFCR_CDMEIF6 | DMA_HIFCR_CFEIF6;
	USART2->CR3 &= ~USART_CR3_DMAT;
	puts("\nFinished");

	// v projektu "stm32_flash.ld" je naps�no �e m�m jen 2kB pameti stack: _Min_Stack_Size = 0x400; /* required amount of stack */

	while (1) {

	}
}

