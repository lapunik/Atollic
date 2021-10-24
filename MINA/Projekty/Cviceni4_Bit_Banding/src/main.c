#include "mina_shield_mbed.h"
/*
#define GPIOC_ODR_B2 (*(uint32_t *)(0x42000000 + ((uint32_t)(&(GPIOC->ODR)) - 0x40000000) * 32 + 4 * 2))
#define GPIOC_ODR_B3 (*(uint32_t *)(PERIPH_BB_BASE + (GPIOC_BASE + 0x14 - PERIPH_BASE) * 32 + 4 * 3)) // periph base/ bb base je jen náhrada za ukazatele na zaèátek registrù 40000000 bla bla
*/ //tohle je všechno super a funguje to, ale do budoucna budeme používat makra viz stm_core
volatile uint32_t ticks = 0;

void SysTick_Handler(void) {
	ticks++;
}

void TIM3_IRQHandler(void)
{
TIM3->SR &= ~TIM_SR_UIF;
//GPIOC->ODR ^= 1 << 3; // jebe se
//GPIOC_ODR_B3 ^= 1; // už se nejebe
BB_REG(GPIOC->ODR, 3) ^= 1; // už se nejebe a je to pøes makra
}

int main(void) {

    SystemCoreClockUpdate();

    SetClockHSI();

	SysTick_Config(SystemCoreClock / 1000);

	GPIOConfigurePin(GPIOC, 2, ioPortOutputPushPull);
	GPIOConfigurePin(GPIOC, 3, ioPortOutputPushPull);


	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	TIM3->CR1 = TIM_CR1_DIR; // DIR = 1 = cnt-down
	TIM3->PSC = 1; // : 2 = 8MHz
	//TIM3->ARR = 40 - 1; // : 40 = 200kHz pìkný prùbìh
	//TIM3->ARR = 20 - 1; // : 20 = 400kHz už je to rozbitý, jen handler
	TIM3->ARR = 50 - 1; // : 50 = 160kHz divný vìci(nepravidelná velikost obdélníkù na hendler
	// je to z dùvodu že kdo poslední má pøístup, ten vyhrál, handler nìco nahraje ale hned v zápìtí mu to while zase pøepíše
	// proto je tady nìco jako bit_banding (šlo by i na tu dobu zakázat pøerušení ale to je na vyližprdel)
	// bitbanding je takový to zrdcadlení bitù jak jsme si øíkali v MPP
	TIM3->CR1 |= TIM_CR1_CEN; // CEN = 1 = enable
	TIM3->DIER = TIM_DIER_UIE;

	NVIC_EnableIRQ(TIM3_IRQn);

	while (1)
	{
	//GPIOC->ODR ^= 1 << 2; // jebe se
//		GPIOC_ODR_B2 ^= 1; // už se nejebe... stejný zápis jako "GPIOC_ODR_B2 = !GPIOC_ODR_B2;" jen trochu jinak
		BB_REG(GPIOC->ODR, 2) ^= 1; // už se nejebe a k tomu je to pøes makra
	}


}
