#include "mina_shield_mbed.h"
#include "nucleo_usart.h"

volatile uint32_t ticks = 0;

void SysTick_Handler(void)
{
	ticks++;
}

void TIM3_IRQHandler(void)
{
	TIM3->SR &= ~TIM_SR_UIF;
	BB_REG(GPIOB->ODR, 4) ^= 1; // MBED_LED_R
}

void EXTI9_5_IRQHandler(void) // PC7 = zelená led proto EXIT 5 až 9
// 0 až 4 jsou rozdìleny po jedné, pak je 5 až 9 a 10 až 16.. na ty se pak musím ptát v pending registru od kterého to bylo
// do jednoho bloku jdou všechny PA/B/C.. ale jsou slouèenné do (0/1/2/3/4/5 až 9/10 až 16)
// Blok SYCFG_EXTI pak EXTI
{
if (EXTI->PR & EXTI_PR_PR5)
{
EXTI->PR = EXTI_PR_PR5; // RM10.3.6 - This bit is cleared by programming it to ‘1’.
BB_REG(GPIOC->ODR, 7) = 0; // ON
while(GPIOB->IDR & (1 << 5))
;
BB_REG(GPIOC->ODR, 7) = 1; // OFF
}
}

int main(void)
{

	if (!(RCC->APB1ENR & RCC_APB1ENR_TIM3EN))
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
		RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
		RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;
	}

	if (!(RCC->APB2ENR & RCC_APB2ENR_SYSCFGEN))
	{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	RCC->APB2RSTR |= RCC_APB2RSTR_SYSCFGRST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_SYSCFGRST;
	}

	GPIOConfigurePin(RGBLED_RED, ioPortOutputPushPull);
	GPIOConfigurePin(RGBLED_GREEN, ioPortOutputPushPull);
	GPIOConfigurePin(JOY_CENTER, ioPortInputPullUp);
	BB_REG(GPIOB->ODR, 4) = 1;

	TIM3->PSC = /*GetBusClock(timersClockAPB1)*/16E6 / 1000; // 1ms
	TIM3->ARR = 100 - 1; // 0.1 sec
	TIM3->DIER = TIM_DIER_UIE; // Nastavení generování pøerušení ven, smìr do jádra (jednièka)
	TIM3->CR1 |= TIM_CR1_CEN; // enable
	NVIC_EnableIRQ(TIM3_IRQn); // povolení správce zaøízení(pøerušovací kontroler), //IGQje typ pøerušení
    // TIM3_IRQn F3 ukáže posloupnost který interupt je duležitìjší (menší èíslo -> vyšší dùležitost)
	// EXTI  = 23.... udìlá se ikdyž se zrovna dìlá TIM_IRQn
	// TIM_IRQn = 29
	// oba mají prioritu nula,
	// kdo má vyšší prioritu(menší èíslo), ten vyhrává (neplést prioritu a posloupnost interuptù viz F3)

	//NVIC_SetPriority(TIM3_IRQn,0); // kdybych chtìl nastavit prioritu blikani cervenou

	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PB; // !!! registry 1...4, index 0...3 !!!!
	EXTI->RTSR |= EXTI_RTSR_TR5;
	EXTI->IMR |= EXTI_IMR_MR5;
	NVIC_EnableIRQ(EXTI9_5_IRQn);

	NVIC_SetPriority(EXTI9_5_IRQn,1); // nastaveni priority, zvýšíme èíslo, tím snížíme dùležitost, pokud zmáèknu a držím, sítí zelená, to je jasný, ale když má nastavenou prioritu vìtší než systik nebo blikání èervenou, tak bliká èervenou poøádad a poèítá taky poøát ale k tomu ještì svítí zelenou (tiká ale nevypisuje na terminal pochopitelnì - protože hlavni programova linka ma nejmensi prioritu)

	// nastavení o kterej bit (EXTI) znamená je ve slajdech

	uint32_t tm = 0;
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);

	// SysTick_IRQn = 15 defalutnì

	NVIC_SetPriority(SysTick_IRQn,0); // nastavení nejnižší priority SysTicku, "aby poèítal ikdyž budu držet tlèítko"

	// RM 10.1.3 je tabulka, co má jakou prioritu, popø. mùžu na "SysTick_IRQn" F3 a potom vybrat f411 a mám to tam

	/*
	 * pøehled:
	 *
	 * Systick:    -1  defalut priorita: 15
	 * EXTI:       23  defalut priorita: 0
	 * TIM3:       29  defalut priorita: 0
	 *
	*/
	Usart2Initialization(38400);

	while (1)
	{
       // 7.2.4 SYSCFG_EXTICR...

		if (ticks >= tm)
				{
					tm = ticks + 100;
					printf("%d\n",(int)ticks);
				}
	}
}

