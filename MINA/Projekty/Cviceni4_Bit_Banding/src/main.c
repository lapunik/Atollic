#include "mina_shield_mbed.h"
/*
#define GPIOC_ODR_B2 (*(uint32_t *)(0x42000000 + ((uint32_t)(&(GPIOC->ODR)) - 0x40000000) * 32 + 4 * 2))
#define GPIOC_ODR_B3 (*(uint32_t *)(PERIPH_BB_BASE + (GPIOC_BASE + 0x14 - PERIPH_BASE) * 32 + 4 * 3)) // periph base/ bb base je jen n�hrada za ukazatele na za��tek registr� 40000000 bla bla
*/ //tohle je v�echno super a funguje to, ale do budoucna budeme pou��vat makra viz stm_core
volatile uint32_t ticks = 0;

void SysTick_Handler(void) {
	ticks++;
}

void TIM3_IRQHandler(void)
{
TIM3->SR &= ~TIM_SR_UIF;
//GPIOC->ODR ^= 1 << 3; // jebe se
//GPIOC_ODR_B3 ^= 1; // u� se nejebe
BB_REG(GPIOC->ODR, 3) ^= 1; // u� se nejebe a je to p�es makra
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
	//TIM3->ARR = 40 - 1; // : 40 = 200kHz p�kn� pr�b�h
	//TIM3->ARR = 20 - 1; // : 20 = 400kHz u� je to rozbit�, jen handler
	TIM3->ARR = 50 - 1; // : 50 = 160kHz divn� v�ci(nepravideln� velikost obd�ln�k� na hendler
	// je to z d�vodu �e kdo posledn� m� p��stup, ten vyhr�l, handler n�co nahraje ale hned v z�p�t� mu to while zase p�ep�e
	// proto je tady n�co jako bit_banding (�lo by i na tu dobu zak�zat p�eru�en� ale to je na vyli�prdel)
	// bitbanding je takov� to zrdcadlen� bit� jak jsme si ��kali v MPP
	TIM3->CR1 |= TIM_CR1_CEN; // CEN = 1 = enable
	TIM3->DIER = TIM_DIER_UIE;

	NVIC_EnableIRQ(TIM3_IRQn);

	while (1)
	{
	//GPIOC->ODR ^= 1 << 2; // jebe se
//		GPIOC_ODR_B2 ^= 1; // u� se nejebe... stejn� z�pis jako "GPIOC_ODR_B2 = !GPIOC_ODR_B2;" jen trochu jinak
		BB_REG(GPIOC->ODR, 2) ^= 1; // u� se nejebe a k tomu je to p�es makra
	}


}
