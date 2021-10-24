/*
******************************************************************************
File:     main.c
Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2019-02-27

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


#include "mina_shield_mbed.h"

volatile uint32_t ticks = 0; // prom�n� kter� symbolizuje dob�hnut� hodin
// "volatile" znamen� �e kompil�tor tuto prom�nou nebude optimalizovat, to je t�eba, proto�e by jinak mohl kompil�tor doj�t k chybn�m z�v�r�m o fknci n�kter�ch programov�ch konstrukc� a nespr�vn� program optimalizovat

void SysTick_Handler(void) // TODO funkce mus� m�t p�esn� takov�to n�zev, jinak se p�i p�eru�en� vol� defalutn� nastaven�
{
	ticks++;
}

int main(void)
{

	// PWM: ARR jsou v��ka schod�, CCRx je kdy se pulz v dan� period� "vypne", pokud je CCRx v pulce schod�, d�v� 50%, pokud v�c, st��d� bude v�t��

	uint32_t timeConstant = 100;

	SystemCoreClockUpdate();

	SysTick_Config(SystemCoreClock / 1000);

	uint32_t apb2 = SystemCoreClock; //TODO dopln podle RCC (16MHz)

	if (!(RCC->APB2ENR & RCC_APB2ENR_TIM1EN)) // v DataSheet str15 je vid�t �e TIM1 je na sb�rnici APB2
	{
			RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
			RCC->APB2RSTR |= RCC_APB2RSTR_TIM1RST;
			RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM1RST;
	} // nastaven TIM1

		TIM1->CR1 = 0 | TIM_CR1_ARPE; // up counter(po��t� nahoru)
		TIM1->CR2 = 0;
		TIM1->PSC = apb2 / 1E6 - 1; // (1us) => MHz
		TIM1->ARR = 128-1; // (1000-1 => p�ete�e ka�dou 1ms => kHz)
		TIM1->CR1 = TIM_CR1_CEN;

		TIM1->BDTR |= TIM_BDTR_MOE; //nutno nastavit, na jedna (povoluje OC v�stup�), ur�eno k jednoduch�mu odpojen� motorov�ch budi��

		GPIOConfigurePin(RGBLED_BLUE, ioPortAlternatrPushPull);
		GPIOConfigureAlternativFunction(RGBLED_BLUE, 1); // AF01 odpovida TIM1_CH2N => datasheet


		TIM1->CCMR1 &= ~TIM_CCMR1_OC2M; // Pozor!! nutno OC2M proto�e pot�ebueme druh� kan�l!!!!
		TIM1->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1; // _2 a _1 jako prvni a druhy bit, 110 � PWM 1
		TIM1->CCER |= TIM_CCER_CC2E; // k�n�l 2 enable (mus� b�t jenom E!!!)
		TIM1->CCR2 = TIM1->ARR / 2; // polovina TIM3 ARR, tak�e st��da 50%

	    // RM str 355

		if (!(RCC->APB1ENR & RCC_APB1ENR_TIM3EN)) // v DataSheet str15 je vid�t �e TIM1 je na sb�rnici APB2
		{
				RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
				RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
				RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;
		} // nastaven TIM3

			TIM3->CR1 = 0 | TIM_CR1_ARPE; // up counter(po��t� nahoru)
			TIM3->CR2 = 0;
			TIM3->PSC = apb2 / 1E6 - 1; // (1us) => MHz
			TIM3->ARR = 128-1; // (p�ete�e ka�dou 256us)
			TIM3->CR1 = TIM_CR1_CEN;

			GPIOConfigurePin(RGBLED_RED, ioPortAlternatrPushPull);
			GPIOConfigureAlternativFunction(RGBLED_RED, 2); // AF02 odpovida TIM3_CH1N => datasheet


			TIM3->CCMR1 &= ~TIM_CCMR1_OC1M; // Pozor!! nutno OC1M proto�e pot�ebueme prvn� kan�l!!!!
			TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // _2 a _1 jako prvni a druhy bit, 110 � PWM 1
			TIM3->CCER |= TIM_CCER_CC1E; // k�n�l 1 enable (mus� b�t jenom E!!!)
			TIM3->CCR1 =  TIM3->ARR / 2; // polovina TIM3 ARR, tak�e st��da 50%

			GPIOConfigurePin(RGBLED_GREEN, ioPortAlternatrPushPull);
			GPIOConfigureAlternativFunction(RGBLED_GREEN, 2); // AF02 odpovida TIM3_CH2N => datasheet


			TIM3->CCMR1 &= ~TIM_CCMR1_OC2M; // Pozor!! nutno OC2M proto�e pot�ebueme druh� kan�l!!!!
			TIM3->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1; // _2 a _1 jako prvni a druhy bit, 110 � PWM 1
			TIM3->CCER |= TIM_CCER_CC2E; // k�n�l 1 enable (mus� b�t jenom E!!!)
			TIM3->CCR2 = TIM3->ARR / 2; // polovina TIM3 ARR, tak�e st��da 50%

		    // RM str 355

			uint8_t b = 0;


			while (1) {

				if (ticks >= timeConstant) {
							timeConstant = ticks + 50;

					        b+=4;

					        TIM3->CCR1 = b;
							TIM3->CCR2 = b;
							TIM1->CCR2 = b;

							if(b>=127)
							{
							b = 0;
							}

					}

		}
	}
