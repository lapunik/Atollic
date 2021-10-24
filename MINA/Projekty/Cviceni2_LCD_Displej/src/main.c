#include "mina_shield_mbed.h"
#include "system_stm32f4xx.h"
#include "nucleo_usart.h"
#include "mbed_shield_lcd.h"


volatile uint32_t ticks = 0; // prom�n� kter� symbolizuje dob�hnut� hodin, "volatile" znamen� �e kompil�tor tuto prom�nou nebude optimalizovat, to je t�eba, proto�e by jinak mohl kompil�tor doj�t k chybn�m z�v�r�m o fknci n�kter�ch programov�ch konstrukc� a nespr�vn� program optimalizovat

void SysTick_Handler(void) // tato funkce mus� m�t p�esn� takov�to n�zev, jinak se p�i p�eru�en� vol� defalutn� nastaven�
{
ticks++;
}


int main(void) {

	SystemCoreClockUpdate();

	uint32_t timeBase = 1000;

	GPIOConfigurePin(RGBLED_BLUE,ioPortOutputPushPull);

	SysTick_Config(SystemCoreClock/1000); // nastaven� �asova�e pro p�eru�en�, jeden systick je jedna milisekunda, kdybych nechal pouze (SystemCoreClock) tak jeden dob�h systicku bude jedna sekunda


	if(!MBED_LCD_init())
	{

		while(1);

	}


      MBED_LCD_InitVideoRam(0x00);      // fill content with 0 = clear memory buffer

     // char* buf;

	  // sprintf(buf, "Disp: %02dx%02d pix", MBED_LCD_GetColumns(), MBED_LCD_GetRows());
	 //  MBED_LCD_WriteStringXY(buf, 0, 2);    // example string output


	  MBED_LCD_WriteStringXY("Lapi je nejlepsi", 0, 2);

	  MBED_LCD_VideoRam2LCD();          // move changes in video buffer to LCD


	  MBED_LCD_DrawCircle(80, 16, 8, true);     // sample drawing

	  MBED_LCD_DrawRect(20,0,20,20,true);

	  MBED_LCD_VideoRam2LCD();         // move changes in video buffer to LCD


	while (1)
	{



		if (ticks >= timeBase)
		{

		      timeBase = ticks + 1000;

		}


    }


}

