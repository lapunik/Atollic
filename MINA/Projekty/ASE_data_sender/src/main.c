#include "mina_shield_mbed.h"
#include "nucleo_usart.h"
#include "mbed_shield_lcd.h"
#include "string.h"
#include "stdio.h"

volatile uint32_t ticks = 0;

void SysTick_Handler(void) {
	ticks++;
}


int main(void) {



    SystemCoreClockUpdate();
	SysTick_Config(GetBusClock(timersClockAPB1) / 1000);  // SysTick nastaven na 1   [ms]
	Usart2Initialization(38400);

	uint32_t timeBase = 1000;

	uint8_t value = 0;

	while (1) {

		if (ticks >= timeBase) {
			timeBase = ticks + 1000;

		 printf("%d\n",value);

		 value++;

		 if(value>40)
		 {
		     value = 0;
		 }

		}

	}
}

