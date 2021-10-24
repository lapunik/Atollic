#include "mina_shield_mbed.h"


volatile uint32_t ticks = 0;

void SysTick_Handler(void) {
	ticks++;
}

int main(void) {



	// 16MHz:  Bliká: (6.79 až 9.12 mA)  Nebliká: (6.45 mA) Svítí: (9.26 mA)
	// 100MHz: Bliká: (13.65 až 16.1 mA) Nebliká: (23.1 mA) Svítí: (16.22 mA) // je fakt divný že když svítí má menší spotøebu než když nesvítí
	// 1MHz:   Bliká: (5.55 mA)          Nebliká: ( mA) Svítí: (6.85 mA)

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

