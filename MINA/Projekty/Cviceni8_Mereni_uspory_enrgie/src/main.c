#include "mina_shield_mbed.h"


volatile uint32_t ticks = 0;

void SysTick_Handler(void) {
	ticks++;
}

int main(void) {



	// 16MHz:  Blik�: (6.79 a� 9.12 mA)  Neblik�: (6.45 mA) Sv�t�: (9.26 mA)
	// 100MHz: Blik�: (13.65 a� 16.1 mA) Neblik�: (23.1 mA) Sv�t�: (16.22 mA) // je fakt divn� �e kdy� sv�t� m� men�� spot�ebu ne� kdy� nesv�t�
	// 1MHz:   Blik�: (5.55 mA)          Neblik�: ( mA) Sv�t�: (6.85 mA)

	SetClock100MHz(clockSourceHSE);

	SystemCoreClockUpdate();


	//SysTick_Config(SystemCoreClock / 1000);
	//SysTick_Config(100000000 / 1000);
	//SysTick_Config(1000000/1000);

	RCC -> CFGR |= RCC_CFGR_HPRE_DIV16; // pro 1MHz

	//GPIOConfigurePin(BOARD_LED,ioPortOutputPushPull);

	uint32_t timeBase = 200;

	while (1) {

		if (ticks >= timeBase) {
			timeBase = ticks + 200;

			//GPIOToggle(BOARD_LED);
			//GPIOWrite(BOARD_LED,1);

		}

	}
}

