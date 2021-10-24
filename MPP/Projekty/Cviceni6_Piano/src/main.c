/*
 ******************************************************************************
 File:     main.c
 Info:     Generated by Atollic TrueSTUDIO(R) 9.3.0   2019-03-06

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
#include "mpp_shield.h"
#include "system_stm32f4xx.h"
#include "nucleo_usart.h"

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */


   volatile bool onBuzz = false;

   void TIM3_IRQHandler(void) // obsluha preruseni od TIM3
   {
    TIM3->SR &= ~TIM_SR_UIF; // shodit priznak !!
    if(onBuzz)
    {
    GPIOToggle(BUZZER); // pipak
    }
   }



   void TIM4_IRQHandler(void) //
   {
    onBuzz = false; // pipak
    TIM4->SR &= ~TIM_SR_UIF; // shodit priznak !!
    TIM4->CR1 &= ~TIM_CR1_CEN;
    }

int main(void) {

	GPIOConfigurePin(BUZZER,ioPortOutputPushPull);
	GPIOConfigurePin(BUTTON_LEFT,ioPortInputFloat);

	if (!(RCC->APB1ENR & RCC_APB1ENR_TIM3EN))
	{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;
	}

	if (!(RCC->APB1ENR & RCC_APB1ENR_TIM4EN))
	{
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM4RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM4RST;
	}


	TIM3->DIER = TIM_DIER_UIE; // nastaveni preruseni
	NVIC_EnableIRQ(TIM3_IRQn); // nastaveni preruseni

	TIM3->CR1 = TIM_CR1_DIR; // DIR = 1 = dekrementace
	TIM3->CR2 = 0; //prost� jen reset
	TIM3->PSC = SystemCoreClock/1000000-1 ; //
	TIM3->ARR = 1000-1; // jedna milisekunda
	TIM3->CR1 |= TIM_CR1_CEN; // enable


	TIM4->DIER = TIM_DIER_UIE; // nastaveni preruseni
	NVIC_EnableIRQ(TIM4_IRQn); // nastaveni preruseni

	TIM4->CR1 = TIM_CR1_DIR; // DIR = 1 = dekrementace
	TIM4->CR2 = 0; //prost� jen reset
	TIM4->PSC = SystemCoreClock/1000-1 ;
	TIM4->ARR = 200-1; // 0,2 sekundy
	TIM4->CR1 &= ~TIM_CR1_CEN; // disenable

	Usart2Initialization(38400);

    puts("\nStart App\n");

	while (1) {


		if (IsUsart2Recived()) { // funkci mus�m volat a� kdy� v�m �e tam n�jak� data �ekaj�, jinak by tam �ekal dokud nep�ijou a program by se zastavil proto testovac� funkce, kdybych dal if testovani do funkce pro cteni, nepoznal bych kdy prisla nula a kdy mi to vratilo nulu jako �e podminka nebyla splnena


			char x = Usart2Recived();

			bool correctButton = true;

			uint frequency;

			if(x == 's')
			{
				printf("C2");
				frequency = 523;

			}
			else if (x == 'd')
			{
				printf("D2");
				frequency = 587;
			}
			else if (x == 'f')
			{
				printf("E2");
				frequency = 659;
			}
			else if (x == 'g')
			{
				printf("F2");
				frequency = 698;
			}
			else if(x == 'h')
			{
				printf("G2");
				frequency = 784;
			}
			else if (x == 'j')
			{
				printf("A2");
				frequency = 880;
			}
			else if (x == 'k')
			{
				printf("H2");
				frequency = 988;
			}
			else if (x == 'l')
			{
				printf("C3");
				frequency = 1047;
			}
			else
			{
				correctButton = false;
			}

			if(correctButton){

				TIM4->CR1 |= TIM_CR1_CEN;

				onBuzz = true;

			    TIM4->SR &= ~TIM_SR_UIF; // shodit priznak !!

			    TIM4->CNT = 255;

			printf("\n");
			float time =((1/((float)frequency))*1000000);
			TIM3->ARR = time;
			}
		}

	}
}
