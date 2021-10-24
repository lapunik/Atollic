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

void EXTI9_5_IRQHandler(void) // PC7 = zelen� led proto EXIT 5 a� 9
// 0 a� 4 jsou rozd�leny po jedn�, pak je 5 a� 9 a 10 a� 16.. na ty se pak mus�m pt�t v pending registru od kter�ho to bylo
// do jednoho bloku jdou v�echny PA/B/C.. ale jsou slou�enn� do (0/1/2/3/4/5 a� 9/10 a� 16)
// Blok SYCFG_EXTI pak EXTI
{
if (EXTI->PR & EXTI_PR_PR5)
{
EXTI->PR = EXTI_PR_PR5; // RM10.3.6 - This bit is cleared by programming it to �1�.
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
	TIM3->DIER = TIM_DIER_UIE; // Nastaven� generov�n� p�eru�en� ven, sm�r do j�dra (jedni�ka)
	TIM3->CR1 |= TIM_CR1_CEN; // enable
	NVIC_EnableIRQ(TIM3_IRQn); // povolen� spr�vce za��zen�(p�eru�ovac� kontroler), //IGQje typ p�eru�en�
    // TIM3_IRQn F3 uk�e posloupnost kter� interupt je dule�it�j�� (men�� ��slo -> vy��� d�le�itost)
	// EXTI  = 23.... ud�l� se ikdy� se zrovna d�l� TIM_IRQn
	// TIM_IRQn = 29
	// oba maj� prioritu nula,
	// kdo m� vy��� prioritu(men�� ��slo), ten vyhr�v� (nepl�st prioritu a posloupnost interupt� viz F3)

	//NVIC_SetPriority(TIM3_IRQn,0); // kdybych cht�l nastavit prioritu blikani cervenou

	SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PB; // !!! registry 1...4, index 0...3 !!!!
	EXTI->RTSR |= EXTI_RTSR_TR5;
	EXTI->IMR |= EXTI_IMR_MR5;
	NVIC_EnableIRQ(EXTI9_5_IRQn);

	NVIC_SetPriority(EXTI9_5_IRQn,1); // nastaveni priority, zv���me ��slo, t�m sn��me d�le�itost, pokud zm��knu a dr��m, s�t� zelen�, to je jasn�, ale kdy� m� nastavenou prioritu v�t�� ne� systik nebo blik�n� �ervenou, tak blik� �ervenou po��dad a po��t� taky po��t ale k tomu je�t� sv�t� zelenou (tik� ale nevypisuje na terminal pochopiteln� - proto�e hlavni programova linka ma nejmensi prioritu)

	// nastaven� o kterej bit (EXTI) znamen� je ve slajdech

	uint32_t tm = 0;
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);

	// SysTick_IRQn = 15 defalutn�

	NVIC_SetPriority(SysTick_IRQn,0); // nastaven� nejni��� priority SysTicku, "aby po��tal ikdy� budu dr�et tl��tko"

	// RM 10.1.3 je tabulka, co m� jakou prioritu, pop�. m��u na "SysTick_IRQn" F3 a potom vybrat f411 a m�m to tam

	/*
	 * p�ehled:
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

