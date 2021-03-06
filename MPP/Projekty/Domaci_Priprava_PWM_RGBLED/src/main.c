/*
 ******************************************************************************
 File:     main.c
 Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2019-03-15

 The MIT License (MIT)
 Copyright (c) 2019 STMicroelectronics

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 ******************************************************************************
 */

/* Includes */
#include "stm_core.h"
#include "mpp_shield.h"
#include "system_stm32f4xx.h"

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */

volatile uint32_t ticks = 0; // prom?n? kter? symbolizuje dob?hnut? hodin
// "volatile" znamen? ?e kompil?tor tuto prom?nou nebude optimalizovat, to je t?eba, proto?e by jinak mohl kompil?tor doj?t k chybn?m z?v?r?m o fknci n?kter?ch programov?ch konstrukc? a nespr?vn? program optimalizovat

void SysTick_Handler(void) // tato funkce mus? m?t p?esn? takov?to n?zev, jinak se p?i p?eru?en? vol? defalutn? nastaven?
{
	ticks++;
}

int main(void) {

	uint32_t timeConstant = 50;
	uint32_t apb2 = SystemCoreClock; //TODO dopln podle RCC
	uint8_t b = 0;

	SystemCoreClockUpdate();

	SysTick_Config(SystemCoreClock / 1000);

	if (!(RCC->APB2ENR & RCC_APB2ENR_TIM1EN)) {
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
		RCC->APB2RSTR |= RCC_APB2RSTR_TIM1RST;
		RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM1RST;
	}

	TIM1->CR1 = 0 | TIM_CR1_ARPE;
	TIM1->CR2 = 0;
	TIM1->PSC = apb2 / 1E6 - 1; // (1us)
	TIM1->ARR = 256 - 1; // (256us)
	TIM1->CR1 = TIM_CR1_CEN;

	TIM1->BDTR |= TIM_BDTR_MOE; //nutno nastavit, na jedna (povoluje OC v?stup?), ur?eno k jednoduch?mu odpojen? motorov?ch budi??

	GPIOConfigurePin(RGBLED_BLUE, ioPortAlternatrPushPull);
	GPIOConfigureAlternativFunction(RGBLED_BLUE, 1); // AF01 odpovida TIM1_CH1N

	TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // 110 ? PWM 1
	TIM1->CCER |= TIM_CCER_CC1NE; // povol komplementarni vystup
	TIM1->CCR1 = 0; //TIM1->ARR / 2;

	GPIOConfigurePin(RGBLED_GREEN, ioPortAlternatrPushPull);
	GPIOConfigureAlternativFunction(RGBLED_GREEN, 1); // AF01 odpovida TIM1_CH2N

	TIM1->CCMR1 &= ~TIM_CCMR1_OC2M;
	TIM1->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1; // 110 - PWM 1
	TIM1->CCER |= TIM_CCER_CC2NE; // povol komplementarni vystup
	TIM1->CCR2 = 0; //TIM1->ARR / 2;

	GPIOConfigurePin(RGBLED_RED, ioPortAlternatrPushPull);
	GPIOConfigureAlternativFunction(RGBLED_RED, 1); // AF01 odpovida TIM1_CH3N

	TIM1->CCMR2 &= ~TIM_CCMR2_OC3M;
	TIM1->CCMR2 |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1; // 110 - PWM 1
	TIM1->CCER |= TIM_CCER_CC3NE; // povol komplementarni vystup
	TIM1->CCR3 = 0; //TIM1->ARR / 2;


	while (1) {
		if (ticks >= timeConstant) {
			timeConstant = ticks + 5; // 20ms
			b++;

			TIM1->CCR1 = b;
			TIM1->CCR2 = b;
			TIM1->CCR3 = b;


		}
	}
}
