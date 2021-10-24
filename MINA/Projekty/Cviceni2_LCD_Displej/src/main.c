#include "mina_shield_mbed.h"
#include "system_stm32f4xx.h"
#include "nucleo_usart.h"
#include "mbed_shield_lcd.h"


volatile uint32_t ticks = 0; // promìná která symbolizuje dobìhnutí hodin, "volatile" znamená že kompilátor tuto promìnou nebude optimalizovat, to je tøeba, protože by jinak mohl kompilátor dojít k chybným závìrùm o fknci nìkterých programových konstrukcí a nesprávnì program optimalizovat

void SysTick_Handler(void) // tato funkce musí mít pøesnì takovýto název, jinak se pøi pøerušení volá defalutnì nastavená
{
ticks++;
}


int main(void) {

	SystemCoreClockUpdate();

	uint32_t timeBase = 1000;

	GPIOConfigurePin(RGBLED_BLUE,ioPortOutputPushPull);

	SysTick_Config(SystemCoreClock/1000); // nastavení èasovaèe pro pøerušení, jeden systick je jedna milisekunda, kdybych nechal pouze (SystemCoreClock) tak jeden dobìh systicku bude jedna sekunda


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

