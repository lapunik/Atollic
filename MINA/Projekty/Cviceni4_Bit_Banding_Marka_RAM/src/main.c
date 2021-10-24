#include "mina_shield_mbed.h"
#include "nucleo_usart.h"
/*
#define GPIOC_ODR_B2 (*(uint32_t *)(0x42000000 + ((uint32_t)(&(GPIOC->ODR)) - 0x40000000) * 32 + 4 * 2))
#define GPIOC_ODR_B3 (*(uint32_t *)(PERIPH_BB_BASE + (GPIOC_BASE + 0x14 - PERIPH_BASE) * 32 + 4 * 3)) // periph base/ bb base je jen n�hrada za ukazatele na za��tek registr� 40000000 bla bla
 */ //tohle je v�echno super a funguje to, ale do budoucna budeme pou��vat makra viz stm_core
volatile uint32_t ticks = 0;

void SysTick_Handler(void) {
	ticks++;
}

int main(void) {

	uint32_t tm = 0;
	uint8_t b = 'a';
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
	Usart2Initialization(38400);

	unsigned int temp = 0;

	//uint32_t * adr = (uint32_t *)(SRAM_BB_BASE + ((uint32_t)(&b) - SRAM_BASE) * 32 + 4 * (5));

	uint32_t * adr = &BB_RAM(&b,5); // je po��d stejn� hodnota, jen p�t� bit ud�v� jestli velk� nebo mal� p�smeno

	while (1)
	{
		if (ticks >= tm)
		{
			tm = ticks + 100;
			putchar(b);
			b++;
			if (b > 'z')
			{
				b = 'a';
			}
			*adr ^= 1; // krokovani v asembleru jedine bez optimalizace!
			// BB_RAM(&b,5) ^= 1
		}
	}
}



