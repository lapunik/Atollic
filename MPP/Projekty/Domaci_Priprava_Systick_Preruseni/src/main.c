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
#include "stm_core.h"
#include "mpp_shield.h"
#include "system_stm32f4xx.h"

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */

volatile bool tickElapsed = false; // prom�n� kter� symbolizuje dob�hnut� hodin
// "volatile" znamen� �e kompil�tor tuto prom�nou nebude optimalizovat, to je t�eba, proto�e by jinak mohl kompil�tor doj�t k chybn�m z�v�r�m o fknci n�kter�ch programov�ch konstrukc� a nespr�vn� program optimalizovat

void SysTick_Handler(void) // tato funkce mus� m�t p�esn� takov�to n�zev, jinak se p�i p�eru�en� vol� defalutn� nastaven�
{
tickElapsed = true;
}
int main(void)
{

SystemCoreClockUpdate();

SysTick_Config(SystemCoreClock); // nastaven� �asova�e pro p�eru�en�

GPIOConfigurePin(RGBLED_BLUE,ioPortOutputPushPull);

  /* Infinite loop */
while (1)
{

	if (tickElapsed)
	{
	tickElapsed = false;
	GPIOToggle(RGBLED_BLUE);
	}


}

}
